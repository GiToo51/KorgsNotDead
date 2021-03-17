
#include <Arduino.h>

// micros() is too slow (32bit)
// we use custom clock counter: timer 1
// 16bits: clock/1024 = 64µs per unit
// overflow at = 65535 * TIME_MICROS_FACTOR = ~4s
#define TIME_MICROS_FACTOR 64
extern uint8_t nowL; // change every     64µs, overflow at    16384µs
extern uint8_t nowH; // change every 16.384ms, overflow at 4194.304ms
extern uint16_t now; // change every     64µs, overflow at 4194.304ms
#define UPDATE_NOW nowL = TCNT1L; nowH = TCNT1H; now = nowL + (nowH << 8);

#define VELOCITY_FACTOR 1 // * TIME_MICROS_FACTOR = µs
#define VELOCITY_MINIMUM_UNITS 56 // * VELOCITY_FACTOR * TIME_MICROS_FACTOR = µs
#define VELOVITY_CURVE 32 // 1|1  1|2  1|4

// Main Loop
extern uint16_t timePerClock; // time between 2 send Clock


// lcd
/*
 * ASCII: 00-1F
 *        00: (A)
 *     01-08: _ 8x levels
 *     09-0F:
 *   
 * ASCII: 80-9F
 *     10-17: 8x notes: 10:/1 11:/2 12:/4 13:/8 14:/16 15:/32 16:/64 17:/128
 */
//extern const char* lcdBarLevel18Top;
//extern const char* lcdBarLevel18Bottom;


// global defs
#define KEYS_NB 88
#define FIRST_KEY 21
#define PRE_KEYS_NB 2
#define POST_KEYS_NB 2

typedef enum {
  KEY_IS_UP         = 0,
  KEY_IS_GOING_DOWN = 1,  // We increment the timer in this state
  KEY_IS_DOWN       = 2,
  KEY_IS_GOING_UP   = 3,    // We increment the timer in this state
} state_t;
typedef struct {
  uint8_t  state;
  uint8_t  note;
  uint16_t since;
  uint16_t until;
} key_t;
extern key_t keys[KEYS_NB];
extern uint8_t keyNbPressed;



// Looper & TickSubmenu
#define CLOCKS_PER_TICK 24
#define TICK_MEASURE_COUNT 32
#define TICK_MEASURE_FACTOR (CLOCKS_PER_TICK*(1<<settings.clock.tickDivisor))
#define TICK_MEASURES_MAX 0xFFFF
#define TICK_MEASURE_ROUND(pos) (pos / TICK_MEASURE_FACTOR * TICK_MEASURE_FACTOR)
extern uint16_t lastClockSend;

using computeByte_f = uint8_t (*)(uint8_t index);


// LCD & LEDS

#define LED_OFF 0
#define LED_WHITE(bright)                 (bright<<4)
#define LED_MENU_SEPARATOR(bright)        (bright<<4 | 0x01)
#define LED_MENU_PAGE(     bright)        (bright<<4 | 0x02)
#define LED_COLOR(color,   bright) (0x80 | bright<<4 | color)
#define LED_MEASURE LED_OFF
#define LED_BOOT LED_COLOR(15, 13)
//LED_WHITE(7 - ((settings.clock.pos / 3) % 8))



// Speed measurments
#define SPEED_AVERAGE_ID 5
#define SPEED_KEY_ID 6
#define SPEED_FPS_ID 7
extern const char* speedMainStrings[8];
extern uint8_t  speedMainDuration[8];
extern uint8_t  speedAvgNowH;
extern uint16_t speedAvgCount;
extern uint8_t  speedFpsNowH;
extern uint16_t speedFpsCount;


#define _KEY_PRESSED   B00000001 // normal, pressed or not
#define _KEY_SUSTAINED B00000010 // if sustain pedal is pressed, we keep ligh on as long as pedal is pressed
#define _KEY_FLASHED   B00000100 // on first led computation, flash the fresh pressed
//#define _KEY_PLAYED    B00001000 // TODO: learning mode
extern uint8_t sustainedChannel[17];
extern uint8_t keysStatus[17][KEYS_NB]; // 0: output, 1-16: input

#undef DEBUG_MODE
