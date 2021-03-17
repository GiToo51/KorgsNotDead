
#include "defs.h"
#include "midi.h"
#include "settings.h"

//#define DEBUG_MODE

MIDIExt midiExt;

MIDIExt::MIDIExt() : MIDIExtInterface((midi::SerialMIDI<HardwareSerial>&)Serial3) {}

void MIDIExt::begin() {
  MIDIExtInterface::begin(MIDI_CHANNEL_OMNI);
  turnThruOff();
  setHandleError(&errorReceived);
  setHandleNoteOn(&noteOnReceived);
  setHandleNoteOff(&noteOffReceived);
  setHandleControlChange(&controlChange);
  setHandleProgramChange(&programChange);
  setHandleSystemExclusive(&systemExclusive);
  setHandleTimeCodeQuarterFrame(&timeCodeQuarterReceived);
  setHandleSongPosition(&songPositionReceived);
  setHandleSongSelect(&songSelectReceived);
  setHandleTuneRequest(&tuneRequestReceived);
  setHandleClock(&clockReceived);
  setHandleStart(&startReceived);
  setHandleTick(&tickReceived);
  setHandleContinue(&continueReceived);
  setHandleStop(&stopReceived);
  setHandleSystemReset(&systemResetReceived);
}

uint16_t lastClockSend;
    
void MIDIExt::checkLooperRestart() {
  if (settings.clock.loopTo > 0 && (settings.clock.pos >= settings.clock.loopTo || settings.clock.pos < settings.clock.loopFrom)) {
    midiExt.sendStop();
    settings.clock.pos = settings.clock.loopFrom;
    midiExt.sendSongPosition(settings.clock.pos/6);
    midiExt.sendContinue();
/*#ifdef DEBUG_MODE
  Serial.print("Restart ");
  Serial.print(settings.clock.loopFrom);
  Serial.print(" to ");
  Serial.println(settings.clock.loopTo);
#endif*/
  }
}


void MIDIExt::scan() {
  if (settings.clock.sender && now - lastClockSend >= timePerClock)  {
    // Sending clock
    settings.clock.pos += 1;
    if (settings.clock.pos % TICK_MEASURE_FACTOR == 0)
      checkLooperRestart();
    lastClockSend = now;
    midiExt.sendClock();
  } else {
    midiExt.read();
  }
}

void MIDIExt::errorReceived(int8_t err) {
#ifdef DEBUG_MODE
  Serial.print("Midi Error: ");
  Serial.print(err);
  Serial.println();
#endif
}

void MIDIExt::noteOnReceived(midi::Channel channel, byte note, byte velocity) {
    keysStatus[channel][note - settings.transposeValue - FIRST_KEY] |= _KEY_PRESSED | (sustainedChannel[channel] ? _KEY_SUSTAINED : 0) | _KEY_FLASHED;
}
void MIDIExt::noteOffReceived(midi::Channel channel, byte note, byte velocity) {
    keysStatus[channel][note - settings.transposeValue - FIRST_KEY] &= ~_KEY_PRESSED;
}

void MIDIExt::controlChange(midi::Channel channel, byte control, byte value) {
  if (control == midi::Sustain)
    sustainedChannel[channel] = value;
}
void MIDIExt::programChange(midi::Channel channel, byte a) {
#ifdef DEBUG_MODE
  Serial.print("SongSelect: ");
  Serial.print(a);
  Serial.println();
#endif
}
void MIDIExt::systemExclusive(byte * array, unsigned size) {
#ifdef DEBUG_MODE
  Serial.print("SongSelect: ");
  Serial.print(size);
  Serial.println();
#endif
}

void MIDIExt::timeCodeQuarterReceived(byte data) {
#ifdef DEBUG_MODE
  Serial.print("TimeCodeQuarter: ");
  Serial.print(data);
  Serial.println();
#endif
}

void MIDIExt::songPositionReceived(unsigned beats) {
  settings.clock.pos = (uint32_t)beats * 6;
#ifdef DEBUG_MODE
  Serial.print("SongPosition: ");
  Serial.print(beats);
  Serial.println();
#endif
}
void MIDIExt::songSelectReceived(byte songnumber) {
#ifdef DEBUG_MODE
  Serial.print("SongSelect: ");
  Serial.print(songnumber);
  Serial.println();
#endif
}
void MIDIExt::tuneRequestReceived() {
#ifdef DEBUG_MODE
  Serial.println("TuneRequest");
#endif
}

void MIDIExt::clockReceived() {
  if ( ! settings.clock.sender)
    settings.clock.pos += 1;
}
void MIDIExt::startReceived() {
  settings.clock.pos = 0;
#ifdef DEBUG_MODE
  Serial.println("Start");
#endif
}
void MIDIExt::tickReceived() {
#ifdef DEBUG_MODE
  Serial.println("Tick");
#endif
}
void MIDIExt::continueReceived() {
#ifdef DEBUG_MODE
  Serial.println("Continue");
#endif
}
void MIDIExt::stopReceived() {
#ifdef DEBUG_MODE
  Serial.println("Stop");
#endif
}
void MIDIExt::systemResetReceived() {
#ifdef DEBUG_MODE
  Serial.println("SystemReset");
#endif
}


#undef DEBUG_MODE
