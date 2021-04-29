#pragma once

#define PEDALS_ON_OFF_COUNT 4
#define PEDALS_ANALOG_COUNT 2

enum {
  // ANALOG & ON_OFF
  PEDAL_I_EXTA    = 0,
  PEDAL_I_EXTB    = 1,
  // not ANALOG
  PEDAL_I_SUSTAIN = 2,
  PEDAL_I_CONFIG  = 3,
};

enum {
  PEDAL_M_EXTA    = B00010000, // PORTB Pin10
  PEDAL_M_EXTB    = B00100000, // PORTB Pin11
  PEDAL_M_SUSTAIN = B01000000, // PORTB Pin12
  PEDAL_M_CONFIG  = B10000000, // PORTB Pin13
};

class Pedals {
public:
  void setup();
  void scan();

  uint8_t status[PEDALS_ON_OFF_COUNT];

private:
  void scanSustain();
  void scanConfig();
  void scanExtA();
  void scanAnalogA();
  void scanExtB();
  void scanAnalogB();

  uint8_t newStatus[PEDALS_ON_OFF_COUNT];
  uint8_t statusChangedAt[PEDALS_ON_OFF_COUNT];

  uint16_t analogValue[PEDALS_ANALOG_COUNT];
//  uint8_t analogChangedAt[PEDALS_ANALOG_COUNT];

  uint8_t pedalScanPos;
};

extern Pedals pedals;
