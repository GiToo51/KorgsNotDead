
#include "defs.h"
#include "ui.h"
#include "settings.h"

const uint8_t chromaticScale[12] = {6<<4, 2<<4, 2<<4, 2<<4, 2<<4, 2<<4, 2<<4, 4<<4, 2<<4, 2<<4, 2<<4, 2<<4};

Settings settings;

void Settings::resetScale(byte i) {
  settings.scales.currentNotes[i] = 0; // A
  for (byte j=0; j< 12; j++)
    settings.scales.colors[i][j] = chromaticScale[j]; // A
}
// time units per beat in bpm
void Settings::updateTimePerClock() {
  timePerClock = (1000000/TIME_MICROS_FACTOR) / (settings.clock.ticksPerMinutes*CLOCKS_PER_TICK/60);
}
void Settings::reset() {
  memset(&settings, 0, sizeof(settings));
  settings.mainMenuButton = 5;
  settings.lcd.menuBrightness = 15;
  settings.lcd.liveBrightness = 8;
//  settings.lcd.mode = LCD_MODE_VELO;
  settings.lcd.mode = LCD_MODE_DEBUG;
  settings.led.menuBrightness = 15;
  settings.led.liveBrightness = 15;
//  settings.led.mode = LCD_MODE_OFF;
  settings.led.mode = LED_MODE_MIDI_ONLY;
  settings.led.background = LED_OFF;
  settings.transposeReference = 42; // todo: note A in the middle
  settings.outputChannel = 1;
  for (byte i=1; i< 17; i++)
    settings.chanColors[i] = B10000000 | i-1; // COLOR

  settings.scales.currentIdx = -1;
  for (byte i=0; i< 16; i++)
    resetScale(i);

  settings.clock.tickDivisor = 2; // = 1<<unitDivisor : black = 1<<2 = /4
  settings.clock.ticksPerMeasure = 4; // 4
  settings.clock.ticksPerMinutes = 70;
  updateTimePerClock();
  settings.clock.pos = 0;
  settings.clock.loopFrom = 480;
  settings.clock.loopTo = 2688;
  //settings.clock.sender = true;
}

#undef DEBUG_MODE
