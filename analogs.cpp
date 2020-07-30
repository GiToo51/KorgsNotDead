
#include "module.h"

// #define DEBUG_MODE

// Analogs: Vol / X / Y
#define ANALOGS 3
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
      pinMode(analog_pins[i], INPUT_PULLUP);
    }
  }

  virtual void loop() {
    for (int i = 0; i < ANALOGS; i++) {
      uint16_t a = 127 - analogRead(analog_pins[i]) / 8;
      if (abs(analogs_source[i] - a) > ANALOG_HYSTERESIS) { // protect to send too much messages
        analogs_source[i] = a;
        byte v = 127 - a / 8;
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
          MIDI.sendControlChange(analog_midi[i], v, 1);
        }
      }
    }
  }
};

AbstractModule* analogs = new AnalogsModule();

