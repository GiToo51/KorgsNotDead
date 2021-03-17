
#include "defs.h"
#include "ui.h"
#include "menu.h"
#include "pedals.h"
#include "settings.h"

UI ui;

UI::UI() {
  uiSerialEnabled = false;
}

void UI::reset() {
  uiWriteStep = 0;
  uiWritePos = 0;
  uiTotalFrames = 0;
}

void UI::enableSerial() {
  if (uiSerialEnabled) return;
  pinMode(14, OUTPUT);
  pinMode(15, INPUT);
  Serial2.begin(115200);
  uiSerialEnabled = true;
}

// When this serial port is disabled and input mode, ui arduino nano can be flashed for code upgrade
void UI::disableSerial() {
  Serial2.end();
  pinMode(14, INPUT);
  pinMode(15, INPUT);
  uiSerialEnabled = false;
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
    uiWritePos = 0;
    uiWriteStep = 1;
  } else {
    // average speed
    uint8_t diff = nowH - speedAvgNowH;
    if (diff >= 32) {
      speedMainDuration[SPEED_AVERAGE_ID] = (32 / (speedAvgCount/256/4) - 4);
      speedAvgCount = 0;
      speedAvgNowH = nowH;

    } else if (speedFpsCount >= 10) {
      uint16_t diff = (((nowH - speedFpsNowH) * 256) / 200) * TIME_MICROS_FACTOR;
      speedMainDuration[SPEED_FPS_ID] = ((uint16_t)10 * 5000) / diff;
      //Serial.println(speedMainDuration[SPEED_FPS_ID]);
      speedFpsCount = 0;
      speedFpsNowH = nowH;
    }
  }
}

void UI::writeUILcd() {
  Serial2.write(lcdComputeByte[lcdMode](uiWritePos));
  uiWritePos += 1;
  if (uiWritePos >= 32) {
    uiWritePos = 0;
    uiWriteStep = 2;
    Serial2.write(settings.led.background);
    Serial2.write(settings.led.background);
    Serial2.write(settings.led.background);
    Serial2.write(settings.led.background);
  }
}

void UI::writeUILed() {
  Serial2.write(ledComputeAlt[ledMode](uiWritePos));
  Serial2.write(ledComputeKey[ledMode](uiWritePos));
  uiWritePos += 1;
  if (uiWritePos >= KEYS_NB) {
    uiWritePos = 0;
    uiWriteStep = 0;
  
    Serial2.write(settings.led.background);
    Serial2.write(settings.led.background);
    Serial2.write(settings.led.background);
    Serial2.write(settings.led.background);

    speedFpsCount += 1;
    uiTotalFrames += 1;
    if (lcdMode == LCD_MODE_MENU && uiTotalFrames % 32 == 0)
      firstMenu->nextPage();
  }
}

void UI::sendNext() {
  switch (uiWriteStep) {
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
