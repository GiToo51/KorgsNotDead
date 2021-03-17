
#include "defs.h"

uint16_t speedMin;
uint8_t speedMinItem;

uint8_t  speedMainDuration[8];
uint8_t  speedAvgNowH;
uint16_t speedAvgCount;
uint8_t  speedFpsNowH;
uint16_t speedFpsCount;

const char* speedMainStrings[8] = {
  "Ctr ",
  "Mid ",
  "UI  ",
  "Ped ",
  "    ",
  "Avg ",
  "Key ",
  "Fps ",
};
