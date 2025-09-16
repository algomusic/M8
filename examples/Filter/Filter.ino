// M8 Filter test
#include "M8.h"
#include "M8Osc.h"
#include "M8Filt.h"

M8Osc osc1;
M8Filt filter1;
unsigned long msNow = millis();
unsigned long pitchTime = msNow;
int pitchDelta = 5000;
unsigned long filterTime = msNow;
int filterDelta = 20;
float filterVal = 0.5; // 0.0 - 1.0

void setup() {
  Serial.begin(115200);
  setM8PwmPin(8); // any PWM capable GPIO
  osc1.setWave("sawtooth"); // "sine", "triangle", "square", "sawtooth", "noise"
  osc1.setPitch(69); // A 440
  filter1.setCutoff(filterVal); // 0.0 - 1.0
  // Start audio with timer-based callback
  startM8Audio(audioCallback);
  Serial.println("M8 filter test");
}

void loop() {
  msNow = millis();

  // regularly change pitch
  if ((unsigned long)(msNow - pitchTime) >= pitchDelta) {
    pitchTime += pitchDelta;
    int pitch = random(24) + 55;
    osc1.setPitch(pitch);
    Serial.print("New pitch: ");
    Serial.println(pitch);
    filterVal = 0.7; // reset filter
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
  return filter1.next(osc1.next());
}
