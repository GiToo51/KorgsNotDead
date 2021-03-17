
//#define DEBUG_MODE

key_t keys[KEYS_NB];
uint8_t keyNbPressed;
uint8_t sustainedChannel[17];
uint8_t keysStatus[17][KEYS_NB]; // 0: output, 1-16: input

bool keyChangedDuringScan;

// send note events are not done during scan loop (because we use micros() slow function only once per cycle)
// So keys that produces events are queued there
uint8_t sendQueue[256];
uint8_t sendQueueIPos;
uint8_t sendQueueOPos;

// Ultimate optimisation: defuauls keys are always up
// So we keep for each 6 keys block if something

// forward declarations
void ledSettingsButtonPressed(uint8_t key, uint8_t velo);
void ledSettingsButtonReleased(uint8_t key, uint8_t velo);

void keyboardSetup() {
  // OUTPUTS: 1pin per half Octave (6 keys): 15pins \\
  // Korg CN10:  1- 8
  // Mega 2560: 22-29
  DDRA = B11111111;
  // Korg CN10:  9-15
  // Mega 2560: 37-31 // reversed !!
  DDRC = B11111111;
  // Initial value
  PORTA = ~1;
  PORTC = ~0;

  // INPUTS + pull-up: 12pins: 2x6 \\
  // TOP & BOTTOM
  // Korg CN9 :  1-12
  // Mega 2560: 53-42 // reversed !!
  DDRL = B00000000;
  DDRB  = B00000000;
  // Pull Up
  PORTL = B11111111;
  PORTB = B11111111;
}

uint8_t top;
uint8_t bottom;
uint8_t oldTops[15];
uint8_t oldBottoms[15];
void keyboardScanKey(uint8_t key, uint8_t pos) {
  switch (keys[key].state) { // on previous state
    case KEY_IS_UP:
      if (top & pos) {
        keys[key].state = KEY_IS_GOING_DOWN;
        keys[key].since = now;
      }
      break;
    case KEY_IS_GOING_DOWN:
      if (!(top & pos)) {
        keys[key].state = KEY_IS_UP; // ignored unfinished press
      } else if (bottom & pos) {
        keys[key].state = KEY_IS_DOWN;
        keys[key].until = now;
        sendQueueIPos++;
        sendQueue[sendQueueIPos] = key;
      }
      break;
    case KEY_IS_DOWN:
      if (!(bottom & pos)) {
        keys[key].state = KEY_IS_GOING_UP;
        keys[key].since = now;
      }
      break;
    case KEY_IS_GOING_UP:
      if (bottom & pos) {
        keys[key].state = KEY_IS_DOWN; // ignored unfinished release
      } else if (!(top & pos)) {
        keys[key].state = KEY_IS_UP;
        keys[key].until = now;
        sendQueueIPos++;
        sendQueue[sendQueueIPos] = key;
      }
      break;
  }
}

// Reading start at 2, but first key start at 0
void keyboardScanFirstBloc() {
  if (oldTops[0] == top && oldBottoms[0] == bottom) {
    // because whith this optimisation, this is too fast, we should debounce for bloc next selection
    delayMicroseconds(4);
    return;
  }
  keyChangedDuringScan = true;
      oldTops[0]  = top;   oldBottoms[0]  = bottom;

  keyboardScanKey(0, 1<<2);
  keyboardScanKey(1, 1<<3);
  keyboardScanKey(2, 1<<4);
  keyboardScanKey(3, 1<<5);
}
// Then everything is -2
void keyboardScanBloc(uint8_t block) {
  if (oldTops[block] == top && oldBottoms[block] == bottom) {
    // because whith this optimisation, this is too fast, we should debounce for next bloc selection
    delayMicroseconds(4);
    return;
  }
  keyChangedDuringScan = true;
      oldTops[block]  = top;   oldBottoms[block]  = bottom;

  block = 6 * block;
  keyboardScanKey(block-2, 1<<0);
  keyboardScanKey(block-1, 1<<1);
  keyboardScanKey(block  , 1<<2);
  keyboardScanKey(block+1, 1<<3);
  keyboardScanKey(block+2, 1<<4);
  keyboardScanKey(block+3, 1<<5);
}

#define UPDATE_INPUTS bottom = ~PINL; top = (~PINB & B00001111) | ((bottom & B11000000) >> 2)

bool keyboardScan() {
  keyChangedDuringScan = false;
  // Sorry if this difficult to read, but this is so efficient !!!! <100µs/scan cycle !!!
  // We first Read INPUTS (first OUTPUT was written during setup, and also written after the 15th read)
  // / / / / / ; Then we Write the NEXT OUTPUT
  // / / / / / / / / / / / / / / / / / ; Then we scan  for INPUTS VALUES
  // this order is to avoid waiting for debounce between writing output and reading input
  UPDATE_INPUTS;        PORTA = ~(1<<1); keyboardScanFirstBloc();
  UPDATE_INPUTS;        PORTA = ~(1<<2); keyboardScanBloc( 1);
  UPDATE_INPUTS;        PORTA = ~(1<<3); keyboardScanBloc( 2);
  UPDATE_INPUTS;        PORTA = ~(1<<4); keyboardScanBloc( 3);
  UPDATE_INPUTS;        PORTA = ~(1<<5); keyboardScanBloc( 4);
  UPDATE_INPUTS;        PORTA = ~(1<<6); keyboardScanBloc( 5);
  UPDATE_INPUTS;  PORTA = (byte)~(1<<7); keyboardScanBloc( 6);
  UPDATE_INPUTS; PORTC = ~1; PORTA = ~0; keyboardScanBloc( 7);
  UPDATE_INPUTS; PORTC = ~(1<<1);        keyboardScanBloc( 8);
  UPDATE_INPUTS; PORTC = ~(1<<2);        keyboardScanBloc( 9);
  UPDATE_INPUTS; PORTC = ~(1<<3);        keyboardScanBloc(10);
  UPDATE_INPUTS; PORTC = ~(1<<4);        keyboardScanBloc(11);
  UPDATE_INPUTS; PORTC = ~(1<<5);        keyboardScanBloc(12);
  UPDATE_INPUTS; PORTC = ~(1<<6);        keyboardScanBloc(13);
  UPDATE_INPUTS; PORTC = ~0; PORTA = ~1; keyboardScanBloc(14);
  return keyChangedDuringScan;
}

uint8_t velocity(uint8_t key) {
  uint8_t v = 126;
  uint16_t d = (keys[key].until - keys[key].since) / VELOCITY_FACTOR;

  if (d < VELOCITY_MINIMUM_UNITS) return 127;
  d = d - VELOCITY_MINIMUM_UNITS;

  while (d > VELOVITY_CURVE) {
    if (v <= VELOVITY_CURVE) return 1;
    d = (d - VELOVITY_CURVE) / 2;
    v =  v - VELOVITY_CURVE;
  }

  if (v > d) return v - d;
  return 1;
}

void sendNextControl() { // return true if sent
  if (sendQueueOPos != sendQueueIPos) {
    sendQueueOPos++;
    uint8_t i = sendQueue[sendQueueOPos];
    if (keys[i].state == KEY_IS_DOWN) {
      firstMenu->onPress(i, velocity(i));
      keyNbPressed += 1;
    } else {
      firstMenu->onRelease(i, velocity(i));
      keyNbPressed -= 1;
    }
  }
}

#undef DEBUG_MODE
