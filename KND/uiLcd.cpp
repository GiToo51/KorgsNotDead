
#include "defs.h"
#include "ui.h"
#include "menu.h"


const char* lcdBarLevel18Top    = "\xA0\xA0\xA0\xA0\xA0\xA0\xA0\xA0\xA0\x01\x02\x03\x04\x05\x06\x07\x08\xFF\xFF\xFF";
const char* lcdBarLevel18Bottom = "\xA0\x01\x02\x03\x04\x05\x06\x07\x08\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";

const char* lcdDefaultString = "\x10  \x11  \x12  \x13  \x14  \x15K\x10rg 's N\x10t De\0d";
const char* lcdBootString = "! DON'T  PANIC !    booting...  ";

uint8_t lcdMode;
uint8_t lcdVelocityPos;
uint8_t lcdDebugVelocity[LCD_DEBUG_VELOCITY_SIZE];

uint8_t lcdDebug(uint8_t b) {
  uint8_t idx = b%16/2;
  switch (b) {
    case  0: case 16:
    case  2: case 18:
    case  4: case 20:
    case  6: case 22:
    case  8: case 24:
    case 10: case 26:
    case 12: case 28:
    case 14: case 30:
      return speedMainStrings[b%16/2][b/16];

    case  1: case  3: case  5: case  7: case  9: case 11: case 13:
      return lcdBarLevel18Top[   speedMainDuration[idx]];
    case 17: case 19: case 21: case 23: case 25: case 27: case 29:
      return lcdBarLevel18Bottom[speedMainDuration[idx]];

    case 15:
      return '0' + speedMainDuration[SPEED_FPS_ID]/10;
    case 31:
      return '0' + speedMainDuration[SPEED_FPS_ID]%10;
    default:
      return ' ';
  }
}

uint8_t lcdDefault(uint8_t b) {
  return lcdDefaultString[b];
}

uint8_t lcdBoot(uint8_t b) {
  return lcdBootString[b];
}

uint8_t lcdMenu(uint8_t b) {
/*  Menu* menu = firstMenu;
  while (menu->nextMenu())
    menu = menu->nextMenu();*/
  if (b < 16)
    return firstMenu->menuCharL1(b);
  else
    return firstMenu->menuCharL2(b-16);
}

uint8_t lcdVelocity(uint8_t b) {
  switch (b) {
    case  0: case  1: case  2: case  3:
    case  4: case  5: case  6: case  7:
    case  8: case  9: case 10: case 11:
    case 12: case 13:
      return lcdBarLevel18Top[(lcdDebugVelocity[b]+7)/8];

    case 16: case 17: case 18: case 19:
    case 20: case 21: case 22: case 23:
    case 24: case 25: case 26: case 27:
    case 28: case 29:
      return lcdBarLevel18Bottom[(lcdDebugVelocity[b-16]+7)/8];

    // Avg
    case 14: return speedMainDuration[SPEED_AVERAGE_ID] > 9 ? '0' + (speedMainDuration[SPEED_AVERAGE_ID]/10 %10) : 0xF2;
    case 15: return '0' + (speedMainDuration[SPEED_AVERAGE_ID]%10);
    // Fps
    case 30: return '0' + (speedMainDuration[SPEED_FPS_ID]/10 %10);
    case 31: return '0' + (speedMainDuration[SPEED_FPS_ID]    %10);
    default: return ' ';
  }
}
/*
const char* noteStrings[12] = {
  "A "  ,
  "A#Bb",
  "B "  ,
  "C "  ,
  "C#Db",
  "D "  ,
  "D#Eb",
  "E "  ,
  "F "  ,
  "F#Gb",
  "G "  ,
  "G#Ab",
};

const char* scaleStrings[15] = {
  "Ab Fm  4b",
  "A  F#m 3#",
  "Bb Gm  2b",
  "B  G#m 5#",
  "Cb Abm 7b",
  "C  Am  0 ",
  "C# A#m 7#",
  "Db Bbm 5b",
  "D  Bm  2#",
  "Eb Cm  3b",
  "E  C#m 4#",
  "F  Dm  1b",
  "F# D#m 6#",
  "Gb Ebm 6b",
  "G  Em  1#",
};*/
/*
    Do Majeur : La mineur --> aucune altération
    Fa Majeur : Ré mineur --> 1 b
    Si bémol Majeur : Sol mineur --> 2 b
    Mi bémol Majeur : Do mineur --> 3 b
    La bémol Majeur : Fa mineur --> 4 b
    Ré bémol Majeur : Si bémol mineur --> 5 b
    Do dièse Majeur : La dièse mineur --> 7 #
    Sol bémol Majeur : Mi bémol mineur --> 6 b
    Fa dièse Majeur : Ré dièse mineur --> 6 #
    Do bémol Majeur : La bémol mineur --> 7 b
    Si Majeur : Sol dièse mineur --> 5 #
    Mi Majeur : Do dièse mineur --> 4 #
    La Majeur : Fa dièse mineur --> 3 #
    Ré Majeur : Si mineur --> 2 #
    Sol Majeur : Mi mineur --> 1 #
*/
const computeByte_f lcdComputeByte[LCD_MODE_COUNT] = {
  lcdDefault,
  lcdBoot,
  lcdMenu,
  lcdVelocity,
  lcdDebug,
};

#undef DEBUG_MODE
