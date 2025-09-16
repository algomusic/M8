// M8 Enveloped osc with detuned dual oscillator
#include "M8.h"
#include "M8Osc.h"
#include "M8Filt.h"
#include "M8Env.h"

M8Osc osc1;
M8Filt filter1;
M8Env env1;
unsigned long msNow = millis();
unsigned long pitchTime = msNow;
int pitchDelta = 250;
unsigned long followTime = msNow;
int followDelta = 21;
float baseCutoff = 0.1;
float followAmnt = 0.6;
float currentPitch = 69;
float targetPitch = 69;
int stepCounter = 0;
int pcPent [] = {0,2,4,7,9};
int pcTriad [] = {0,4,7,0,0};
int currPc[5];
unsigned long slewTime = msNow;
int slewDelta = 19;
float slewAmnt = 0.5; // 0.0 - 1.0

void setup() {
  Serial.begin(115200);
  setM8PwmPin(8); // any PWM capable GPIO
  osc1.setWave("sawtooth"); // "sine", "triangle", "square", "sawtooth", "noise"
  osc1.setPitch(targetPitch); // A 440
  osc1.setDetune(0.7); // 0.0 - 1.0
  filter1.setCutoff(baseCutoff); // 0.0 - 1.0
  env1.setAttack(0.1); // 0.0 - 1.0
  env1.setDecay(0.6); // 0.0 - 1.0
  // fill current picth class set
  for (int i=0; i<5; i++) {
    if (stepCounter%4 == 0) {
      currPc[i] = pcPent[i];
    } else currPc[i] = pcTriad[i];
  }
  // Start audio with timer-based callback
  startM8Audio(audioCallback);
  Serial.println("M8 filter envelope follow test");
}

void loop() {
  // check if it's time for a new note, if so then generate one
  msNow = millis();

  if ((unsigned long)(msNow - pitchTime) >= pitchDelta) {
    pitchTime += pitchDelta;
    if (stepCounter%4 == 0 || random(10) < 7) {
      targetPitch += (random(15) - 7); // random walk
      targetPitch = pitchQuantize(targetPitch, currPc, 0); // quantise to scale
      if (targetPitch < 55 || targetPitch > 84) targetPitch = 60; // reset if out of range
      // osc1.setPitch();
      filter1.setCutoff(baseCutoff);
      env1.start();
    }
    stepCounter++;
  }

  if ((unsigned long)(msNow - slewTime) >= slewDelta) {
    slewTime += slewDelta;
    currentPitch = targetPitch * (1 - slewAmnt) + currentPitch * slewAmnt;
    osc1.setPitch(currentPitch);
  }
    

  // env follow
  if ((unsigned long)(msNow - followTime) >= followDelta) {
    followTime += followDelta;
    filter1.setCutoff(env1.getValue() * followAmnt + baseCutoff);
  }
}

// Audio callback function - called automatically by the M8 timer
uint8_t audioCallback() {
  // low pass gate
  return  env1.next(filter1.next(osc1.nextDual()));

}

