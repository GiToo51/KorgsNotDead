
// MIDI port
#include <MIDI.h>

typedef midi::MidiInterface<midi::SerialMIDI<HardwareSerial>, midi::DefaultSettings> MIDIExtInterface;

class MIDIExt : public MIDIExtInterface {

public:
  MIDIExt();
  void begin();
  void scan();

private:
  void checkLooperRestart();
  static void errorReceived(int8_t err);
  static void noteOnReceived(midi::Channel channel, byte note, byte velocity);
  static void noteOffReceived(midi::Channel channel, byte note, byte velocity);
  static void controlChange(midi::Channel channel, byte control, byte value);
  static void programChange(midi::Channel channel, byte a);
  static void systemExclusive(byte * array, unsigned size);
  static void timeCodeQuarterReceived(byte data);
  static void songPositionReceived(unsigned beats);
  static void songSelectReceived(byte songnumber);
  static void tuneRequestReceived();
  static void clockReceived();
  static void startReceived();
  static void tickReceived();
  static void continueReceived();
  static void stopReceived();
  static void systemResetReceived();
};

extern MIDIExt midiExt;
