
#include <MIDI.h>
#include "module.h"

// #define DEBUG_MODE

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

uint16_t count;
uint16_t now;

#ifdef DEBUG_MODE
uint16_t speed_test;
#endif

void setup() {
  count = 0;
  Serial.begin(9600);
  MIDI.begin(1);

  keyboard->setup();
  buttons->setup();
  analogs->setup();
  korgbuttonsnleds->setup();

#ifdef DEBUG_MODE
#define DEBUG_CYCLES 1024
  speed_test = millis();
#endif
}


void loop() {
  now = millis();

#ifdef DEBUG_MODE
  if (count % DEBUG_CYCLES == 0) {
    Serial.println((double)(now - speed_test) / DEBUG_CYCLES, 3);
    speed_test = now;
  }
#endif

  keyboard->loop();
  buttons->loop();
  analogs->loop();
  korgbuttonsnleds->loop();
  count++;
}

