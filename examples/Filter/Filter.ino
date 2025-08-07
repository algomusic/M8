// M8 Filter test
#include "M8.h"
#include "M8Osc.h"
#include "M8Filt.h"

M8Osc osc1;
M8Filt filter1;
unsigned long now = micros();
unsigned long nextMicros = micros();

void setup() {
  Serial.begin(115200);
  setM8PwmPin(36); // any PWM capable GPIO
  osc1.setWave("sawtooth"); // "sine", "triangle", "square", "sawtooth", "noise"
  osc1.setPitch(69); // A 440
  filter1.setCutoff(0.5); // 0.0 - 1.0
  Serial.println("M8 filter test");
}

void loop() {
  // calculate and send the next audio value at regular intervals
  // always include this in M8 sketches and don't block loop() with delays, print statements or complex functions
  now = micros();

  if ((long)(now - nextMicros) >= 0) {
    nextMicros += 9;
    uint8_t nextSample = filter1.next(osc1.next());
    ledcWrite(M8_pwm_pin, nextSample);
  }
}
