
#include "midi.h"
#include "menu.h"
#include "kbd.h"
#include "timer.h"
#include "settings.h"

//#define DEBUG_MODE

MIDIExt midiExt;

midi::SerialMIDI<HardwareSerial> midiExtInterface(Serial3);

MIDIExt::MIDIExt() : midi::MidiInterface<midi::SerialMIDI<HardwareSerial>, midi::DefaultSettings>(midiExtInterface) {}

void MIDIExt::setup() {
  begin(MIDI_CHANNEL_OMNI);
  turnThruOff();
  setHandleNoteOn(&noteOnReceived);
  setHandleNoteOff(&noteOffReceived);
  setHandleControlChange(&controlChange);
  setHandleClock(&clockReceived);
  setHandleStart(&startReceived);
#ifdef DEBUG_MODE
  setHandleError(&errorReceived);
  setHandleSongPosition(&songPositionReceived);
  setHandleSongSelect(&songSelectReceived);
  setHandleProgramChange(&programChange);
  setHandleSystemExclusive(&systemExclusive);
  setHandleTimeCodeQuarterFrame(&timeCodeQuarterReceived);
  setHandleTuneRequest(&tuneRequestReceived);
  setHandleTick(&tickReceived);
  setHandleContinue(&continueReceived);
  setHandleStop(&stopReceived);
  setHandleSystemReset(&systemResetReceived);
#endif
  updateTimePerClock();
  lastClockSentAt = timer.now.now16;
}

// time units per beat in bpm
void MIDIExt::updateTimePerClock() {
  ticksPerMinutes  = settings.clock.ticksPerMinutes;
  timePerClock = (1000000/TIME_MICROS_FACTOR) / (ticksPerMinutes*CLOCKS_PER_TICK/60);
}

uint16_t lastClockSend;
    
void MIDIExt::checkLooperRestart() {
  if (settings.clock.loopTo > 0 && (settings.clock.pos >= settings.clock.loopTo || settings.clock.pos < settings.clock.loopFrom)) {
    sendStop();
    settings.clock.pos = settings.clock.loopFrom;
    sendSongPosition(settings.clock.pos/6);
    sendContinue();
/*#ifdef DEBUG_MODE
  Serial.print("Restart ");
  Serial.print(settings.clock.loopFrom);
  Serial.print(" to ");
  Serial.println(settings.clock.loopTo);
#endif*/
  }
}


void MIDIExt::scan() {
  if (settings.clock.sender && timer.now.now16 - lastClockSentAt >= timePerClock)  {
    if (ticksPerMinutes != settings.clock.ticksPerMinutes)
      updateTimePerClock();
    // Sending clock
    settings.clock.pos += 1;
    if (settings.clock.pos % TICK_MEASURE_FACTOR == 0)
      checkLooperRestart();
    lastClockSentAt = timer.now.now16;
    sendClock();
  } else {
    read();
  }
}

void MIDIExt::noteOnReceived(midi::Channel channel, byte note, byte) {
    keyboard.keysStatus[channel][note - settings.transposeValue - FIRST_KEY] |= _KEY_PRESSED | (keyboard.sustainedChannel[channel] ? _KEY_SUSTAINED : 0) | _KEY_FLASHED;
#ifdef DEBUG_MODE
  Serial.println("noteOnReceived");
#endif
}
void MIDIExt::noteOffReceived(midi::Channel channel, byte note, byte) {
    keyboard.keysStatus[channel][note - settings.transposeValue - FIRST_KEY] &= ~_KEY_PRESSED;
}

void MIDIExt::controlChange(midi::Channel channel, byte control, byte value) {
  if (control == midi::Sustain)
    keyboard.sustainedChannel[channel] = value;
}

void MIDIExt::clockReceived() {
  if ( ! settings.clock.sender)
    settings.clock.pos += 1;
#ifdef DEBUG_MODE
  Serial.print('.');
#endif
}
void MIDIExt::startReceived() {
  settings.clock.pos = 0;
#ifdef DEBUG_MODE
  Serial.println("Start");
#endif
}

#ifdef DEBUG_MODE
void MIDIExt::errorReceived(int8_t err) {
  Serial.print("Midi Error: ");
  Serial.print(err);
  Serial.println();
}
void MIDIExt::songPositionReceived(unsigned beats) {
  settings.clock.pos = (uint32_t)beats * 6;
  Serial.print("SongPosition: ");
  Serial.print(beats);
  Serial.println();
}
void MIDIExt::songSelectReceived(byte songnumber) {
  Serial.print("SongSelect: ");
  Serial.print(songnumber);
  Serial.println();
}
void MIDIExt::tuneRequestReceived() {
  Serial.println("TuneRequest");
}
void MIDIExt::programChange(midi::Channel, byte a) {
  Serial.print("SongSelect: ");
  Serial.print(a);
  Serial.println();
}
void MIDIExt::systemExclusive(byte *, unsigned size) {
  Serial.print("SongSelect: ");
  Serial.print(size);
  Serial.println();
}
void MIDIExt::timeCodeQuarterReceived(byte data) {
  Serial.print("TimeCodeQuarter: ");
  Serial.print(data);
  Serial.println();
}
void MIDIExt::tickReceived() {
  Serial.println("Tick");
}
void MIDIExt::continueReceived() {
  Serial.println("Continue");
}
void MIDIExt::stopReceived() {
  Serial.println("Stop");
}
void MIDIExt::systemResetReceived() {
  Serial.println("SystemReset");
}
#endif


#undef DEBUG_MODE
