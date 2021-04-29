
#include "timer.h"
#include "midi.h"
#include "kbd.h"
#include "ui.h"
#include "pedals.h"
#include "settings.h"
#include "menu.h"
#include "trackpad.h"

//#define DEBUG_MODE

uint8_t functionIndex;

void setup() {
#ifdef DEBUG_MODE
  Serial.begin(115200);
  Serial.print("setup. Settings size:");
  Serial.println(sizeof(Settings_struct));
#endif

  timer.setup();
  settings.setup();
  Menu::resetMenu();
  midiExt.setup();
  keyboard.setup();
  ui.setup();
  pedals.setup();
  pedals.scan();
#ifdef DEBUG_MODE
  if (trackpad.setup(3, 2))
    Serial.println("Successfully found trackapd");
  else
    Serial.println("Did not find trackapd");
#else
  trackpad.setup(3, 2);
#endif

  timer.updateNow();
  functionIndex = 0;
#ifdef DEBUG_MODE
      Serial.println("setup done.");
#endif
}

void loop() {
  uint8_t speedIndex;
  if (keyboard.scan()) {
    speedIndex = SPEED_KEY_ID;
  } else {
    speedIndex = functionIndex%5;
    switch (speedIndex) {
      case 0:
        keyboard.sendNext();
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
      case 4:
        trackpad.scan();
        break;
    }
    functionIndex += 1;
  }

//  delayMicroseconds(2*TIME_MICROS_FACTOR);
  // max(s) cycle duration detection
  uint8_t oldNowL = timer.now.raw.nowL;
  timer.updateNow();
  uint8_t diff = timer.now.raw.nowL - oldNowL;
  if (timer.cycleDuration[speedIndex] < diff)
      timer.cycleDuration[speedIndex] = diff;
  // average speed detection
  timer.avgCount += 1;
}
