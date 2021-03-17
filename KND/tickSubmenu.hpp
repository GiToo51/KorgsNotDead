
const char* errorMessage = " # # ERROR  # #  # # ERROR  # # ";

#define TTTB_DIV_IDX 4
#define TTTB_MEAS_IDX 5

class TickTapTimeButton : public Button {
public:
  uint8_t menuChar(uint8_t menuIndex, uint8_t index) {
    switch (menuIndex) {
      case TTTB_DIV_IDX:  return ">> Tap for time "[index];
      case TTTB_MEAS_IDX: return ">> Tap for measu"[index];
      default: return errorMessage[index];
    }
  }
  uint8_t  color(uint8_t menuIndex) {
    switch (menuIndex) {
      case TTTB_DIV_IDX:  return (settings.clock.pos / 3) % (CLOCKS_PER_TICK    /2) == 0 ? LED_COLOR(5, 6) : LED_OFF;
      case TTTB_MEAS_IDX: return (settings.clock.pos / 3) % (TICK_MEASURE_FACTOR/2) == 0 ? LED_COLOR(8, 6) : LED_OFF;
      default: LED_OFF;
    }    
  }
private:
  uint16_t tappedAt[4];
  uint8_t tappedAtPos;
};

class TickSendModeButton : public Button {
public:
  uint8_t menuChar(uint8_t menuIndex, uint8_t index) { return (settings.clock.sender ? ">> Mode: Send   " : ">> Mode: Receive")[index]; }
  uint8_t  color(uint8_t menuIndex) {
    if (settings.clock.sender)
      return LED_COLOR(15, 6);
    else
      return LED_COLOR(4, 6);
  }
  void onRelease(uint8_t menuIndex, uint8_t velo) {
    Button::onRelease(menuIndex, velo);
    midiExt.sendStop();
    midiExt.sendSystemReset();
    settings.clock.sender = !settings.clock.sender;
    settings.clock.pos = settings.clock.loopFrom;
    midiExt.sendSongPosition(settings.clock.pos/6);
    if (settings.clock.sender)
      midiExt.sendContinue();
  }
};
class TickSetLooperButton : public Button {
public:
  uint8_t menuChar(uint8_t menuIndex, uint8_t index) { return (">> Press to loop")[index]; }
  uint8_t  color(uint8_t menuIndex) {
    if (settings.clock.loopFrom != settings.clock.loopTo)
      return LED_COLOR(2, 6);
    return LED_COLOR(2, 2);
  }
  void onPress(uint8_t menuIndex, uint8_t velo) {
    Button::onPress(menuIndex, velo);
    settings.clock.loopFrom = TICK_MEASURE_ROUND(settings.clock.pos);
    settings.clock.loopTo = TICK_MEASURES_MAX;
  }
  void onRelease(uint8_t menuIndex, uint8_t velo) {
    Button::onRelease(menuIndex, velo);
    settings.clock.loopTo = TICK_MEASURE_ROUND(settings.clock.pos) + TICK_MEASURE_FACTOR;
#ifdef DEBUG_MODE
  Serial.print("Looper Set From ");
  Serial.print(settings.clock.loopFrom);
  Serial.print(" to ");
  Serial.println(settings.clock.loopTo);
#endif
  }
};
class TickResetLooperButton : public Button {
public:
  uint8_t menuChar(uint8_t menuIndex, uint8_t index) { return (">> Disable loop ")[index]; }
  uint8_t  color(uint8_t menuIndex) {
    if (settings.clock.loopTo > 0)
      return LED_COLOR(15, 3);
    return LED_OFF;
  }
  void onRelease(uint8_t menuIndex, uint8_t velo) {
    Button::onRelease(menuIndex, velo);
    settings.clock.loopFrom = 0;
    settings.clock.loopTo = 0;
  }
};


