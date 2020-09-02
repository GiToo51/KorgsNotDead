
#include "module.h"

//#define VELOCITY_DEBUG
//#define DEBUG_MODE
//#define VERBOSE_DEBUG_MODE

// this number can be negative !
// changed by korgbuttonsnleds.cpp
int16_t keyboardTranspose = 0;
// can be set by another file. Will be fire only once and reset after
void (*catchNextNoteCallback)(const byte note) = NULL;

// first number: min duration for max velocity
// second number: more you decrease, more you can play more slowly
// 127 - log(dur / 2700.0) * 49.0
//const uint16_t velocityTab[] = {2701,2756,2813,2871,2930,2991,3052,3115,3179,3245,3312,3380,3450,3521,3593,3668,3743,3820,3899,3979,4061,4145,4231,4318,4407,4498,4590,4685,4782,4880,4981,5084,5188,5295,5404,5516,5630,5746,5864,5985,6108,6234,6363,6494,6628,6765,6904,7046,7192,7340,7491,7646,7803,7964,8128,8296,8467,8641,8820,9001,9187,9376,9570,9767,9968,10174,10384,10598,10816,11039,11267,11499,11736,11978,12225,12477,12734,12997,13265,13538,13818,14102,14393,14690,14993,15302,15617,15939,16268,16603,16946,17295,17652,18016,18387,18766,19153,19548,19951,20362,20782,21211,21648,22094,22550,23015,23489,23973,24468,24972,25487,26012,26549,27096,27655,28225,28807,29401,30007,30626,31257,31902,32559,33231,33916,34615,35329,36057};

// log((i+10000.0) / 13000.0) * 100.0)
//const uint16_t velocityTab[] = {3001,3131,3263,3396,3531,3667,3804,3943,4083,4225,4368,4512,4658,4805,4954,5104,5256,5409,5564,5721,5879,6038,6199,6362,6527,6693,6861,7030,7201,7374,7549,7725,7903,8083,8265,8448,8634,8821,9010,9201,9394,9589,9786,9985,10186,10389,10593,10800,11009,11221,11434,11649,11867,12087,12309,12533,12759,12988,13219,13452,13688,13926,14167,14409,14655,14903,15153,15406,15661,15919,16179,16442,16708,16977,17248,17522,17798,18077,18360,18645,18933,19223,19517,19814,20113,20416,20722,21030,21342,21657,21975,22297,22621,22949,23280,23615,23953,24294,24638,24987,25338,25693,26052,26414,26780,27150,27523,27900,28281,28666,29055,29447,29844,30244,30648,31057,31470,31886,32307,32733,33162,33596,34034,34476,34923,35375,35831,36292};

// log((i-2500.0) / 1000.0 +1.0) * 32.0)
//const uint16_t velocityTab[] = {2501,2532,2565,2599,2634,2670,2707,2745,2785,2825,2867,2911,2955,3002,3049,3098,3149,3202,3256,3311,3369,3428,3489,3552,3618,3685,3754,3826,3899,3976,4054,4135,4219,4305,4394,4486,4581,4678,4779,4883,4991,5102,5216,5334,5456,5581,5711,5844,5982,6124,6271,6423,6579,6740,6906,7078,7255,7438,7626,7821,8021,8228,8442,8662,8890,9124,9366,9616,9873,10139,10413,10696,10988,11289,11600,11921,12252,12593,12945,13308,13683,14070,14469,14880,15305,15743,16195,16662,17143,17640,18152,18681,19226,19789,20369,20968,21586,22224,22881,23560,24260,24983,25728,26497,27291,28110,28954,29826,30725,31652,32610,33597,34616,35667,36752,37871,39025,40216,41445,42713,44022,45371,46764,48201,49683,51213,52791,54419};
// log((i-2500.0) / 1000.0 +1.0) * 36.0)
const uint16_t velocityTab[] = {2501,2529,2558,2587,2618,2649,2682,2715,2749,2785,2821,2858,2896,2935,2976,3017,3060,3104,3149,3196,3243,3293,3343,3395,3448,3503,3560,3618,3677,3738,3801,3866,3933,4001,4072,4144,4219,4295,4374,4455,4538,4624,4712,4802,4895,4991,5089,5190,5294,5401,5511,5624,5740,5859,5982,6108,6238,6372,6509,6650,6795,6944,7097,7255,7417,7584,7755,7931,8113,8299,8490,8687,8890,9098,9312,9532,9758,9990,10230,10476,10728,10988,11255,11530,11813,12103,12402,12709,13025,13349,13683,14026,14379,14742,15115,15498,15892,16298,16715,17143,17584,18037,18503,18981,19474,19980,20501,21036,21586,22152,22733,23332,23946,24579,25229,25897,26584,27291,28017,28764,29532,30322,31134,31968,32826,33709,34616,35549};

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
  byte     velo;
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
      keys[i].velo = 0;
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
      uint32_t now = micros();

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
              // set this only on key down, to release with the same code if transpose happen during key press
              keys[key].midi = key + keyboardTranspose + KORG_88_FIRST_KEY;
              keys[key].state = KEY_IS_DOWN;
              uint32_t diff = now > keys[key].since ? now - keys[key].since : UINT32_MAX - keys[key].since + now;
              keys[key].velo = velocity(diff);
            }
            break;
          case KEY_IS_DOWN:
            if (!(bottom & 1<<pos) && keys[key].midi) {
              keys[key].state = KEY_IS_GOING_UP;
            }
            break;
          case KEY_IS_GOING_UP:
            if (!(top & 1<<pos) && keys[key].midi) {
              keys[key].state = KEY_IS_UP;
              uint32_t now = micros();
              uint32_t diff = now > keys[key].since ? now - keys[key].since : UINT32_MAX - keys[key].since + now;
              keys[key].velo = velocity(diff);
            }
            break;
          case KEY_IS_CATCHED:
            if (!(top & 1<<pos) && !(bottom & 1<<pos))
              keys[key].state = KEY_IS_UP;
            break;
        }
      }
    }
    for (byte i = 0; i < OUTPUTS * INPUTS; i++) {
      if (keys[i].velo != 0) {
        if (keys[i].state == KEY_IS_DOWN) {
          MIDI1.sendNoteOn(keys[i].midi, keys[i].velo, channel);
#ifdef VELOCITY_DEBUG
          Serial.print(v, HEX);
          Serial.print(" : ");
          Serial.println(diff);
#endif
        } else {
          MIDI1.sendNoteOff(keys[i].midi, keys[i].velo, channel);
        }
        keys[i].velo = 0;
      }
    }
  }

private:
  byte velocity(uint32_t duration) {
    if (duration < 1000) return 1; // seems to be a bug, we send very low velocity
    for (byte i = 0; i < 127; i++)
      if (duration <= velocityTab[i])
        return 127 - i;
    return 2;
/*    float x = duration - 2100; // shortest push
    if (x < 0) x = 0;
    x = x / 50000;
    if (x > 1) x = 1;
    return 127 - (x * 128);*/
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
