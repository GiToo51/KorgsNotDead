
#include "ui.h"
#include "settings.h"

const uint8_t chromaticScale[12] = {6<<4, 2<<4, 2<<4, 2<<4, 2<<4, 2<<4, 2<<4, 4<<4, 2<<4, 2<<4, 2<<4, 2<<4};

Settings settings;

void Settings::resetScale(byte i) {
  scales.currentNotes[i] = 0; // A
  for (byte j=0; j< 12; j++)
    scales.colors[i][j] = chromaticScale[j]; // A
}
void Settings::setup() {
  memset((Settings_struct*)this, 0, sizeof(Settings_struct));
  mainMenuButton = 5;
  lcd.menuBrightness = 15;
  lcd.liveBrightness = 8;
  lcd.mode = LCD_MODE_VELO;
//  lcd.mode = LCD_MODE_DEBUG;
  led.menuBrightness = 15;
  led.liveBrightness = 15;
//  led.mode = LCD_MODE_OFF;
  led.mode = LED_MODE_MIDI_ONLY;
  led.background = LED_OFF;
  transposeReference = 42; // todo: note A in the middle
  outputChannel = 1;
  for (byte i=1; i< 17; i++)
    chanColors[i] = B10000000 | (i-1); // COLOR

  scales.currentIdx = -1;
  for (byte i=0; i< 16; i++)
    resetScale(i);

  clock.tickDivisor = 2; // = 1<<unitDivisor : black = 1<<2 = /4
  clock.ticksPerMeasure = 4; // 4
  clock.ticksPerMinutes = 70;

  clock.pos = 0;
  clock.loopFrom = 480;
  clock.loopTo = 2688;
  //clock.sender = true;
}

#undef DEBUG_MODE
