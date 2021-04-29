
#include "midi.h"
#include "kbd.h"
#include "menu.h"
#include "settings.h"
#include "timer.h"
#include "pedals.h"

//#define DEBUG_MODE

#define PEDAL_ANALOG_PRECISION (7<<4)

void Pedals::setup() {
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);
  // port init done in keyboard
  memset(status, 0, sizeof(status));
  memset(newStatus, 0, sizeof(newStatus));
  memset(analogValue, 0, sizeof(analogValue));
//  memset(analogChangedAt, 0, sizeof(analogChangedAt));
  memset(statusChangedAt, 0, sizeof(statusChangedAt));
  pedalScanPos = 0;
  ADCSRA = B00000111;
  ADCSRB = 1<<ACME | 1<<MUX5;
  ADCSRA = B10000111;
}

void Pedals::scanSustain() {
  if (timer.now.raw.nowH == statusChangedAt[PEDAL_I_SUSTAIN]) return;
  if (status[PEDAL_I_SUSTAIN] == newStatus[PEDAL_I_SUSTAIN]) return;

      status[PEDAL_I_SUSTAIN]  = newStatus[PEDAL_I_SUSTAIN];
  statusChangedAt[PEDAL_I_SUSTAIN] = timer.now.raw.nowH;
  if (status[PEDAL_I_SUSTAIN]) {
    keyboard.sustainedChannel[0] = 64;
    midiExt.sendControlChange(midi::Sustain, 64, settings.outputChannel);
#ifdef DEBUG_MODE
  Serial.println("Sustain 64");
#endif
  } else {
    keyboard.sustainedChannel[0] =  0;
    midiExt.sendControlChange(midi::Sustain,  0, settings.outputChannel);
#ifdef DEBUG_MODE
  Serial.println("Sustain 0");
#endif
  }
}

void Pedals::scanConfig() {
  if (timer.now.raw.nowH == statusChangedAt[PEDAL_I_CONFIG]) return;
  if (status[PEDAL_I_CONFIG] == newStatus[PEDAL_I_CONFIG]) return;

      status[PEDAL_I_CONFIG]  = newStatus[PEDAL_I_CONFIG];
  statusChangedAt[PEDAL_I_CONFIG] = timer.now.raw.nowH;
  if (status[PEDAL_I_CONFIG]) {
    Menu::firstMenu->onOpen();
#ifdef DEBUG_MODE
  Serial.println("firtMenu onOpen");
#endif
  } else {
    Menu::firstMenu->onClose();
#ifdef DEBUG_MODE
  Serial.println("firtMenu onClose");
#endif
  }
}

void Pedals::scanExtA() {
  // first we start analog convertion
  // set the analog reference (high two bits of ADMUX) and select the
  // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
  // to 0 (the default).
  // EXTERNAL || INTERNAL2V56
  ADMUX = (EXTERNAL << 6) | (10 & 0x07);
  // start the conversion
  ADCSRA |= B01000000;

  if (timer.now.raw.nowH == statusChangedAt[PEDAL_I_EXTA]) return;
  if (status[PEDAL_I_EXTA] == newStatus[PEDAL_I_EXTA]) return;

      status[PEDAL_I_EXTA]  = newStatus[PEDAL_I_EXTA];
  statusChangedAt[PEDAL_I_EXTA] = timer.now.raw.nowH;
  if (status[PEDAL_I_EXTA]) {
    midiExt.sendControlChange(midi::EffectControl1, 64, settings.outputChannel);
#ifdef DEBUG_MODE
  Serial.println("Pedal A 64");
#endif
  } else {
    midiExt.sendControlChange(midi::EffectControl1,  0, settings.outputChannel);
#ifdef DEBUG_MODE
  Serial.println("Pedal A 0");
#endif
  }
}

