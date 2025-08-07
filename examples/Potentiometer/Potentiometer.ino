// M8 4 Potentiometer test
// Move the first pot to chnage pitch and trigger notes
// Pot 2 is filter cutoff, pot 3 is attack, pot 4 is decay
#include "M8.h"
#include "M8Osc.h"
#include "M8Filt.h"
#include "M8Env.h"

M8Osc osc1;
M8Filt filter1;
M8Env env1;
unsigned long now = micros();
unsigned long nextMicros = micros();
unsigned long nextNote = micros();
unsigned long msNow = millis();
unsigned long potTime = millis();
int pots [] = {69,0,0,0};
int prevPots [] = {0,0,0,0};
int potReadIndex = 0;

void setup() {
  Serial.begin(115200);
  setM8PwmPin(36); // any PWM capable GPIO
  osc1.setWave("sawtooth"); // "sine", "triangle", "square", "sawtooth", "noise"
  osc1.setPitch(69); // A 440
  filter1.setCutoff(0.5); // 0.0 - 1.0
  env1.setAttack(0.0); // 0.0 - 1.0
  env1.setDecay(0.5); // 0.0 - 1.0
  Serial.println("M8 Potentiometer Control test");
}

void loop() {
  // calculate and send the next audio value at regular intervals
  // always include this in M8 sketches and don't block loop() with delays, print statements or complex functions
  now = micros();

  if ((long)(now - nextMicros) >= 0) {
    nextMicros += 9;
    uint8_t nextSample = env1.next(filter1.nextEnvFollow(osc1.next(), 0.2, env1.getValue()));
    ledcWrite(M8_pwm_pin,nextSample);
  }

  // check the poteniomenters and trigger notes when pitch changes
  if ((long)(now - potTime) >= 0) {
    potTime = now + 10000;
    int read = analogRead(potReadIndex + 1)>>4;
    if (abs(read - prevPots[potReadIndex]) > 1) {
      pots[potReadIndex] = read;
      prevPots[potReadIndex] = read;
      if (potReadIndex == 0) {
        osc1.setPitch(48 + (pots[0]>>3));
        env1.start();
      }
      if (potReadIndex == 1) {
       filter1.setCutoff(pots[1] * 0.00392);  // / 255.0f);
      }
      if (potReadIndex == 2) {
        env1.setAttack(pots[2] * 0.00392);
      }
      if (potReadIndex == 3) {
        env1.setDecay(pots[3] * 0.00392);
      }
    }
    potReadIndex = (potReadIndex + 1)%4; // increment
  }
}
