#pragma once

#include <Arduino.h>

using computeByte_f = uint8_t (*)(uint8_t index);

#define LCD_DEBUG_VELOCITY_SIZE 14

// LED
enum {
  LED_MODE_OFF   = 0,
  LED_MODE_BOOT  = 1,
  LED_MODE_MENU  = 2,
  LED_MODE_SOLO  = 3,
  LED_MODE_SCALE = 4, // Scale
  LED_MODE_SCORE = 5, // Partition
  LED_MODE_MIDI_ONLY  = 6, // Midi in only (default)
  LED_MODE_MIDI_SOLO  = 7, // Solo + Midi in
  LED_MODE_GAME  = 8, // White if match midi, Red otherwize
  LED_MODE_COUNT = 9,
};

// LCD
enum {
  LCD_MODE_OFF   = 0,
  LCD_MODE_BOOT  = 1,
  LCD_MODE_MENU  = 2,
  LCD_MODE_VELO  = 3,
  LCD_MODE_DEBUG = 4,
  LCD_MODE_COUNT = 5,
};

#define LED_OFF 0
#define LED_WHITE(bright)                 (bright<<4)
#define LED_MENU_SEPARATOR(bright)        (bright<<4 | 0x01)
#define LED_MENU_PAGE(     bright)        (bright<<4 | 0x02)
#define LED_COLOR(color,   bright) (0x80 | bright<<4 | color)
#define LED_MEASURE LED_OFF
#define LED_BOOT LED_COLOR(15, 13)
//LED_WHITE(7 - ((settings.clock.pos / 3) % 8))

class UI {

public:
  // When this serial port is disabled and input mode, ui arduino nano can be flashed for code upgrade
  void enableSerial();
  void disableSerial();

  void setup();
  void sendNext();

  uint8_t lcdMode;
  uint8_t ledMode;

  uint8_t lcdDebugVelocity[LCD_DEBUG_VELOCITY_SIZE];
  uint8_t lcdVelocityPos;

private:
  static const computeByte_f ledComputeKey[LED_MODE_COUNT];
  static const computeByte_f ledComputeAlt[LED_MODE_COUNT];

  uint8_t lcdBoot();
  uint8_t lcdMenu();
  uint8_t lcdVelocity();
  uint8_t lcdDebug();
  uint8_t lcdDefault();

  uint8_t ledOff();
  uint8_t ledBoot();
  uint8_t ledMenuKey();
  uint8_t ledSolo();
  uint8_t ledScale();
  uint8_t ledScore();
  uint8_t ledMIDIOnly();
  uint8_t ledMIDISolo();
  uint8_t ledGame();
  uint8_t ledBackground();
  uint8_t ledMenuAlt();
  uint8_t ledChannel(uint8_t chan);

  void writeUIBegin();
  void writeUILcd();
  void writeUILed();

  uint8_t writePos;
  uint8_t writeStep;
  uint8_t totalFrames;
  bool serialEnabled;
};

extern UI ui;

#undef DEBUG_MODE
