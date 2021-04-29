#pragma once

// MIDI port
#include <MIDI.h>

#define CLOCKS_PER_TICK 24

class MIDIExt : public midi::MidiInterface<midi::SerialMIDI<HardwareSerial>, midi::DefaultSettings> {

public:
  MIDIExt();
  void setup();
  void scan();

  uint16_t lastClockSentAt;

private:
  uint16_t ticksPerMinutes; // copy from settings.clock.ticksPerMinutes for change detection
  void updateTimePerClock();
  uint16_t timePerClock; // time between 2 send Clock

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
