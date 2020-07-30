
#include <MIDI.h>
#include "module.h"

//#define DEBUG_MODE

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

uint32_t count;
uint32_t running;
uint32_t now; // = micros();

#ifdef DEBUG_MODE
uint32_t speed_test;
#endif

void setup() {
  running = false;
  count = 0;
  MIDI.begin(1);

  // setup
  keyboard->setup();
  buttons->setup();
  analogs->setup();
  korgbuttonsnleds->setup();

  // first scann
  now = micros();
  keyboard->loop();
  buttons->loop();
  analogs->loop();
  korgbuttonsnleds->loop();

#ifdef DEBUG_MODE
#define DEBUG_CYCLES 1024
  Serial.begin(9600);
  speed_test = micros();
#endif

  count++;
  running = true;
}


void loop() {
  now = micros();

  keyboard->loop();
  buttons->loop();
  analogs->loop();
  korgbuttonsnleds->loop();

/*  if (MIDI.read()) {
    Serial.print(MIDI.getType());
    Serial.print(' ');
    Serial.print(MIDI.getChannel());
    Serial.print(' ');
    Serial.print(MIDI.getData1());
    Serial.print(' ');
    Serial.print(MIDI.getData2());
    Serial.print(' ');
//    Serial.print(MIDI.getSysExArray());
//    Serial.print(' ');
    Serial.print(MIDI.getSysExArrayLength());
    Serial.println(' ');
  }  */
  count++;

#ifdef DEBUG_MODE
  if (count % DEBUG_CYCLES == 0) {
    Serial.println((now - speed_test) / DEBUG_CYCLES, 1);
    speed_test = now;
  }
#endif
}
