#pragma once

#include <Arduino.h>

#define KEYS_NB 88
#define FIRST_KEY 21
#define PRE_KEYS_NB 2
#define POST_KEYS_NB 2

#define VELOCITY_FACTOR 1 // * TIME_MICROS_FACTOR = µs
#define VELOCITY_MINIMUM_UNITS 56 // * VELOCITY_FACTOR * TIME_MICROS_FACTOR = µs
#define VELOCITY_CURVE 32 // 1|1  1|2  1|4

#define _KEY_PRESSED   B00000001 // normal, pressed or not
#define _KEY_SUSTAINED B00000010 // if sustain pedal is pressed, we keep ligh on as long as pedal is pressed
#define _KEY_FLASHED   B00000100 // on first led computation, flash the fresh pressed

typedef enum {
  KEY_IS_UP         = 0,
  KEY_IS_GOING_DOWN = 1,  // We increment the timer in this state
  KEY_IS_DOWN       = 2,
  KEY_IS_GOING_UP   = 3,    // We increment the timer in this state
} state_t;
typedef struct {
  uint8_t  state;
  uint8_t  note;
  uint16_t since;
  uint16_t until;
} key_t;

class Keyboard {
public:
  void setup();
  bool scan();
  void sendNext();

  key_t keys[KEYS_NB];
  uint8_t keyNbPressed;
  uint8_t sustainedChannel[17];
  uint8_t keysStatus[17][KEYS_NB]; // 0: output, 1-16: input

private:
  void scanKey(uint8_t key, uint8_t pos);
  void scanFirstBloc();
  void scanBloc(uint8_t block);
  bool keyChangedDuringScan;
  uint8_t velocity(uint8_t key);

  // send note events are not done during scan loop (because we use micros() slow function only once per cycle)
  // So keys that produces events are queued there
  uint8_t sendQueue[256];
  uint8_t sendQueueIPos;
  uint8_t sendQueueOPos;

  uint8_t top;
  uint8_t bottom;
  uint8_t oldTops[15];
  uint8_t oldBottoms[15];
};

extern Keyboard keyboard;
