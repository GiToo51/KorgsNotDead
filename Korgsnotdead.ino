
#include <MIDI.h>
#include "module.h"

// #define DEBUG_MODE
// #define DEBUG_SPEED

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

uint32_t count;
uint32_t running;
byte channel; // 16: Focus, default, first light/button, MIDI/TRANSPOSE button

#ifdef DEBUG_MODE
uint32_t now;
uint32_t speed_test;
#endif

void setup() {
  running = false;
  count = 0;
  channel = 16;
  MIDI.begin(16);

  // setup
  keyboard->setup();
  buttons->setup();
  analogs->setup();
  korgbuttonsnleds->setup();

  // first scan
  keyboard->loop();
  buttons->loop();
  analogs->loop();
  korgbuttonsnleds->loop();

#ifdef DEBUG_MODE
  Serial.begin(9600);
#endif

#ifdef DEBUG_SPEED
  now = micros();
  speed_test = micros();
#endif

  count++;
  running = true;
}


void loop() {
  keyboard->loop();
  buttons->loop();
  analogs->loop();
  korgbuttonsnleds->loop();

  count++;

#ifdef DEBUG_SPEED
  now = micros();

  if (count % 1024 == 0) {
    Serial.println((now - speed_test) / 1024);
    speed_test = now;
  }
#endif
}
