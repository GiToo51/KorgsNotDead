#pragma once

#include "ui.h" // COLORS

#define TICK_MEASURE_COUNT 32
#define TICK_MEASURE_FACTOR (CLOCKS_PER_TICK*(1<<settings.clock.tickDivisor))
#define TICK_MEASURES_MAX 0xFFFF
#define TICK_MEASURE_ROUND(pos) (pos / TICK_MEASURE_FACTOR * TICK_MEASURE_FACTOR)

// BASE MENU & BUTTON
class Menu;
class Button {
public:
  virtual uint8_t    color(uint8_t) { return LED_OFF; }
  virtual Menu*    subMenu() { return NULL; }
  virtual void     onPress(uint8_t, uint8_t) { }
  virtual void   onRelease(uint8_t, uint8_t) { }
  virtual uint8_t menuChar(uint8_t, uint8_t);
};
extern Button nullButton;

class Menu {
protected:
  int8_t _active;
public:
  static void resetMenu();
  static Menu* firstMenu;
  static uint8_t page;

  Menu() { _active = -1; }
  virtual  int8_t    active() { return _active; }
  virtual uint8_t      size() { return 0; }
  virtual Button**  buttons() { return NULL; }
  virtual Menu*    nextMenu() { return active() >= 0 && buttons() ? buttons()[active()]->subMenu() : NULL; }
  virtual void       onOpen() { if (nextMenu()) nextMenu()->onOpen( ); }
  virtual void      onClose() { if (nextMenu()) nextMenu()->onClose();  _active = -1; }
  virtual void     nextPage() { page++; if (page >= size() + (nextMenu() ? 1 : 0)) page = 0; if (buttons() && buttons()[page] == &nullButton) nextPage(); }
  virtual uint8_t menuCharL1(uint8_t index) { return menuCharL2(index); }
  virtual uint8_t menuCharL2(uint8_t index)               { if (page == size()) return nextMenu()->menuCharL2(index); if (buttons()) return buttons()[page]->menuChar(page, index); return (" # MenuL2 TBD # ")[index]; }
  virtual void       onPress(uint8_t index, uint8_t velo) { if (buttons()) buttons()[index]->onPress(  index, velo); _active = index; page = index; }
  virtual void     onRelease(uint8_t index, uint8_t velo) { if (buttons()) buttons()[index]->onRelease(index, velo); _active = -1;    page = 0; }
  virtual uint8_t      color(uint8_t index)               { if (buttons()) return buttons()[index]->color(index); else return LED_OFF; }
  virtual uint8_t   altColor(uint8_t index);
};


class WColoredMenu : public Menu {
public:
  uint8_t size() { return 17; }
  uint8_t color(uint8_t index) { return index == 0 ? LED_WHITE(2) : LED_COLOR(index, 2); }
  uint8_t altColor(uint8_t index) {
    if (index < size()) {
      if (active() == index)
        return color(index);
    }
    if (index > 0) {
      if (active() == index-1)
        return color(index-1);
    }
    return LED_OFF;
  }
};
