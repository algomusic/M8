// M8 Osc test
#include "M8.h"
#include "M8Osc.h"

M8Osc osc1;

hw_timer_t* timer = nullptr;

// Timer ISR: fires every 9 µs
void IRAM_ATTR onTimer() {
  M8_writeSample(osc1.next());
}

void setup() {

  timer = timerBegin(0);                 // Timer 0
  timerAttachInterrupt(timer, &onTimer); // Attach ISR
  timerAlarm(timer, 9, true, 0);         // 9us, auto-reload, 0=infinite
}

void loop() {
  // Optionally, you could update pwmValue in main loop for custom modulation
}