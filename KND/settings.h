#pragma once

#include <Arduino.h>

typedef struct {
  uint8_t id; // 0-15
   int8_t mainMenuButton; // -1 for none

  struct {
    uint8_t menuBrightness; // 0-15 0: disabled
    uint8_t liveBrightness; // 0-15 0: disabled
    uint8_t mode; // LCD_MODE_*
  } lcd;

  struct {
    uint8_t menuBrightness; // 0-15 0: disabled
    uint8_t liveBrightness; // 0-15 0: disabled
    uint8_t mode; // LED_MODE_*
    uint8_t background;
  } led;

  uint8_t transposeReference;
   int8_t transposeValue;

  uint8_t outputChannel;
  uint8_t chanColors[17]; // 16 chans

  struct {
    uint8_t count;
     int8_t currentIdx; // -1 disabled
    uint8_t autoNote; // 0:NO | until note ()
    uint8_t autoNext; // auto_next_e
    uint8_t currentNotes[16]; // max 16 scales, note 0-11
    uint8_t colors[16][12];
  } scales;

  struct {
    bool sender;
    uint8_t tickDivisor; // = 1<<unitDivisor : black = 1<<2 = /4
    uint8_t ticksPerMeasure; // 4
    uint16_t ticksPerMinutes; // BPM
    uint32_t pos;
    uint32_t loopFrom;
    uint32_t loopTo;
    uint16_t viewOffsetKeys;
  } clock;

} Settings_struct;

class Settings : public Settings_struct {

public:
  void setup();

private:
  void resetScale(byte i);
};

extern Settings settings;