class TickMeasureSubmenu : public Menu {
public:
  uint8_t menuCharL2(uint8_t index) { return (">> Time view")[index]; }
  uint8_t      size() { return TICK_MEASURE_COUNT + 4; }
  void   onPress(uint8_t menuIndex, uint8_t velo) {
    switch (menuIndex) {
      case 0:
        settings.clock.viewOffsetKeys -= settings.clock.viewOffsetKeys > TICK_MEASURE_COUNT ? TICK_MEASURE_COUNT/2 : settings.clock.viewOffsetKeys;
        break;
      case 1:
        settings.clock.viewOffsetKeys -= settings.clock.viewOffsetKeys > 0 ? 1 : 0;
        break;
      case TICK_MEASURE_COUNT+2:
        settings.clock.viewOffsetKeys += 1;
        break;
      case TICK_MEASURE_COUNT+3:
        settings.clock.viewOffsetKeys += TICK_MEASURE_COUNT/2;
        break;
    default:
      menuIndex += settings.clock.viewOffsetKeys - 2;

      if (keyNbPressed <= 1) {
        uint32_t diff = settings.clock.loopTo - settings.clock.loopFrom;
        if (diff == 0)
          diff = TICK_MEASURE_FACTOR;
        settings.clock.loopFrom = menuIndex * TICK_MEASURE_FACTOR;
        settings.clock.loopTo = settings.clock.loopFrom + diff;
      } else if(menuIndex < settings.clock.loopFrom / TICK_MEASURE_FACTOR) {
        settings.clock.loopFrom = menuIndex * TICK_MEASURE_FACTOR;
      } else {
        settings.clock.loopTo = menuIndex * TICK_MEASURE_FACTOR;
      }
    }
  }
  uint8_t color(uint8_t menuIndex) {
    switch (menuIndex) {
      case 0:
        if (settings.clock.pos / 3 % 8 == 0 && settings.clock.viewOffsetKeys > settings.clock.pos/TICK_MEASURE_FACTOR+1)
          return LED_WHITE(7);
        return settings.clock.viewOffsetKeys >= TICK_MEASURE_COUNT/2 ? LED_COLOR(4, 4) : LED_COLOR(4, 1);
      case 1:
        if (settings.clock.pos / 3 % 8 == 0 && settings.clock.viewOffsetKeys == settings.clock.pos/TICK_MEASURE_FACTOR+1)
          return LED_WHITE(7);
        return settings.clock.viewOffsetKeys > 0                  ? LED_COLOR(4, 2) : LED_COLOR(4, 1);
      case TICK_MEASURE_COUNT+2:
        if (settings.clock.pos / 3 % 8 == 0 && settings.clock.viewOffsetKeys + TICK_MEASURE_COUNT == settings.clock.pos/TICK_MEASURE_FACTOR)
          return LED_WHITE(7);
        return LED_COLOR(4, 2);
      case TICK_MEASURE_COUNT+3:
        if (settings.clock.pos / 3 % 8 == 0 && settings.clock.viewOffsetKeys + TICK_MEASURE_COUNT < settings.clock.pos/TICK_MEASURE_FACTOR)
          return LED_WHITE(7);
        return LED_COLOR(4, 4);
    }
    menuIndex += settings.clock.viewOffsetKeys - 2;

    if (menuIndex == (settings.clock.pos / TICK_MEASURE_FACTOR)) {
      if (settings.clock.pos / 3 % 8 == 0)
        return LED_WHITE(7);
      if (settings.clock.sender)
        return LED_COLOR(15, 6);
      else
        return LED_COLOR(4, 6);
    }
    if (menuIndex%(TICK_MEASURE_COUNT/2) == 0)
      return LED_WHITE(1);
//    if (settings.clock.loopTo / TICK_MEASURE_FACTOR > menuIndex && settings.clock.loopFrom / TICK_MEASURE_FACTOR <= menuIndex)
//      return LED_COLOR(2, 1);
    return LED_OFF; //LED_WHITE(1);
  }

  uint8_t  altColor(uint8_t menuIndex) {
    switch (menuIndex) {
      case 0:
      case 1:
      case TICK_MEASURE_COUNT+3:
      case TICK_MEASURE_COUNT+4:
        return keyNbPressed > 0 ? LED_COLOR(4, 1) : LED_OFF;
    }

    menuIndex += settings.clock.viewOffsetKeys - 2;

    if (settings.clock.loopTo / TICK_MEASURE_FACTOR >= menuIndex && settings.clock.loopFrom / TICK_MEASURE_FACTOR <= menuIndex)
      return LED_COLOR(2, 1);
    return LED_OFF;
  }
};



class TickSubmenu : public Menu {
public:
  TickSubmenu() {
    _b[0] = &sendModeButton;
    _b[1] = &setLooperButton;
    _b[2] = &resetLooperButton;
    _b[3] = &tapTimeButton;
    _b[4] = &tapTimeButton;
  };
  uint8_t      size() { return 5; }
  Button**  buttons() { return _b; }
  Menu*    nextMenu() { return &measureSubmenu; }
  void onRelease(uint8_t menuIndex, uint8_t velo) { Menu::onRelease(menuIndex, velo); _active = -1; }
  uint8_t menuCharL1(uint8_t index) {
    switch (index) {
      case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
        return ("> Timing")[index];
      case  8:
        return settings.clock.ticksPerMeasure >= 10 ? '0' + settings.clock.ticksPerMeasure / 10 : ' ';
      case  9:
        return '0' + settings.clock.ticksPerMeasure % 10;
      case 10:
        return ' ';
      case 11:
        return 0x10 + settings.clock.tickDivisor;
      case 12:
        return ' ';
      case 13:
        return settings.clock.ticksPerMinutes >= 100 ? '0' + settings.clock.ticksPerMinutes / 100 : ' ';
      case 14:
        return '0' + settings.clock.ticksPerMinutes / 10 % 10;
      case 15:
        return '0' + settings.clock.ticksPerMinutes % 10;
      default:
        return ' ';
    }
  }

private:
  Button* _b[5];
  TickSendModeButton    sendModeButton;
  TickSetLooperButton   setLooperButton;
  TickResetLooperButton resetLooperButton;
  TickTapTimeButton     tapTimeButton;
  TickMeasureSubmenu    measureSubmenu;
};

#undef DEBUG_MODE
