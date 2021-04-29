
#include "kbd.h"
#include "ui.h"
#include "settings.h"
#include "pedals.h"
#include "midi.h"
#include "menu.h"

#include "menuTick.hpp"

//#define DEBUG_MODE

Button nullButton;
uint8_t Menu::page;

uint8_t Button::menuChar(uint8_t, uint8_t index) {
  return subMenu() ? subMenu()->menuCharL1(index) : (" # Button TBD # ")[index];
}

class ResetMenu : public Menu {
  public:
    void onOpen() {
      ui.ledMode = LED_MODE_BOOT;
      ui.lcdMode = LCD_MODE_BOOT;
#ifdef DEBUG_MODE
      Serial.println("ResetMenu->onOpen()");
#endif
    }
    void onClose() {
#ifdef DEBUG_MODE
      Serial.println("ResetMenu->onClose()");
#endif
      setup();
      ui.disableSerial();
    }
  private:
    bool _triggered;
};
class ResetSettingButton : public Button {
  public:
    uint8_t menuChar(uint8_t, uint8_t index) {
      return ("> Reboot        ")[index];
    }
    uint8_t  color(uint8_t) {
      return LED_COLOR(15, 4);
    }
    void onRelease(uint8_t index, uint8_t velo) {
      Button::onRelease(index, velo);
      Menu::firstMenu->onClose();
      Menu::firstMenu = &resetMenu;
      Menu::firstMenu->onOpen();
#ifdef DEBUG_MODE
      Serial.println("Reset Triggered");
#endif
    }
  private:
    ResetMenu resetMenu;
};

class LoadSlotMenu : public WColoredMenu {
  public:
    uint8_t menuCharL1(uint8_t index) {
      return ("> Load Config   ")[index];
    }
    void onRelease(uint8_t index, uint8_t) {
#ifdef DEBUG_MODE
      Serial.print("Loading Settings to: ");
      Serial.println(index);
#endif
    }
};
class LoadSettingButton : public Button {
  public:
    uint8_t  color(uint8_t) {
      return LED_COLOR( 3, 4);
    }
    Menu* subMenu() {
      return &slotMenu;
    }
  private:
    LoadSlotMenu slotMenu;
};

class SaveSlotMenu : public WColoredMenu {
  public:
    uint8_t menuCharL1(uint8_t index) {
      return ("> Save Config   ")[index];
    }
    void onRelease(uint8_t index, uint8_t) {
#ifdef DEBUG_MODE
      Serial.print("Saving Settings to: ");
      Serial.println(index);
#endif
    }
};
class TickSubmenu;
class SaveSettingButton : public Button {
  public:
    uint8_t  color(uint8_t) {
      return LED_COLOR( 6, 4);
    }
    Menu* subMenu() {
      return &slotMenu;
    }
  private:
    SaveSlotMenu slotMenu;
};

class TickSubmenuButton : public Button {
  public:
    uint8_t  color(uint8_t) {
      return LED_COLOR( 10, 4);
    }
    void onRelease(uint8_t index, uint8_t velo); // defined later
    virtual Menu* subMenu() {
      return &tickSubmenu;
    }

  private:
    TickSubmenu tickSubmenu;
};


