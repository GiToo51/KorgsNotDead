
//#define DEBUG_MODE

#include "defs.h"
#include "midi.h"
#include "menu.h"
#include "settings.h"
#include "pedals.h"

void Pedals::begin() {
//  analogReference(DEFAULT);
  analogReference(INTERNAL2V56); // because 20k pullup and 50k yamaha pedal (max 3.57V) set EXTERNAL to 3.3V ?
  pinMode(A10, INPUT);
  // port init done in keyboard
  memset(status, 0, sizeof(status));
  memset(newStatus, 0, sizeof(newStatus));
  memset(analogValue, 0, sizeof(analogValue));
  memset(analogChangedAt, 0, sizeof(analogChangedAt));
  memset(statusChangedAt, 0, sizeof(statusChangedAt));
  pedalScanPos = 0;
}

void Pedals::scanAnalogA() {
  if (nowH == analogChangedAt[PEDAL_I_EXTA]) return;

  uint16_t newAnalog = PEDAL_ANALOG_READ(A10);
  int16_t diff = newAnalog - analogValue[PEDAL_I_EXTA];
  if (diff < 0) diff = -diff;
  if (diff > PEDAL_ANALOG_PRECISION) {
    analogValue[PEDAL_I_EXTA] = newAnalog;
    analogChangedAt[PEDAL_I_EXTA] = nowH;
    midiExt.sendControlChange(midi::EffectControl1, (uint8_t)(newAnalog/128%128), settings.outputChannel);
    midiExt.sendControlChange(44,                   (uint8_t)(newAnalog%128), settings.outputChannel);
#ifdef DEBUG_MODE
    Serial.print("PedalA analog: ");
    Serial.print(newAnalog);
    Serial.print(' ');
    Serial.print(newAnalog/128);
    Serial.print(' ');
    Serial.println(newAnalog%128);
#endif
  }
}

void Pedals::scanAnalogB() {
  if (nowH == analogChangedAt[PEDAL_I_EXTB]) return;

  uint16_t newAnalog = PEDAL_ANALOG_READ(A11);
  int16_t diff = newAnalog - analogValue[PEDAL_I_EXTB];
  if (diff < 0) diff = -diff;
  if (diff > PEDAL_ANALOG_PRECISION) {
    analogValue[PEDAL_I_EXTB]  = newAnalog;
    analogChangedAt[PEDAL_I_EXTB] = nowH;
    midiExt.sendControlChange(midi::EffectControl2, (uint8_t)(newAnalog/128), settings.outputChannel);
    midiExt.sendControlChange(45,                   (uint8_t)(newAnalog%128), settings.outputChannel);
#ifdef DEBUG_MODE
    Serial.print("PedalB analog: ");
    Serial.print(newAnalog);
    Serial.print(' ');
    Serial.print(newAnalog/128);
    Serial.print(' ');
    Serial.println(newAnalog%128);
#endif
  }
}

void Pedals::scanSustain() {
  if (nowH == statusChangedAt[PEDAL_I_SUSTAIN]) return;
  if (status[PEDAL_I_SUSTAIN] == newStatus[PEDAL_I_SUSTAIN]) return;

      status[PEDAL_I_SUSTAIN]  = newStatus[PEDAL_I_SUSTAIN];
  statusChangedAt[PEDAL_I_SUSTAIN] = nowH;
  if (status[PEDAL_I_SUSTAIN]) {
    sustainedChannel[0] = 64;
    midiExt.sendControlChange(midi::Sustain, 64, settings.outputChannel);
  } else {
    sustainedChannel[0] =  0;
    midiExt.sendControlChange(midi::Sustain,  0, settings.outputChannel);
  }
}

void Pedals::scanConfig() {
  if (nowH == statusChangedAt[PEDAL_I_CONFIG]) return;
  if (status[PEDAL_I_CONFIG] == newStatus[PEDAL_I_CONFIG]) return;

      status[PEDAL_I_CONFIG]  = newStatus[PEDAL_I_CONFIG];
  statusChangedAt[PEDAL_I_CONFIG] = nowH;
  if (status[PEDAL_I_CONFIG]) {
    firstMenu->onOpen();
  } else {
    firstMenu->onClose();
  }
}

void Pedals::scanExtA() {
  if (nowH == statusChangedAt[PEDAL_I_EXTA]) return;
  if (status[PEDAL_I_EXTA] == newStatus[PEDAL_I_EXTA]) return;

      status[PEDAL_I_EXTA]  = newStatus[PEDAL_I_EXTA];
  statusChangedAt[PEDAL_I_EXTA] = nowH;
  if (status[PEDAL_I_EXTA]) {
    midiExt.sendControlChange(midi::EffectControl1, 64, settings.outputChannel);
  } else {
    midiExt.sendControlChange(midi::EffectControl1,  0, settings.outputChannel);
  }
}

void Pedals::scanExtB() {
  if (nowH == statusChangedAt[PEDAL_I_EXTB]) return;
  if (status[PEDAL_I_EXTB] == newStatus[PEDAL_I_EXTB]) return;

      status[PEDAL_I_EXTB]  = newStatus[PEDAL_I_EXTB];
  statusChangedAt[PEDAL_I_EXTB] = nowH;
  if (status[PEDAL_I_EXTB]) {
    midiExt.sendControlChange(midi::EffectControl2, 64, settings.outputChannel);
  } else {
    midiExt.sendControlChange(midi::EffectControl2,  0, settings.outputChannel);
  }
}

void Pedals::scan() {
  uint8_t tmp;
  pedalScanPos += 1;
  switch (pedalScanPos) {
    case 1:
      tmp = ~PINB & (PEDAL_M_SUSTAIN | PEDAL_M_CONFIG | PEDAL_M_EXTA | PEDAL_M_EXTB);
      newStatus[PEDAL_I_EXTA   ] = tmp & PEDAL_M_EXTA;
      newStatus[PEDAL_I_EXTB   ] = tmp & PEDAL_M_EXTB;
      newStatus[PEDAL_I_SUSTAIN] = tmp & PEDAL_M_SUSTAIN;
      newStatus[PEDAL_I_CONFIG ] = tmp & PEDAL_M_CONFIG;
      break;

    case 2: scanSustain(); break;
    case 3: scanConfig();  break;
    case 4: scanExtA();    break;
    case 5: scanAnalogA(); break;
    case 6: scanExtB();    break;
    case 7: scanAnalogB(); break;

    default: pedalScanPos = 0; break;
  }
}

Pedals pedals;

#undef DEBUG_MODE
