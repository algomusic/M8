// M8 Dual Osc test
#include "M8.h"
#include "M8Osc.h"

M8Osc osc1;
unsigned long msNow = millis();
unsigned long pitchTime = msNow;
int pitchDelta = 2000;

void setup() {
  Serial.begin(115200);
  setM8PwmPin(8); // any PWM capable GPIO
  osc1.setWave("sawtooth"); // "sine", "triangle", "square", "sawtooth", "noise"
  osc1.setDetune(0.3); // 0.0 - 1.0
  osc1.setPitch(69); // MIDI pitches
  // Start audio with timer-based callback
  startM8Audio(audioCallback);
  Serial.println("M8 dual oscillator test");
}

void loop() {
  msNow = millis();

  if ((unsigned long)(msNow - pitchTime) >= pitchDelta) {
    pitchTime += pitchDelta;
    int pitch = random(24) + 48;
    osc1.setPitch(pitch);
    Serial.print("New pitch: ");
    Serial.println(pitch);
  }
}

// Audio callback function - called automatically by the M8 timer
uint8_t audioCallback() {
  return osc1.nextDual();
}


