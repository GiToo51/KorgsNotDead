/*
* light weight WS2812 lib V2.1 - Arduino support
*
* Controls WS2811/WS2812/WS2812B RGB-LEDs
* Author: Tim (cpldcpu@gmail.com)
*
* Jan  18th, 2014  v2.0b Initial Version
* March 7th, 2014  v2.1  Added option to retarget the port register during runtime
*                        Removes inlining to allow compiling with c++
*
* License: GNU GPL v2 (see License.txt)
*/

#define LEDS_PIN 13

#include <Arduino.h>

void  ws2815_send_frame(uint8_t *data, uint16_t datlen)
{
  uint8_t maskhi = digitalPinToBitMask(LEDS_PIN);
  volatile uint8_t* port = portOutputRegister(digitalPinToPort(LEDS_PIN));
  volatile uint8_t* portreg = portModeRegister(digitalPinToPort(LEDS_PIN));
  *portreg |= maskhi;

  uint8_t curbyte,ctr,masklo;
  uint8_t sreg_prev;
  
  masklo = ~maskhi & *port;
  maskhi |= *port;
  sreg_prev=SREG;
  cli();  

  while (datlen--) {
    curbyte=*data++;
    
    asm volatile(
    "  ldi   %0,8 \n\t"
    "loop%=:      \n\t"
    "  st    X,%3 \n\t"   //  '1' [02] '0' [02] - re
// W1
//\W1
    "  sbrs  %1,7 \n\t"   //  '1' [04] '0' [03]
    "  st    X,%4 \n\t"   //  '1' [--] '0' [05] - fe-low
    "  lsl   %1   \n\t"   //  '1' [05] '0' [06]
//W2
    "  nop        \n\t"
    "  nop        \n\t"
    "  nop        \n\t"
    "  nop        \n\t"
//\W2
//    "  brcc skipone%= \n\t"    //  '1' [+1] '0' [+2] - 
    "  st   X,%4      \n\t"    //  '1' [+3] '0' [--] - fe-high
//    "skipone%=:               "     //  '1' [+3] '0' [+2] - 
// W3
    "  nop        \n\t"
//\W3
    "  dec   %0     \n\t"    //  '1' [+4] '0' [+3]
    "  brne  loop%= \n\t"    //  '1' [+5] '0' [+4]
    :	"=&d" (ctr)
    :	"r" (curbyte), "x" (port), "r" (maskhi), "r" (masklo)
    );
  }
  
  SREG=sreg_prev;
}
