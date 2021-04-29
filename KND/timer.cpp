
#include "timer.h"

void Timer::setup() {
  // turn off timer0, we don't need it but breaks micros() function
  TIMSK0 = 0;
  // timer1 settings, for custom clock counter
  TCCR1A = B00000000;
  TCCR1B = B00000101;
  TIMSK1 = B00000000;
  TCNT1  = B00000000;  
  OCR1A  = B00000000;

  memset(cycleDuration, 0, sizeof(cycleDuration));
  updateNow();
  avgNowH = now.raw.nowH;
  avgCount = 0;
  fpsNowH = now.raw.nowH;
  fpsCount = 0;
}

void Timer::updateNow(){
  now.raw.nowL = TCNT1L;
  now.raw.nowH = TCNT1H;
}

const char* Timer::cycleStrings[8] = {
  "Ctr ",
  "Mid ",
  "UI  ",
  "Ped ",
  "Trk ",
  "Avg ",
  "Key ",
  "Fps ",
};

Timer timer;
