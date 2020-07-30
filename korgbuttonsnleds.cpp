
#include "module.h"

// #define DEBUG_MODE

extern uint16_t count;

//#define DEBUG_MODE

// Korg CN12: Mega 2560
//         6: GND
//         7: VCC
//         8: 30 SCK Leds & buttons Latch : orange
//         9: 50 Buttons        Shift : yellow          
//        10: 51 Leds           Shift : white
//        11: 52 Led            Data  : green
//        12: 53 Button         Data : blue
#define LEDS_N_BUTTONS_LATCH 30
#define BUTTONS_SHIFT        50
#define LEDS_SHIFT           51
#define LEDS_DATA            52
#define BUTTONS_DATA         53
// Leds: Hall / Room / Organ / Harp / E.Piano / A.Piano / MIDI Trans
byte korgLeds[8];
byte korgButtons;
byte buttonComboReleasing; // because we should fire events on button combo releasing slowly

class KorgbuttonsnledsModule : public AbstractModule {
public:
  virtual void setup() {
    korgButtons  = 0;
    buttonComboReleasing = 1;
    pinMode(LEDS_N_BUTTONS_LATCH, OUTPUT);
    pinMode(BUTTONS_SHIFT       , OUTPUT);
    pinMode(LEDS_SHIFT          , OUTPUT);
    pinMode(LEDS_DATA           , OUTPUT);
    pinMode(BUTTONS_DATA        , INPUT );
  }

  virtual void loop() {
    if (korgButtons != 0 && ! buttonComboReleasing)
      for (byte i = 0; i < 8; i++) {
        korgLeds[i] = (korgButtons & (1<<i)) ? 255 : 0;
      }
    else
      for (byte i = 0; i < 8; i++) {
        byte diff = abs((double)(count / 64 % 8) - i);
        korgLeds[i] = 64 >> diff;
      }
    
    // write leds
    digitalWrite(LEDS_N_BUTTONS_LATCH, LOW);
    delayMicroseconds(DEBOUNCE);
    byte leds = 0;
    for (byte i = 0; i < 8; i++)
      if (count % 8 < korgLeds[i] / 16)
        leds |= 1<<i;
    shiftOut(LEDS_DATA, LEDS_SHIFT, MSBFIRST, leds);

    // tell the buttons to load the data during the same time
    shiftIn(BUTTONS_DATA, BUTTONS_SHIFT, MSBFIRST);
    digitalWrite(LEDS_N_BUTTONS_LATCH, HIGH);
    delayMicroseconds(DEBOUNCE);

    // read buttons
    byte b = ~shiftIn(BUTTONS_DATA, BUTTONS_SHIFT, MSBFIRST);
    if (korgButtons > b && ! buttonComboReleasing) { // releasing, but not second release
      fireButtons(korgButtons);
      buttonComboReleasing = 1;
    } else if (korgButtons < b) { // new button pushed
      buttonComboReleasing = 0;
    }
    korgButtons = b;
  }

private:

  void fireButtons(const byte b) {
#ifdef DEBUG_MODE
      Serial.print("Buttons: ");
      Serial.println(b, HEX);
#endif
  }
};

AbstractModule* korgbuttonsnleds = new KorgbuttonsnledsModule();

