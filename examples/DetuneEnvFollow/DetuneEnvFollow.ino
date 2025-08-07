// M8 Enveloped osc with detuned dual oscillator
#include "M8.h"
#include "M8Osc.h"
#include "M8Filt.h"
#include "M8Env.h"

M8Osc osc1;
M8Filt filter1;
M8Env env1;
unsigned long now = micros();
unsigned long nextMicros = micros();
unsigned long msNow = millis();
unsigned long nextNote = millis();
int stepCounter = 0;
int pcPent [] = {0,2,4,7,9};
int pcTriad [] = {0,4,7,0,0};

void setup() {
  Serial.begin(115200);
  setM8PwmPin(36); // any PWM capable GPIO
  osc1.setWave("sawtooth"); // "sine", "triangle", "square", "sawtooth", "noise"
  osc1.setPitch(69); // A 440
  osc1.setDetune(0.7); // 0.0 - 1.0
  filter1.setCutoff(0.4); // 0.0 - 1.0
  env1.setAttack(0.0); // 0.0 - 1.0
  env1.setDecay(0.7); // 0.0 - 1.0
  Serial.println("M8 envelope test");
}

void loop() {
  // calculate and send the next audio value
  // always include this in M8 sketches and don't block loop() with delays, print statements or complex functions
  now = micros();

  if ((long)(now - nextMicros) >= 0) {
    nextMicros += 9;
    uint8_t nextSample = env1.next(filter1.nextEnvFollow(osc1.nextDual(), 0.3, env1.getValue()));
    ledcWrite(M8_pwm_pin, nextSample);
  }

  // check if it's time for a new note, if so then generate one
  msNow = millis();
  
  if (msNow - nextNote > 250 || msNow - nextNote < 0) {
    nextNote = msNow;
    int currPc[5];
    for (int i=0; i<5; i++) {
      if (stepCounter%4 == 0) {
        currPc[i] = pcPent[i];
      } else currPc[i] = pcTriad[i];
    }
    osc1.setPitch(pitchQuantize(random(36) + 24, currPc, 0));
    env1.start();
    stepCounter++;
  }
}
