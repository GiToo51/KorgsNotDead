
#include "defs.h"
#include "ui.h"
#include "menu.h"
#include "settings.h"

uint8_t ledMode;

// LIVE
uint8_t ledChannel(uint8_t chan, uint8_t key) {
  if (keysStatus[chan][key] & _KEY_FLASHED) {
    keysStatus[chan][key] &= ~_KEY_FLASHED;
    return settings.chanColors[chan] | 7<<4; // flash on one frame only
  }
  if (keysStatus[chan][key] & _KEY_PRESSED) {
    return settings.chanColors[chan] | 5<<4;
  }
  if (keysStatus[chan][key] & _KEY_SUSTAINED) {
    if (sustainedChannel[chan] == 0)
      keysStatus[chan][key] &= ~_KEY_SUSTAINED;
    else
      return settings.chanColors[chan] | 1<<4;
  }
  return LED_OFF;
}


// C: Do
#define LED_SCALE(b) (LED_WHITE(b))
#define LED_SCALE_m(b) (LED_COLOR(15, b))
#define LED_SCALE_M(b) (LED_COLOR( 9, b))
const uint8_t scaleDefinition[12] = {
  LED_SCALE(  7), // 1st
  LED_SCALE_m(2), // 2nd m
  LED_SCALE_M(2), // 2nd M
  LED_SCALE_m(4), // 3rd m
  LED_SCALE_M(4), // 3rd M
  LED_SCALE(  1), // 4th j
  LED_OFF,
  LED_SCALE(  6), // 5th J
  LED_SCALE_m(2), // 6th m
  LED_SCALE_M(2), // 6th M
  LED_SCALE_m(2), // 7th m
  LED_SCALE_M(2), // 7th M
};

uint8_t ledBoot(uint8_t key) {
  return LED_BOOT;
}

uint8_t ledScale(uint8_t key) {
  return scaleDefinition[key%12];
}

uint8_t ledMIDIOnly(uint8_t key) {
  uint8_t led;
  if ((led = ledChannel( 1, key)) != LED_OFF) return led;
  if ((led = ledChannel( 2, key)) != LED_OFF) return led;
  if ((led = ledChannel( 3, key)) != LED_OFF) return led;
  if ((led = ledChannel( 4, key)) != LED_OFF) return led;
  if ((led = ledChannel( 5, key)) != LED_OFF) return led;
  if ((led = ledChannel( 6, key)) != LED_OFF) return led;
  if ((led = ledChannel( 7, key)) != LED_OFF) return led;
  if ((led = ledChannel( 8, key)) != LED_OFF) return led;
  if ((led = ledChannel( 9, key)) != LED_OFF) return led;
  if ((led = ledChannel(10, key)) != LED_OFF) return led;
  if ((led = ledChannel(11, key)) != LED_OFF) return led;
  if ((led = ledChannel(12, key)) != LED_OFF) return led;
  if ((led = ledChannel(13, key)) != LED_OFF) return led;
  if ((led = ledChannel(14, key)) != LED_OFF) return led;
  if ((led = ledChannel(15, key)) != LED_OFF) return led;
  if ((led = ledChannel(16, key)) != LED_OFF) return led;
  return LED_OFF;
}
uint8_t ledMIDISolo(uint8_t key) {
  uint8_t led;
  if ((led = ledChannel( 0, key)) != LED_OFF) return led;
  if ((led = ledChannel( 1, key)) != LED_OFF) return led;
  if ((led = ledChannel( 2, key)) != LED_OFF) return led;
  if ((led = ledChannel( 3, key)) != LED_OFF) return led;
  if ((led = ledChannel( 4, key)) != LED_OFF) return led;
  if ((led = ledChannel( 5, key)) != LED_OFF) return led;
  if ((led = ledChannel( 6, key)) != LED_OFF) return led;
  if ((led = ledChannel( 7, key)) != LED_OFF) return led;
  if ((led = ledChannel( 8, key)) != LED_OFF) return led;
  if ((led = ledChannel( 9, key)) != LED_OFF) return led;
  if ((led = ledChannel(10, key)) != LED_OFF) return led;
  if ((led = ledChannel(11, key)) != LED_OFF) return led;
  if ((led = ledChannel(12, key)) != LED_OFF) return led;
  if ((led = ledChannel(13, key)) != LED_OFF) return led;
  if ((led = ledChannel(14, key)) != LED_OFF) return led;
  if ((led = ledChannel(15, key)) != LED_OFF) return led;
  if ((led = ledChannel(16, key)) != LED_OFF) return led;
  return LED_OFF;
}
uint8_t ledGame(uint8_t key) {
  uint8_t led;
  if ((led = ledChannel( 0, key)) != LED_OFF) return led;
  if ((led = ledChannel( 1, key)) != LED_OFF) return led;
  if ((led = ledChannel( 2, key)) != LED_OFF) return led;
  if ((led = ledChannel( 3, key)) != LED_OFF) return led;
  if ((led = ledChannel( 4, key)) != LED_OFF) return led;
  if ((led = ledChannel( 5, key)) != LED_OFF) return led;
  if ((led = ledChannel( 6, key)) != LED_OFF) return led;
  if ((led = ledChannel( 7, key)) != LED_OFF) return led;
  if ((led = ledChannel( 8, key)) != LED_OFF) return led;
  if ((led = ledChannel( 9, key)) != LED_OFF) return led;
  if ((led = ledChannel(10, key)) != LED_OFF) return led;
  if ((led = ledChannel(11, key)) != LED_OFF) return led;
  if ((led = ledChannel(12, key)) != LED_OFF) return led;
  if ((led = ledChannel(13, key)) != LED_OFF) return led;
  if ((led = ledChannel(14, key)) != LED_OFF) return led;
  if ((led = ledChannel(15, key)) != LED_OFF) return led;
  if ((led = ledChannel(16, key)) != LED_OFF) return led;
  return LED_OFF;
}
uint8_t ledSolo(uint8_t key) {
  return ledChannel(0, key);
}
uint8_t ledOff(uint8_t key) {
  return LED_OFF;
}
uint8_t ledBackground(uint8_t key) {
  return settings.led.background;
}
uint8_t ledScore(uint8_t key) {
  return LED_OFF;
}

uint8_t ledMenuKey(uint8_t key) {
  Menu* menu = firstMenu;
  while (menu) {
    if (key == 0 || key == menu->size()+1)
      return LED_MENU_SEPARATOR(3);

    if (key-1 < menu->size())
      return menu->color(key-1);

    key -= menu->size() + 2;
    menu = menu->nextMenu();
  }
  return LED_OFF;
}

uint8_t ledMenuAlt(uint8_t key) {
  Menu* menu = firstMenu;
  while (menu) {
    if (key == 0)
      return LED_MENU_SEPARATOR(6);
    if (key-1 <= menu->size())
      return menu->altColor(key-1);
    key -= menu->size() + 2;
    menu = menu->nextMenu();
  }
  if (key == 0)
    return LED_MENU_SEPARATOR(6);
  return LED_OFF;
}


const computeByte_f ledComputeKey[LED_MODE_COUNT] = {
  ledOff,
  ledBoot,
  ledMenuKey,
  ledSolo,
  ledScale,
  ledScore, // Mode partition
  ledMIDIOnly,
  ledMIDISolo,
  ledGame,
};
const computeByte_f ledComputeAlt[LED_MODE_COUNT] = {
  ledBackground,
  ledBoot,
  ledMenuAlt,
  ledBackground,
  ledBackground,
  ledBackground,
  ledBackground,
  ledBackground,
  ledBackground,
};

#undef DEBUG_MODE