class MainMenu : public Menu {
  public:
    MainMenu() {
      _b[0] = &resetSettingButton;
      _b[1] = &loadSettingButton;
      _b[2] = &saveSettingButton;
      _b[3] = &nullButton;
      _b[4] = &nullButton;
      _b[5] = &tickSubmenuButton;
      _b[6] = &nullButton;
    };
    uint8_t     size() { return 7; }
    int8_t    active() { return _active >= 0 ? _active : settings.mainMenuButton; }
    Button** buttons() { return _b; }
    void onOpen() {
      ui.ledMode = LED_MODE_MENU;
      ui.lcdMode = LCD_MODE_MENU;
#ifdef DEBUG_MODE
      Serial.println("MainMenu->onOpen()");
#endif
    }
    void onClose() {
      ui.ledMode = settings.led.mode;
      ui.lcdMode = settings.lcd.mode;
#ifdef DEBUG_MODE
      Serial.println("MainMenu->onClose()");
#endif
    }
    void onPress(uint8_t i, uint8_t velo) {
      if (pedals.status[PEDAL_I_CONFIG]) {
        Menu* menu = this;
        while (menu && i > 0) {
          i--;
          if (i < menu->size()) {
            if (menu == this)
              return (void)Menu::onPress(i, velo);
            else
              return (void)menu->onPress(i, velo);
          }
          i -= (menu->size()+1);
          menu = menu->nextMenu();
        }
      } else {
        // set this only on key down, to release with the same code if transpose happen during key press
        keyboard.keys[i].note = i + settings.transposeValue + FIRST_KEY;
        keyboard.keysStatus[0][i] |= _KEY_PRESSED | (keyboard.sustainedChannel[0] ? _KEY_SUSTAINED : 0) | _KEY_FLASHED;
        midiExt.sendNoteOn(keyboard.keys[i].note, velo, settings.outputChannel);
        ui.lcdDebugVelocity[ui.lcdVelocityPos] = velo;
        ui.lcdVelocityPos = (ui.lcdVelocityPos + 1) % LCD_DEBUG_VELOCITY_SIZE;
        ui.lcdDebugVelocity[ui.lcdVelocityPos] = 0;
      }
    }
    void onRelease(uint8_t i, uint8_t velo) {
      if (pedals.status[PEDAL_I_CONFIG] && keyboard.keys[i].note == 0) {
        Menu* menu = this;
        while (menu && i > 0) {
          i--;
          if (i < menu->size()) {
            if (menu == this)
              return (void)Menu::onRelease(i, velo);
            else
              return (void)menu->onRelease(i, velo);
          }
          i -= (menu->size() + 1);
          menu = menu->nextMenu();
        }
      } else {
        midiExt.sendNoteOff(keyboard.keys[i].note, velo, settings.outputChannel);
        keyboard.keys[i].note = 0;
        ui.enableSerial(); // check if ui serial is started. TODO, maybe move this elsewhere ?
        keyboard.keysStatus[0][i] &= ~_KEY_PRESSED;
      }
    }
    void nextPage() {
      if      (_active >= 0) ; // ignore next if button active
      else if (nextMenu()) nextMenu()->nextPage();
      else Menu::nextPage();
    }
    uint8_t menuCharL1(uint8_t index) {
      if (_active >= 0 || ! nextMenu())
        return ("   Main  Menu   ")[index];
      return nextMenu()->menuCharL1(index);
    }
    uint8_t menuCharL2(uint8_t index) {
      if (_active >= 0 || ! nextMenu())
        return Menu::menuCharL2(index);
      return nextMenu()->menuCharL2(index);
    }

  private:
    Button* _b[7];
    ResetSettingButton resetSettingButton;
    LoadSettingButton   loadSettingButton;
    SaveSettingButton   saveSettingButton;

    TickSubmenuButton   tickSubmenuButton;
};

void TickSubmenuButton::onRelease(uint8_t index, uint8_t velo) {
  settings.mainMenuButton = (settings.mainMenuButton == index ? -1 : index);
  Button::onRelease(index, velo);
  Menu::page = (settings.mainMenuButton == -1 ? index : 0);
}

uint8_t Menu::altColor(uint8_t index) {
  if (index < size()) {
    if (active() == index)
      return color(index);
    if (page == index && active() < 0)
      return LED_MENU_PAGE(4);
  }
  if (index > 0) {
    if (active() == index-1)
      return color(index-1);
    if (page == index-1 && active() < 0)
      return LED_MENU_PAGE(4);
  }
  return LED_OFF;
}

MainMenu mainMenuInstance;
Menu* Menu::firstMenu = &mainMenuInstance;

void Menu::resetMenu() {
#ifdef DEBUG_MODE
      Serial.println("resetMenu");
#endif
  Menu::page = 0;
  Menu::firstMenu = &mainMenuInstance;
}

#undef DEBUG_MODE
