
#include "ui.h"
#include "kbd.h"
#include "menu.h"
#include "settings.h"

uint8_t ledMode;

uint8_t UI::ledChannel(uint8_t chan) {
  if (keyboard.keysStatus[chan][writePos] & _KEY_FLASHED) {
    keyboard.keysStatus[chan][writePos] &= ~_KEY_FLASHED;
    return settings.chanColors[chan] | 7<<4; // flash on one frame only
  }
  if (keyboard.keysStatus[chan][writePos] & _KEY_PRESSED) {
    return settings.chanColors[chan] | 5<<4;
  }
  if (keyboard.keysStatus[chan][writePos] & _KEY_SUSTAINED) {
    if (keyboard.sustainedChannel[chan] == 0)
      keyboard.keysStatus[chan][writePos] &= ~_KEY_SUSTAINED;
    else
      return settings.chanColors[chan] | 1<<4;
  }
  return LED_OFF;
}


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

uint8_t UI::ledBoot() {
  return LED_BOOT;
}

uint8_t UI::ledScale() {
  return scaleDefinition[writePos%12];
}

uint8_t UI::ledMIDIOnly() {
  uint8_t led;
  if ((led = ledChannel( 1)) != LED_OFF) return led;
  if ((led = ledChannel( 2)) != LED_OFF) return led;
  if ((led = ledChannel( 3)) != LED_OFF) return led;
  if ((led = ledChannel( 4)) != LED_OFF) return led;
  if ((led = ledChannel( 5)) != LED_OFF) return led;
  if ((led = ledChannel( 6)) != LED_OFF) return led;
  if ((led = ledChannel( 7)) != LED_OFF) return led;
  if ((led = ledChannel( 8)) != LED_OFF) return led;
  if ((led = ledChannel( 9)) != LED_OFF) return led;
  if ((led = ledChannel(10)) != LED_OFF) return led;
  if ((led = ledChannel(11)) != LED_OFF) return led;
  if ((led = ledChannel(12)) != LED_OFF) return led;
  if ((led = ledChannel(13)) != LED_OFF) return led;
  if ((led = ledChannel(14)) != LED_OFF) return led;
  if ((led = ledChannel(15)) != LED_OFF) return led;
  if ((led = ledChannel(16)) != LED_OFF) return led;
  return LED_OFF;
}
uint8_t UI::ledMIDISolo() {
  uint8_t led;
  if ((led = ledChannel( 0)) != LED_OFF) return led;
  if ((led = ledChannel( 1)) != LED_OFF) return led;
  if ((led = ledChannel( 2)) != LED_OFF) return led;
  if ((led = ledChannel( 3)) != LED_OFF) return led;
  if ((led = ledChannel( 4)) != LED_OFF) return led;
  if ((led = ledChannel( 5)) != LED_OFF) return led;
  if ((led = ledChannel( 6)) != LED_OFF) return led;
  if ((led = ledChannel( 7)) != LED_OFF) return led;
  if ((led = ledChannel( 8)) != LED_OFF) return led;
  if ((led = ledChannel( 9)) != LED_OFF) return led;
  if ((led = ledChannel(10)) != LED_OFF) return led;
  if ((led = ledChannel(11)) != LED_OFF) return led;
  if ((led = ledChannel(12)) != LED_OFF) return led;
  if ((led = ledChannel(13)) != LED_OFF) return led;
  if ((led = ledChannel(14)) != LED_OFF) return led;
  if ((led = ledChannel(15)) != LED_OFF) return led;
  if ((led = ledChannel(16)) != LED_OFF) return led;
  return LED_OFF;
}
uint8_t UI::ledGame() {
  uint8_t led;
  if ((led = ledChannel( 0)) != LED_OFF) return led;
  if ((led = ledChannel( 1)) != LED_OFF) return led;
  if ((led = ledChannel( 2)) != LED_OFF) return led;
  if ((led = ledChannel( 3)) != LED_OFF) return led;
  if ((led = ledChannel( 4)) != LED_OFF) return led;
  if ((led = ledChannel( 5)) != LED_OFF) return led;
  if ((led = ledChannel( 6)) != LED_OFF) return led;
  if ((led = ledChannel( 7)) != LED_OFF) return led;
  if ((led = ledChannel( 8)) != LED_OFF) return led;
  if ((led = ledChannel( 9)) != LED_OFF) return led;
  if ((led = ledChannel(10)) != LED_OFF) return led;
  if ((led = ledChannel(11)) != LED_OFF) return led;
  if ((led = ledChannel(12)) != LED_OFF) return led;
  if ((led = ledChannel(13)) != LED_OFF) return led;
  if ((led = ledChannel(14)) != LED_OFF) return led;
  if ((led = ledChannel(15)) != LED_OFF) return led;
  if ((led = ledChannel(16)) != LED_OFF) return led;
  return LED_OFF;
}
uint8_t UI::ledSolo() {
  return ledChannel(0);
}
uint8_t UI::ledOff() {
  return LED_OFF;
}
uint8_t UI::ledBackground() {
  return settings.led.background;
}
uint8_t UI::ledScore() {
  return LED_OFF;
}

uint8_t UI::ledMenuKey() {
  Menu* menu = Menu::firstMenu;
  uint8_t key = writePos;
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

uint8_t UI::ledMenuAlt() {
  Menu* menu = Menu::firstMenu;
  uint8_t key = writePos;
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

#undef DEBUG_MODE
