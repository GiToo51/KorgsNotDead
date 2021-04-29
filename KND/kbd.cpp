
#include "timer.h"
#include "menu.h"
#include "kbd.h"

//#define DEBUG_MODE

void Keyboard::setup() {
  // OUTPUTS: 1pin per half Octave (6 keys): 15pins
  // Korg CN10:  1- 8
  // Mega 2560: 22-29
  DDRA = B11111111;
  // Korg CN10:  9-15
  // Mega 2560: 37-31 // reversed !!
  DDRC = B11111111;
  // Initial value
  PORTA = ~1;
  PORTC = ~0;

  // INPUTS + pull-up: 12pins: 2x6
  // TOP & BOTTOM
  // Korg CN9 :  1-12
  // Mega 2560: 53-42 // reversed !!
  DDRL = B00000000;
  DDRB  = B00000000;
  // Pull Up
  PORTL = B11111111;
  PORTB = B11111111;

  memset(sustainedChannel, 0, sizeof(sustainedChannel));
  memset(keysStatus, 0, sizeof(keysStatus));
  memset(keys, 0, sizeof(keys));
  keyNbPressed = 0;
  sendQueueIPos = 0;
  sendQueueOPos = 0;
  sustainedChannel[0] = 0;
}

void Keyboard::scanKey(uint8_t key, uint8_t pos) {
  switch (keys[key].state) { // on previous state
    case KEY_IS_UP:
      if (top & pos) {
        keys[key].state = KEY_IS_GOING_DOWN;
        keys[key].since = timer.now.now16;
      }
      break;
    case KEY_IS_GOING_DOWN:
      if (!(top & pos)) {
        keys[key].state = KEY_IS_UP; // ignored unfinished press
      } else if (bottom & pos) {
        keys[key].state = KEY_IS_DOWN;
        keys[key].until = timer.now.now16;
        sendQueueIPos++;
        sendQueue[sendQueueIPos] = key;
      }
      break;
    case KEY_IS_DOWN:
      if (!(bottom & pos)) {
        keys[key].state = KEY_IS_GOING_UP;
        keys[key].since = timer.now.now16;
      }
      break;
    case KEY_IS_GOING_UP:
      if (bottom & pos) {
        keys[key].state = KEY_IS_DOWN; // ignored unfinished release
      } else if (!(top & pos)) {
        keys[key].state = KEY_IS_UP;
        keys[key].until = timer.now.now16;
        sendQueueIPos++;
        sendQueue[sendQueueIPos] = key;
      }
      break;
  }
}

// Reading start at 2, but first key start at 0
void Keyboard::scanFirstBloc() {
  if (oldTops[0] == top && oldBottoms[0] == bottom) {
    // because whith this optimisation, this is too fast, we should debounce for bloc next selection
    delayMicroseconds(5);
    return;
  }
  keyChangedDuringScan = true;
      oldTops[0]  = top;   oldBottoms[0]  = bottom;

  scanKey(0, 1<<2);
  scanKey(1, 1<<3);
  scanKey(2, 1<<4);
  scanKey(3, 1<<5);
}
// Then everything is -2
void Keyboard::scanBloc(uint8_t block) {
  if (oldTops[block] == top && oldBottoms[block] == bottom) {
    // because whith this optimisation, this is too fast, we should debounce for next bloc selection
    delayMicroseconds(5);
    return;
  }
  keyChangedDuringScan = true;
      oldTops[block]  = top;   oldBottoms[block]  = bottom;

  block = 6 * block;
  scanKey(block-2, 1<<0);
  scanKey(block-1, 1<<1);
  scanKey(block  , 1<<2);
  scanKey(block+1, 1<<3);
  scanKey(block+2, 1<<4);
  scanKey(block+3, 1<<5);
}

#define UPDATE_INPUTS bottom = ~PINL; top = (~PINB & B00001111) | ((bottom & B11000000) >> 2)

bool Keyboard::scan() {
  keyChangedDuringScan = false;
  // Sorry if this difficult to read, but this is so efficient !!!! <100µs/scan cycle !!!
  // We first Read INPUTS (first OUTPUT was written during setup, and also written after the 15th read)
  // / / / / / ; Then we Write the NEXT OUTPUT
  // / / / / / / / / / / / / / / / / / ; Then we scan  for INPUTS VALUES
  // this order is to avoid waiting for debounce between writing output and reading input
  UPDATE_INPUTS;        PORTA = ~(1<<1); scanFirstBloc();
  UPDATE_INPUTS;        PORTA = ~(1<<2); scanBloc( 1);
  UPDATE_INPUTS;        PORTA = ~(1<<3); scanBloc( 2);
  UPDATE_INPUTS;        PORTA = ~(1<<4); scanBloc( 3);
  UPDATE_INPUTS;        PORTA = ~(1<<5); scanBloc( 4);
  UPDATE_INPUTS;        PORTA = ~(1<<6); scanBloc( 5);
  UPDATE_INPUTS;  PORTA = (byte)~(1<<7); scanBloc( 6);
  UPDATE_INPUTS; PORTC = ~1; PORTA = ~0; scanBloc( 7);
  UPDATE_INPUTS; PORTC = ~(1<<1);        scanBloc( 8);
  UPDATE_INPUTS; PORTC = ~(1<<2);        scanBloc( 9);
  UPDATE_INPUTS; PORTC = ~(1<<3);        scanBloc(10);
  UPDATE_INPUTS; PORTC = ~(1<<4);        scanBloc(11);
  UPDATE_INPUTS; PORTC = ~(1<<5);        scanBloc(12);
  UPDATE_INPUTS; PORTC = ~(1<<6);        scanBloc(13);
  UPDATE_INPUTS; PORTC = ~0; PORTA = ~1; scanBloc(14);
  return keyChangedDuringScan;
}

uint8_t Keyboard::velocity(uint8_t key) {
  uint8_t v = 126;
  uint16_t d = (keys[key].until - keys[key].since) / VELOCITY_FACTOR;

  if (d < VELOCITY_MINIMUM_UNITS) return 127;
  d = d - VELOCITY_MINIMUM_UNITS;

  while (d > VELOCITY_CURVE) {
    if (v <= VELOCITY_CURVE) return 1;
    d = (d - VELOCITY_CURVE) / 2;
    v =  v - VELOCITY_CURVE;
  }

  if (v > d) return v - d;
  return 1;
}

void Keyboard::sendNext() {
  if (sendQueueOPos != sendQueueIPos) {
    sendQueueOPos++;
    uint8_t i = sendQueue[sendQueueOPos];
    if (keys[i].state == KEY_IS_DOWN) {
#ifdef DEBUG_MODE
      Serial.print(i);
      Serial.print(" down ");
      Serial.println(velocity(i));
#endif
      Menu::firstMenu->onPress(i, velocity(i));
      keyNbPressed += 1;
    } else {
#ifdef DEBUG_MODE
      Serial.print(i);
      Serial.print("  up  ");
      Serial.println(velocity(i));
#endif
      Menu::firstMenu->onRelease(i, velocity(i));
      keyNbPressed -= 1;
    }
  }
}

Keyboard keyboard;

#undef DEBUG_MODE
