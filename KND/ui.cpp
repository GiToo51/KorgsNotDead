
#include "ui.h"
#include "timer.h"
#include "kbd.h"
#include "menu.h"
#include "pedals.h"
#include "settings.h"

//#define DEBUG_MODE

UI ui;

void UI::setup() {
  serialEnabled = false;
  writeStep = 0;
  writePos = 0;
  totalFrames = 0;
  memset(lcdDebugVelocity, 0, sizeof(lcdDebugVelocity));
  lcdVelocityPos = 0;
  lcdMode = settings.lcd.mode;
  ledMode = settings.led.mode;
}

void UI::enableSerial() {
  if (serialEnabled) return;
  pinMode(14, OUTPUT);
  pinMode(15, INPUT);
  Serial2.begin(115200);
  serialEnabled = true;
}

// When this serial port is disabled and input mode, ui arduino nano can be flashed for code upgrade
void UI::disableSerial() {
  Serial2.end();
  pinMode(14, INPUT);
  pinMode(15, INPUT);
  serialEnabled = false;
}

void UI::writeUIBegin() {
  if (Serial2.read() >= 0) {
    while (Serial2.read() >= 0) ; // flush
    if (pedals.status[PEDAL_I_CONFIG]) {
      Serial2.write(settings.lcd.menuBrightness);
      Serial2.write(settings.led.menuBrightness);
    } else {
      Serial2.write(settings.lcd.liveBrightness);
      Serial2.write(settings.led.liveBrightness);
    }
    writePos = 0;
    writeStep = 1;
  } else {
    // average speed
    uint8_t diff = timer.now.raw.nowH - timer.avgNowH;
    if (diff >= 32) {
      timer.cycleDuration[SPEED_AVERAGE_ID] = (32 / (timer.avgCount/256/4) - 4);
      timer.avgCount = 0;
      timer.avgNowH = timer.now.raw.nowH;

    } else if (timer.fpsCount >= 10) {
      uint16_t diff = (((timer.now.raw.nowH - timer.fpsNowH) * 256) / 200) * TIME_MICROS_FACTOR;
      timer.cycleDuration[SPEED_FPS_ID] = ((uint16_t)10 * 5000) / diff;
      //Serial.println(speedMainDuration[SPEED_FPS_ID]);
      timer.fpsCount = 0;
      timer.fpsNowH = timer.now.raw.nowH;
    }
  }
}

void UI::writeUILcd() {
  switch (lcdMode) {
    case LCD_MODE_BOOT:  Serial2.write(lcdBoot());     break;
    case LCD_MODE_MENU:  Serial2.write(lcdMenu());     break;
    case LCD_MODE_VELO:  Serial2.write(lcdVelocity()); break;
    case LCD_MODE_DEBUG: Serial2.write(lcdDebug());    break;
    default:             Serial2.write(lcdDefault());  break;
  }
  writePos += 1;
  if (writePos >= 32) {
    writePos = 0;
    writeStep = 2;
    Serial2.write(settings.led.background);
    Serial2.write(settings.led.background);
    Serial2.write(settings.led.background);
    Serial2.write(settings.led.background);
    Serial2.write(settings.led.background);
  }
}

void UI::writeUILed() {
  switch (ledMode) {
    case LED_MODE_OFF:        Serial2.write(ledOff());        Serial2.write(ledBackground()); break;
    case LED_MODE_BOOT:       Serial2.write(ledBoot());       Serial2.write(ledBoot());       break;
    case LED_MODE_MENU:       Serial2.write(ledMenuKey());    Serial2.write(ledMenuAlt());    break;
    case LED_MODE_SOLO:       Serial2.write(ledSolo());       Serial2.write(ledBackground()); break;
    case LED_MODE_SCALE:      Serial2.write(ledScale());      Serial2.write(ledBackground()); break;
    case LED_MODE_SCORE:      Serial2.write(ledScore());      Serial2.write(ledBackground()); break;
    case LED_MODE_MIDI_ONLY:  Serial2.write(ledMIDIOnly());   Serial2.write(ledBackground()); break;
    case LED_MODE_MIDI_SOLO:  Serial2.write(ledMIDISolo());   Serial2.write(ledBackground()); break;
    case LED_MODE_GAME:       Serial2.write(ledGame());       Serial2.write(ledBackground()); break;
    default:                  Serial2.write(ledBackground()); Serial2.write(ledBackground()); break;
  }

  writePos += 1;
  if (writePos >= KEYS_NB) {
    writePos = 0;
    writeStep = 0;
  
    Serial2.write(settings.led.background);
    Serial2.write(settings.led.background);
//    Serial2.write(settings.led.background);
    Serial2.write(settings.led.background);

    timer.fpsCount += 1;
    totalFrames += 1;
    if (lcdMode == LCD_MODE_MENU && totalFrames % 32 == 0)
      Menu::firstMenu->nextPage();
  }
}

void UI::sendNext() {
  switch (writeStep) {
    case 0:
      writeUIBegin();
      break;
    case 1:
      writeUILcd();
      break;
    case 2:
      writeUILed();
      break;
  }
}

#undef DEBUG_MODE
