// M8 Dual Osc test
#include "M8.h"
#include "M8Osc.h"

M8Osc osc1;
unsigned long now = micros();
unsigned long nextMicros = micros();

void setup() {
  Serial.begin(115200);
  setM8PwmPin(36); // any PWM capable GPIO
  osc1.setWave("sawtooth"); // "sine", "triangle", "square", "sawtooth", "noise"
  osc1.setDetune(0.3); // 0.0 - 1.0
  osc1.setPitch(69); // MIDI pitches
  Serial.println("M8 wave test");
}

void loop() {
  // calculate and send the next audio value
  // always include this in M8 sketches and don't block loop() with delays or complex functions
  now = micros();

  if ((long)(now - nextMicros) >= 0) {
    nextMicros += 9;
    uint8_t nextSample = osc1.nextDual();
    ledcWrite(M8_pwm_pin, nextSample);
  }
}
