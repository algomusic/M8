/*
 * M8.h
 *
 * M8 is a minimal 8-bit audio synthesis library for ESP32 microprocessors using PWM output on a GPIO pin.
 * by Andrew R. Brown 2025
 * This file is part of the M8 audio library.
 * M8 is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 */

#include "Arduino.h"

// globals
#define SAMPLE_RATE 44100
int M8_pwm_pin = 10;
unsigned long next_callback_time = micros();

// audio callback - callback overhead seems to take too much time
// Anyone with a good solution to using a timer instead please send through a code example :)
// 1/440 = 0.002272727 sec per cycle
// by 256 = 0.000008878 sec per sample - approx 9 micros
// void writeSample(uint8_t sampleVal) {
//   unsigned long M8_now = micros();
//   if (M8_now - next_callback_time > 9 || M8_now - next_callback_time < 0) {
//     next_callback_time = M8_now;
//     ledcWrite(M8_pwm_pin, sampleVal);
//   }
// }

/** Return freq from a MIDI pitch 
* @pitch The MIDI pitch to be converted
*/
float mtof(int pitch){
  return 8.1757989 * pow(2.0, pitch * 0.083333);
}

void setM8PwmPin(int pin) {
  M8_pwm_pin = pin;
  ledcAttach(M8_pwm_pin, SAMPLE_RATE, 8); // sampleRate, bitDepth
}

/** Return closest scale pitch to a given MIDI pitch
* @pitch MIDI pitch number
* @pitchClassSet an int array of chromatic values, 0-11, of size 12 (padded with zeros as required)
* @key pitch class key, 0-11, where 0 = C root
*/
int pitchQuantize(int pitch, int * pitchClassSet, int key) {
  for (int j=0; j<12; j++) {
    int pitchClass = pitch%12;
    bool adjust = true;
    for (int i=0; i < 12; i++) {
      if (pitchClass == pitchClassSet[i] + key) {
        adjust = false;
      }
      if (pitchClass == pitchClassSet[i] - key) {
        adjust = false;
      }
    }
    if (adjust) {
      pitch -= 1;
    } else return pitch;
  }
  return pitch; // just in case
}

// /* M8_H_ */