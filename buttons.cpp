
#include "module.h"

// #define DEBUG_MODE

extern uint32_t count;
extern byte channel;

typedef struct settings {
  const byte pin;
  const byte midi;
  const char* name;
} settings_t;

// Buttons: PedalR / PedalL
#define BUTTONS 3
const settings_t settings[] = {
  {13, 123, "Pedal 2"},
  {12,  64, "Pedal 1"},
  {11,  68, "Joy Click"}, // ? 68 69 ?
};

class ButtonsModule : public AbstractModule {
public:
  virtual void setup() {
    for (int i = 0; i < BUTTONS; i++) {
      button_status[i] = true; // force init
      pinMode(settings[i].pin, INPUT_PULLUP);
    }
  }

  virtual void loop() {
    if (count % 64 != 32) // run once / 64 cycles in alternance with analogs
      return;

    for (int i = 0; i < BUTTONS; i++) {
      byte s = !digitalRead(settings[i].pin);
      if (button_status[i] != s) {
        buttonChanged(i, s);
      }
    }
  }

private:
  byte button_status[BUTTONS];

  void buttonChanged(const byte i, const byte s) {
    const settings_t &set = settings[i];
    button_status[i] = s;
    MIDI.sendControlChange(set.midi, (button_status[i] ? 127 : 0), channel);
#ifdef DEBUG_MODE
    Serial.print(set.name);
    Serial.print(": ");
    Serial.print(i);
    Serial.print(" MIDI: ");
    Serial.print(set.midi);
    Serial.println(button_status[i] ? " true" : " false");
#endif
  }
};

AbstractModule* buttons = new ButtonsModule();