void Pedals::scanAnalogA() {
/*  if ((ADCSRA & B01000000) != 0 || ADMUX != ((INTERNAL2V56 << 6) | (10 & 0x07))) {
    Serial.print('A');
    return;
  }*/

  uint16_t newAnalogValue = (ADCL | (ADCH << 8)) << 4;
  int16_t diff = newAnalogValue - analogValue[PEDAL_I_EXTA];
  if (diff < PEDAL_ANALOG_PRECISION && diff > -PEDAL_ANALOG_PRECISION) return;

  analogValue[PEDAL_I_EXTA] = newAnalogValue;
//  analogChangedAt[PEDAL_I_EXTA] = timer.now.raw.nowH;
  midiExt.sendControlChange(midi::EffectControl1   , (uint8_t)(newAnalogValue/128), settings.outputChannel);
  midiExt.sendControlChange(midi::EffectControl1+32, (uint8_t)(newAnalogValue%128), settings.outputChannel);
#ifdef DEBUG_MODE
  Serial.print("Analog A ");
  Serial.println(newAnalogValue);
#endif
}

void Pedals::scanExtB() {
  // first we start analog convertion
  // set the analog reference (high two bits of ADMUX) and select the
  // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
  // to 0 (the default).
  ADMUX = (EXTERNAL << 6) | (11 & 0x07);
  // start the conversion
  ADCSRA |= B01000000;

  if (timer.now.raw.nowH == statusChangedAt[PEDAL_I_EXTB]) return;
  if (status[PEDAL_I_EXTB] == newStatus[PEDAL_I_EXTB]) return;

      status[PEDAL_I_EXTB]  = newStatus[PEDAL_I_EXTB];
  statusChangedAt[PEDAL_I_EXTB] = timer.now.raw.nowH;
  if (status[PEDAL_I_EXTB]) {
    midiExt.sendControlChange(midi::EffectControl2, 64, settings.outputChannel);
#ifdef DEBUG_MODE
  Serial.println("Pedal B 64");
#endif
  } else {
    midiExt.sendControlChange(midi::EffectControl2,  0, settings.outputChannel);
#ifdef DEBUG_MODE
  Serial.println("Pedal B 0");
#endif
  }
}

void Pedals::scanAnalogB() {
/*  if ((ADCSRA & B01000000) != 0 || ADMUX != ((INTERNAL2V56 << 6) | (11 & 0x07))) {
    Serial.print('B');
    return;
  }*/

  uint16_t newAnalogValue = (ADCL | (ADCH << 8)) << 4;
  int16_t diff = newAnalogValue - analogValue[PEDAL_I_EXTB];
  if (diff < PEDAL_ANALOG_PRECISION && diff > -PEDAL_ANALOG_PRECISION) return;

  analogValue[PEDAL_I_EXTB] = newAnalogValue;
//  analogChangedAt[PEDAL_I_EXTB] = timer.now.raw.nowH;
  midiExt.sendControlChange(midi::EffectControl2   , (uint8_t)(newAnalogValue/128), settings.outputChannel);
  midiExt.sendControlChange(midi::EffectControl2+32, (uint8_t)(newAnalogValue%128), settings.outputChannel);
#ifdef DEBUG_MODE
  Serial.print("Analog B ");
  Serial.println(newAnalogValue);
#endif
}


void Pedals::scan() {
  uint8_t tmp;
  pedalScanPos += 1;
  switch (pedalScanPos) {
    case 1: scanSustain(); break;
    case 2: scanConfig();  break;

    case 3: scanExtA();    break;
    case 4: scanAnalogA(); break;

    case 5: scanExtB();    break;
    case 6: scanAnalogB(); break;

    default:
      tmp = ~PINB & (PEDAL_M_SUSTAIN | PEDAL_M_CONFIG | PEDAL_M_EXTA | PEDAL_M_EXTB);
      newStatus[PEDAL_I_EXTA   ] = tmp & PEDAL_M_EXTA;
      newStatus[PEDAL_I_EXTB   ] = tmp & PEDAL_M_EXTB;
      newStatus[PEDAL_I_SUSTAIN] = tmp & PEDAL_M_SUSTAIN;
      newStatus[PEDAL_I_CONFIG ] = tmp & PEDAL_M_CONFIG;
      pedalScanPos = 0;
      break;
  }
}

Pedals pedals;

#undef DEBUG_MODE
