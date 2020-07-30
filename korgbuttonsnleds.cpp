
#include "module.h"

//#define DEBUG_MODE

extern uint32_t count;
extern  int16_t keyboardTranspose;
extern void (*catchNextNoteCallback)(const byte note);

void catchTransposeFromFunct(const byte note);
void catchTransposeToFunct(const byte note);

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

const byte ledlevels[] = {
  B00000000,
  B00010000,
  B01001000,
  B01010100,
  B01010101,
  B01101101,
  B01110111,
  B11111111,
};

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
    if (korgButtons != 0 && ! buttonComboReleasing) {
      for (byte i = 0; i < 8; i++) {
        korgLeds[i] = (korgButtons & (1<<i)) ? ledlevels[7] : ledlevels[0];
      }
    } else {
      // K2000 background
      byte pos = count / 128;
      if (pos & 1<<3)
        pos = pos % 8;
      else
        pos = 8 - pos % 8;

      for (byte i = 0; i < 8; i++) {
        if (pos == i)
          korgLeds[i] = ledlevels[7];
        else if (pos > i && pos - i < 4)
          korgLeds[i] = ledlevels[4 - (pos - i)];
        else if (i > pos && i - pos < 4)
          korgLeds[i] = ledlevels[4 - (i - pos)];
        else
          korgLeds[i] = ledlevels[0];
      }

      if (catchNextNoteCallback) {
        korgLeds[7] = count & 512 ? ledlevels[7] : ledlevels[0];
        korgLeds[6] = ~korgLeds[7];
      } else {
        if (keyboardTranspose > 0)
          korgLeds[7] = ledlevels[7];
        else if (keyboardTranspose < 0)
          korgLeds[6] = ledlevels[7];
      }
    }

    // 8lvl leds
    byte leds = 0;
    byte mask = 1<<(count%8);
    for (byte i = 0; i < 8; i++)
      if (korgLeds[i] & mask)
        leds |= 1<<i;

    // write leds
    digitalWrite(LEDS_N_BUTTONS_LATCH, LOW);
    delayMicroseconds(DEBOUNCE);
    shiftOut(LEDS_DATA, LEDS_SHIFT, MSBFIRST, leds);

    if (count % 64 != 32) // run once / 64 cycles
      return;

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

  void catchTransposeFrom(const byte note) {
    Serial.println("Catched From!");
    tmpTransposeNote = note;
    catchNextNoteCallback = &catchTransposeToFunct;
  }
  void catchTransposeTo(const byte note) {
    Serial.println("Catched To!");
    keyboardTranspose += tmpTransposeNote - note;
    catchNextNoteCallback = NULL;
  }

private:
  byte tmpTransposeNote;

  void fireButtons(const byte b) {
    // Transpose
    if ((b & B11000000) == B11000000) { // COMBO + & -
      if (catchNextNoteCallback) {
        catchNextNoteCallback = NULL;
        keyboardTranspose = 0;
      } else {
        catchNextNoteCallback = &catchTransposeFromFunct;
      }
    } else if (b & B10000000) { // Transpose +
      keyboardTranspose += 2;
    } else if (b & B01000000) { // transpose -
      keyboardTranspose -= 2;
    } else {
      MIDI.sendProgramChange(b, 1);
    }
#ifdef DEBUG_MODE
      Serial.print("Buttons: ");
      Serial.println(b, HEX);
#endif
  }
};

KorgbuttonsnledsModule* internalInstance = new KorgbuttonsnledsModule();
AbstractModule* korgbuttonsnleds = internalInstance;

void catchTransposeFromFunct(const byte note) {
  internalInstance->catchTransposeFrom(note);
}
void catchTransposeToFunct(const byte note) {
  internalInstance->catchTransposeTo(note);
}
