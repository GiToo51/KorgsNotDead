
#include "module.h"

//#define DEBUG_MODE

typedef enum {
  KEY_IS_UP,
  KEY_IS_GOING_DOWN,  // We increment the timer in this state
  KEY_IS_DOWN,
  KEY_IS_GOING_UP,    // We increment the timer in this state
} state_t;

typedef struct {
  uint16_t since;
  state_t  state;
} key_t;

extern uint16_t now;

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
              keys[key].state = KEY_IS_GOING_DOWN;
              keys[key].since = now;
#ifdef DEBUG_MODE
              Serial.print('#');
              Serial.print(key + 21, DEC);
              Serial.println(": KEY_IS_GOING_DOWN");
#endif
            }
            break;
          case KEY_IS_GOING_DOWN:
            if (!(top & 1<<pos)) {
              keys[key].state = KEY_IS_UP; // ignored unfinished press
            } else if (bottom & 1<<pos) {
              keys[key].state = KEY_IS_DOWN;
              uint16_t v = velocity(now - keys[key].since);
              MIDI.sendNoteOn(key + 21, v, 1);
#ifdef DEBUG_MODE
              Serial.print('#');
              Serial.print(key + 21, DEC);
              Serial.print(": KEY_IS_DOWN: ");
              Serial.println(v, DEC);
#endif
            }
            break;
          case KEY_IS_DOWN:
            if (!(bottom & 1<<pos)) {
              keys[key].state = KEY_IS_GOING_UP;
              keys[key].since = now;
#ifdef DEBUG_MODE
              Serial.print('#');
              Serial.print(key + 21, DEC);
              Serial.println(": KEY_IS_GOING_UP");
#endif
            }
            break;
          case KEY_IS_GOING_UP:
            if (!(top & 1<<pos)) {
              keys[key].state = KEY_IS_UP;
              uint16_t v = velocity(now - keys[key].since);
              MIDI.sendNoteOff(key + 21, v, 1);
#ifdef DEBUG_MODE
              Serial.print('#');
              Serial.print(key + 21, DEC);
              Serial.print(": KEY_IS_UP: ");
              Serial.println(v, DEC);
#endif
            }
            break;
        }
      }
    }
  }

private:

  byte velocity(uint16_t iduration) {
    double duration = iduration;
    double velocity = log(duration-1) * 38.0;
    if (velocity < 0)
      velocity = 0;
    if (velocity >= 126)
      velocity = 126; // 126 because 0 do not play the note
    return 127 - velocity;
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

