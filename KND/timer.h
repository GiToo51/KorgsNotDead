#pragma once

#include <Arduino.h>

// micros() is too slow (32bit)
// we use custom clock counter: timer 1
// 16bits: clock/1024 = 64µs per unit
// overflow at = 65535 * TIME_MICROS_FACTOR = ~4s
#define TIME_MICROS_FACTOR 64

// Speed measurments
#define SPEED_AVERAGE_ID 5
#define SPEED_KEY_ID 6
#define SPEED_FPS_ID 7


class Timer {
public:
  union {
    struct {
      uint8_t nowL; // change every     64µs, overflow at    16384µs
      uint8_t nowH; // change every 16.384ms, overflow at 4194.304ms
    } raw;
    uint16_t now16; // change every     64µs, overflow at 4194.304ms
  } now;

  void setup();
  void updateNow();

  static const char* cycleStrings[8];
  uint8_t  cycleDuration[8];
  uint8_t  avgNowH;
  uint16_t avgCount;
  uint8_t  fpsNowH;
  uint16_t fpsCount;
};

extern Timer timer;
