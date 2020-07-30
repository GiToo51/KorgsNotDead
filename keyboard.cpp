
#include "module.h"

//#define DEBUG_MODE
//#define VERBOSE_DEBUG_MODE

// this number can be negative !
// changed by korgbuttonsnleds.cpp
int16_t keyboardTranspose = 0;
// can be set by another file. Will be fire only once and reset after
void (*catchNextNoteCallback)(const byte note) = NULL;

// log(i / 2500.0) * 51.0
// const uint16_t velocityTab[] = {2501,2550,2600,2652,2704,2758,2813,2868,2925,2983,3042,3102,3164,3226,3290,3355,3422,3490,3559,3629,3701,3774,3849,3925,4003,4082,4163,4245,4329,4415,4503,4592,4683,4775,4870,4966,5065,5165,5267,5371,5478,5586,5697,5810,5925,6042,6162,6284,6408,6535,6664,6796,6931,7068,7208,7351,7496,7645,7796,7950,8108,8268,8432,8599,8769,8943,9120,9301,9485,9672,9864,10059,10258,10462,10669,10880,11095,11315,11539,11768,12001,12238,12481,12728,12980,13237,13499,13766,14039,14317,14600,14889,15184,15485,15791,16104,16423,16748,17080,17418,17763,18114,18473,18839,19212,19592,19980,20376,20779,21191,21610,22038,22475,22920,23374,23836,24308,24790,25281,25781,26292,26812,27343,27885,28437,29000,29574,30160};
// log(i / 2400.0) * 48.0
//const uint16_t velocityTab[] = {2401,2451,2503,2555,2609,2664,2720,2777,2836,2895,2956,3019,3082,3147,3213,3281,3350,3420,3492,3566,3641,3718,3796,3876,3957,4041,4126,4213,4301,4392,4484,4579,4675,4773,4874,4977,5081,5188,5297,5409,5523,5639,5758,5879,6003,6129,6258,6390,6524,6662,6802,6945,7091,7241,7393,7549,7708,7870,8035,8205,8377,8554,8734,8918,9105,9297,9493,9693,9897,10105,10318,10535,10757,10983,11214,11450,11691,11937,12189,12445,12707,12975,13248,13527,13812,14102,14399,14702,15012,15328,15650,15980,16316,16660,17011,17369,17734,18108,18489,18878,19275,19681,20095,20518,20950,21391,21842,22302,22771,23250,23740,24240,24750,25271,25803,26346,26901,27467,28045,28636,29238,29854,30482,31124,31779,32448,33131,33829};
// log(i / 2600.0) * 51.0
const uint16_t velocityTab[] = {2601,2652,2704,2758,2813,2868,2925,2983,3042,3102,3164,3226,3290,3355,3422,3490,3559,3629,3701,3774,3849,3925,4003,4082,4163,4245,4329,4415,4503,4592,4683,4775,4870,4966,5065,5165,5267,5371,5478,5586,5697,5810,5925,6042,6162,6284,6408,6535,6664,6796,6931,7068,7208,7351,7496,7645,7796,7950,8108,8268,8432,8599,8769,8943,9120,9301,9485,9673,9864,10059,10259,10462,10669,10880,11095,11315,11539,11768,12001,12238,12481,12728,12980,13237,13499,13766,14039,14317,14600,14889,15184,15485,15791,16104,16423,16748,17080,17418,17763,18115,18473,18839,19212,19592,19980,20376,20779,21191,21611,22038,22475,22920,23374,23837,24309,24790,25281,25781,26292,26812,27343,27885,28437,29000,29574,30160,30757,31366};
typedef enum {
  KEY_IS_UP,
  KEY_IS_GOING_DOWN,  // We increment the timer in this state
  KEY_IS_DOWN,
  KEY_IS_GOING_UP,    // We increment the timer in this state
  KEY_IS_CATCHED,
} state_t;

typedef struct {
  uint32_t since;
  byte     midi;
  state_t  state;
} key_t;

extern uint32_t now;

// Keyboard matrix: 15*6*2
#define OUTPUTS 15
#define INPUTS 6
key_t keys[OUTPUTS * INPUTS];

