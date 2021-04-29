
#include <Arduino.h>

#include "LiquidCrystal.h"

LiquidCrystal lcd(12, 10, 9, 8, 7, 6, 5, 4, 3, 2);

#define LCD_BRIGHTNESS_PIN 11

#include "custom_charset.hpp"
#include "colors.hpp"

#define _R 1
#define _G 0
#define _B 2


// COMMON
#define KEYS_NB 88
#define LEDS_NB ((KEYS_NB*2)+4+2)
#define LCD_SIZE (16*2)
// !COMMON

float   ledBrightness;
uint8_t lcdBrightness;
uint8_t lcdChars[LCD_SIZE];
uint8_t frame[LEDS_NB*3];
uint8_t framesCount;

uint8_t incommingByte;
uint8_t ledReadPos;
uint8_t customCharR;
uint8_t customCharW;
uint8_t customChars[8];

void  ws2815_send_frame(uint8_t *data, uint16_t datlen);

void setup() {
  Serial.begin(115200);
  pinMode(LCD_BRIGHTNESS_PIN, OUTPUT); //Set pin 10 to OUTPUT

  analogWrite(LCD_BRIGHTNESS_PIN, 3);
  customChars[0] = 0;
  customChars[1] = 0x10;
  customChars[2] = 0x11;
  customChars[3] = 0x12;
  customChars[4] = 0x13;
  customChars[5] = 0x14;
  customChars[6] = 0x15;
  customChars[7] = 0x16;
  for (uint8_t i = 0; i < 8; i++)
    lcd.createChar(i, (uint8_t*)customCharset[customChars[i]]);
  customCharR = 0;
  customCharW = 0;
  
  lcd.setCursor(0,0);
  lcd.write("\x01  \x02  \x03  \x04  \x05  \x06");
  lcd.setCursor(0,1);
  lcd.write("K\x01rg 's N\x01t De\0d", 16);

  memset(frame, 0, LEDS_NB*3);
  framesCount = 0;
  ws2815_send_frame(frame, LEDS_NB*3);
}

void setLedColor(uint16_t ledIdx, uint8_t bright, uint8_t color) {
  uint16_t frameIdx = ledIdx*3;
  uint16_t i = (bright*16*3)+(color*3);
  frame[frameIdx+_G] = (float)colors[i+_G] * ledBrightness;
  frame[frameIdx+_R] = (float)colors[i+_R] * ledBrightness;
  frame[frameIdx+_B] = (float)colors[i+_B] * ledBrightness;
}
void setLedSpecial(uint16_t ledIdx, uint8_t bright, uint8_t special) {
  uint16_t frameIdx = ledIdx*3;
  uint16_t i;
  switch (special) {
    case 0: // Normal white
      i = (float)bright * 32 * ledBrightness;
      frame[frameIdx+_G] = i;
      frame[frameIdx+_R] = i;
      frame[frameIdx+_B] = i;
      break;
    case 1: // Menu separator (smooth beat)
      if (framesCount/32%2)
        i = (float)bright * (framesCount%32) * ledBrightness;
      else
        i = (float)bright * (31- (framesCount%32)) * ledBrightness;
        
      frame[frameIdx+_G] = i;
      frame[frameIdx+_R] = i;
      frame[frameIdx+_B] = i;
      break;
    case 2: // Menu page (color farandole)
      i = (bright*16*3)+(((ledReadPos+framesCount)%16)*3);
      frame[frameIdx+_G] = (float)colors[i+_G] * ledBrightness;
      frame[frameIdx+_R] = (float)colors[i+_R] * ledBrightness;
      frame[frameIdx+_B] = (float)colors[i+_B] * ledBrightness;
      break;
    case 15: // Do not update (experimental)
      break;
    default: // unsupported
      frame[frameIdx+_G] = 0;
      frame[frameIdx+_R] = 0;
      frame[frameIdx+_B] = 0;
      break;
  }
}

uint32_t lastStart;

uint8_t waitNextByte() {
  int in;
  while ((in = Serial.read()) < 0) {
//    writeNextCustomChar(); // when nothing to read, we have time to write one custom char to the lcd during buffer fills
    if (millis() - lastStart > 1000)
      __asm__ __volatile__ ("jmp 0"); // restart if nothing received since 1s
  }
  return in;
}

void writeNextCustomChar() {
//  if (customCharW != customCharR) {
    lcd.createChar(customCharW, (uint8_t*)customCharset[customChars[customCharW]]);
    customCharW = (customCharW + 1) % 8;
//  }
}

uint8_t convertChar(uint8_t in) {
  if (in >= 32) return in; // not custom

  for (uint8_t i = 0; i < 8; i++)
    if (customChars[i] == in)
      return i;

  uint8_t r = customCharR;
  customChars[r] = in;
  customCharR = (customCharR + 1) % 8;
  return r;
}

void loop() {
  while (Serial.read() >= 0) ;
  Serial.write(0);
  lastStart = millis();
  framesCount++;

  lcdBrightness = waitNextByte();

  ledBrightness = brightness[waitNextByte()];

  // Read & Write LCD
  lcd.home();
  analogWrite(LCD_BRIGHTNESS_PIN, lcdBrightness*16);
  for (uint8_t lcdReadPos = 0; lcdReadPos < LCD_SIZE; lcdReadPos++) {
    if (lcdReadPos ==  0) lcd.setCursor(0,0);
    if (lcdReadPos == 16) lcd.setCursor(0,1);
    lcd.write(convertChar(waitNextByte()));
  }

  // Read LEDS
  for (ledReadPos = 0; ledReadPos < LEDS_NB; ledReadPos++) {
    incommingByte = waitNextByte();
    if (incommingByte & B10000000)
      setLedColor(ledReadPos, (incommingByte / 16) % 8, incommingByte % 16);
    else
      setLedSpecial(ledReadPos, (incommingByte / 16) % 8, incommingByte % 16);
    if (ledReadPos % 16 == 0)
      writeNextCustomChar(); // when nothing to read, we have time to write one custom char to the lcd during buffer fills
  }

  // Write LCD Custom chars
//  while (writeNextCustomChar()) ;

  // Write LEDS
  ws2815_send_frame(frame, LEDS_NB*3);
}
