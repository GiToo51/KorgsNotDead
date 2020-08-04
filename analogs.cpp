
#include "module.h"

//#define DEBUG_MODE

extern uint32_t count;
extern byte channel;

// Analogs: Vol / X / Y
#define ANALOGS 1 //3
#define ANALOG_HYSTERESIS 3
uint16_t   analogs_source[ANALOGS];
byte       analogs_values[ANALOGS];
const byte analog_pins[] = {A0, A1, A2};
const byte analog_midi[] = { 7,  1,  11};

class AnalogsModule : public AbstractModule {
public:
  virtual void setup() {
    for ( int i = 0; i < ANALOGS; i++) {
      analogs_source[i] = 0;
      analogs_values[i] = 0;
      pinMode(analog_pins[i], INPUT);
    }
  }

  virtual void loop() {
    if (count % 64 != 0) // run once / 64 cycles
      return;

    for (int i = 0; i < ANALOGS; i++) {
      uint16_t a = analogRead(analog_pins[i]);
      if (abs(analogs_source[i] - a) > ANALOG_HYSTERESIS) { // protect to send too much messages
        analogs_source[i] = a;
        byte v = 127 - (a / 8);
        if (analogs_values[i] != v) {
          analogs_values[i] = v;
#ifdef DEBUG_MODE
          Serial.print("Analog ");
          Serial.print(i);
          Serial.print(" MIDI: ");
          Serial.print(analog_midi[i]);
          Serial.print(" Val: ");
          Serial.println(v);
#endif
          MIDI.sendControlChange(analog_midi[i], v, channel);
        }
      }
    }
  }
};

AbstractModule* analogs = new AnalogsModule();