class KeyboardModule : public AbstractModule {
public:
  virtual void setup() {
    // Init keyboard
    for ( int i = 0; i < OUTPUTS * INPUTS; i++) {
      keys[i].since = 0;
      keys[i].midi = 0;
      keys[i].state = KEY_IS_UP;
    }
    // OUTPUTS: 1pin per half Octave (6 keys): 15pins \\
    // Korg CN10:  1- 8
    // Mega 2560: 22-29
    DDRA = B11111111;
    // Korg CN10:  9-15
    // Mega 2560: 37-31 // reversed !!
    DDRC |= B11111111;

    // INPUTS + pull-up: 12pins: 2x6 \\
    // TOP
    // Korg CN9 :  1- 6
    // Mega 2560: 49-44 // reversed !!
    DDRL = B00000000;
    // Pull Up
    PORTL = B11111111;

    // BOTTOM
    // Korg CN9 :  7- 8- 9-10-11-12
    // Mega 2560: 41-40-39-43-42-38
    // PIN: Reg
    //  38: PD7
    //  39: PG2
    //  40: PG1
    //  41: PG0
    //  42: PL7
    //  43: PL6
    DDRD  &= B01111111;
    PORTD |= B10000000;
    DDRG  &= B11111000;
    PORTG |= B00000111;
    //DDRL  &= B00111111;
    //PORTL |= B11000000;
  }

  virtual void loop() {
    for (uint16_t addr = 0; addr < OUTPUTS; addr++) {
      writeKeysAddr(1 << addr);
      byte top = readKeysTop();
      byte bottom = readKeysBottom();
  
      for (int pos = 0; pos < INPUTS; pos++) {
        byte key = addr * (INPUTS) + pos;

        switch (keys[key].state) { // on previous state
          case KEY_IS_UP:
            if (top & 1<<pos) {
              if (catchNextNoteCallback != NULL) {
                (*catchNextNoteCallback)(key + keyboardTranspose + KORG_88_FIRST_KEY);
                keys[key].state = KEY_IS_CATCHED;
              } else {
                keys[key].state = KEY_IS_GOING_DOWN;
                keys[key].since = now;
                // set this only on key down, to release with the same code if transpose happen during key press
                keys[key].midi = key + keyboardTranspose + KORG_88_FIRST_KEY;
#ifdef VERBOSE_DEBUG_MODE
                Serial.print('#');
                Serial.print(keys[key].midi, DEC);
                Serial.println(": KEY_IS_GOING_DOWN");
#endif
              }
            }
            break;
          case KEY_IS_GOING_DOWN:
            if (!(top & 1<<pos) && keys[key].midi) {
              keys[key].state = KEY_IS_UP; // ignored unfinished press
            } else if (bottom & 1<<pos) {
              keys[key].state = KEY_IS_DOWN;
              byte v = now > keys[key].since ? velocity(now - keys[key].since) : velocity(UINT32_MAX - keys[key].since + now);
              MIDI.sendNoteOn(keys[key].midi, v, 1);
#ifdef DEBUG_MODE
              Serial.print('#');
              Serial.print(keys[key].midi, DEC);
              Serial.print(": KEY_IS_DOWN: ");
              Serial.print(now - keys[key].since);
              Serial.print("us v: ");
              Serial.println(v);
#endif
            }
            break;
          case KEY_IS_DOWN:
            if (!(bottom & 1<<pos) && keys[key].midi) {
              keys[key].state = KEY_IS_GOING_UP;
#ifdef VERBOSE_DEBUG_MODE
              Serial.print('#');
              Serial.print(keys[key].midi, DEC);
              Serial.println(": KEY_IS_GOING_UP");
#endif
            }
            break;
          case KEY_IS_GOING_UP:
            if (!(top & 1<<pos) && keys[key].midi) {
              keys[key].state = KEY_IS_UP;
              byte v = velocity(now - keys[key].since);
              MIDI.sendNoteOff(keys[key].midi, v, 1);
#ifdef VERBOSE_DEBUG_MODE
              Serial.print('#');
              Serial.print(keys[key].midi, DEC);
              Serial.print(": KEY_IS_UP: ");
              Serial.println(v, DEC);
#endif
            }
            break;
          case KEY_IS_CATCHED:
            if (!(top & 1<<pos) && !(bottom & 1<<pos))
              keys[key].state = KEY_IS_UP;
            break;
        }
      }
    }
  }

private:
  byte velocity(uint16_t duration) {
    for (byte i = 0; i < 127; i++)
      if (duration <= velocityTab[i])
        return 127 - i;
    return 1;
  }

  void writeKeysAddr(uint16_t x) {
    byte a = x % 256;
    PORTA = ~a;
    byte c = x / 256;
    PORTC = ~c;
    delayMicroseconds(DEBOUNCE);
  }

  byte readKeysTop() {
    return ~PINL & B00111111;
  }

  byte readKeysBottom() {
    return ((~PING & B00000111))|
           ((~PINL & B11000000) >> 3)|
           ((~PIND & B10000000) >> 2);
  }
};

AbstractModule* keyboard = new KeyboardModule();
