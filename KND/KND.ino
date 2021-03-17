
#include "defs.h"
#include "settings.h"
#include "pedals.h"
#include "midi.h"
#include "menu.h"
#include "ui.h"

#include "keyboard.hpp"

//#define DEBUG_MODE

// Main Loop
uint8_t nowL;
uint8_t nowH;
uint16_t now;
uint16_t timePerClock; // time between 2 send Clock
uint8_t functionIndex;

void setup() {
#ifdef DEBUG_MODE
  Serial.begin(115200);
  Serial.print("Settings size: ");
  Serial.println(sizeof(settings));
#endif

  midiExt.begin();
  keyboardSetup();

  // timer1 settings, for custom clock counter
  TCCR1A = B00000000;
  TCCR1B = B00000101;
  TIMSK1 = B00000000;
  TCNT1  = B00000000;  
  OCR1A  = B00000000;

  memset(sustainedChannel, 0, sizeof(sustainedChannel));
  memset(keysStatus, 0, sizeof(keysStatus));
  memset(keys, 0, sizeof(keys));

  keyNbPressed = 0;
  sendQueueIPos = 0;
  sendQueueOPos = 0;

  lastClockSend = 0;

  ui.reset();

  speedAvgNowH = nowH;
  speedAvgCount = 0;
  speedFpsNowH = nowH;
  speedFpsCount = 0;
  memset(speedMainDuration, 0, sizeof(speedMainDuration));

  keyNbPressed = 0;
  sustainedChannel[0] = 0;
  pedals.begin();

  settings.reset();

  memset(lcdDebugVelocity, 0, sizeof(lcdDebugVelocity));
  lcdVelocityPos = 0;
  lcdMode = settings.lcd.mode;
  ledMode = settings.led.mode;
  firstMenu = Menu::mainMenu();

  functionIndex = 0;

  UPDATE_NOW;
}

void loop() {
  uint8_t speedIndex;
  if (keyboardScan()) {
    speedIndex = SPEED_KEY_ID;
  } else {
    switch (functionIndex) {
      case 0:
        sendNextControl();
        break;
      case 1:
        midiExt.scan();
        break;
      case 2:
        ui.sendNext();
        break;
      case 3:
        pedals.scan();
        break;
    }
    speedIndex = functionIndex;
    functionIndex += 1;
  }

  //delayMicroseconds(2*TIME_MICROS_FACTOR);

  // max(s) cycle duration detection
  uint8_t oldNowL = nowL;
  UPDATE_NOW;
  uint8_t diff = nowL - oldNowL;
  if (speedMainDuration[functionIndex] < diff)
      speedMainDuration[functionIndex] = diff;

  // average speed detection
  speedAvgCount += 1;
}
