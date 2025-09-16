// M8 Filter test
#include "M8.h"
#include "M8Osc.h"
#include "M8Filt.h"
#include "M8Env.h"

M8Osc osc1;
M8Filt filter1;
M8Env env1;
unsigned long msNow = millis();
unsigned long pitchTime = msNow;
int pitchDelta = 500;
unsigned long filterTime = msNow;
int filterDelta = 20;
float filterVal = 0.5; // 0.0 - 1.0
int pitch = 69;

void setup() {
  Serial.begin(115200);
  setM8PwmPin(8); // any PWM capable GPIO
  osc1.setWave("sawtooth"); // "sine", "triangle", "square", "sawtooth", "noise"
  osc1.setPitch(pitch); // A 440
  filter1.setCutoff(filterVal); // 0.0 - 1.0
  env1.setAttack(0.0); // 0.0 - 1.0
  env1.setDecay(0.7); // 0.0 - 1.0
  // Start audio with timer-based callback
  startM8Audio(audioCallback);
  Serial.println("M8 envelope test");
}

void loop() {
  msNow = millis();

  // regularly change pitch
  if ((unsigned long)(msNow - pitchTime) >= pitchDelta) {
    pitchTime += pitchDelta;
    pitch += random(16) - 7; // random walk
    if (pitch < 55 || pitch > 84) pitch = 69;
    osc1.setPitch(pitch);
    Serial.print("New pitch: ");
    Serial.println(pitch);
    filterVal = 0.7; // reset filter
    env1.start(); // trigger envelope
  }

  // sweep filter cutoff
  if ((unsigned long)(msNow - filterTime) >= filterDelta) {
    filterTime += filterDelta;
    filterVal -= 0.002;
    if (filterVal <= 0) filterVal = 1.0;
    filter1.setCutoff(filterVal);
  }

}

// Audio callback function - called automatically by the M8 timer
uint8_t audioCallback() {
  return env1.next(filter1.next(osc1.next()));
  // return osc1.next();
}
