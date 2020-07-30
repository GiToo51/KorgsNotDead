
#include <MIDI.h>
extern midi::MidiInterface<HardwareSerial> MIDI;

#define DEBOUNCE 16 // usec
// Korg 88keys default
#define KORG_88_FIRST_KEY 19

class AbstractModule {
public:
  virtual void setup() = 0;
  virtual void loop() = 0;
};

extern AbstractModule* analogs;
extern AbstractModule* buttons;
extern AbstractModule* keyboard;
extern AbstractModule* korgbuttonsnleds;
