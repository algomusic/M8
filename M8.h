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
int M8_pwm_pin = 5; // GPIO 5 suits most ESP32 types and boards

// Timer-based audio system
hw_timer_t * M8_timer = NULL;
portMUX_TYPE M8_timerMux = portMUX_INITIALIZER_UNLOCKED;
bool M8_timer_enabled = false;

// Callback function pointer for user audio processing
typedef uint8_t (*M8AudioCallback)(void);
M8AudioCallback M8_audio_callback = nullptr;

// Timer interrupt service routine
void IRAM_ATTR M8_onTimer() {
  if (M8_audio_callback != nullptr) {
    portENTER_CRITICAL_ISR(&M8_timerMux);
    uint8_t sample = M8_audio_callback();
    ledcWrite(M8_pwm_pin, sample);
    portEXIT_CRITICAL_ISR(&M8_timerMux);
  }
}

/** Set the PWM pin and initialize PWM
* @pin The GPIO pin to use for PWM output
*/
void setM8PwmPin(int pin) {
  M8_pwm_pin = pin;
  ledcAttach(M8_pwm_pin, SAMPLE_RATE, 8); // sampleRate, bitDepth
}

/** Stop the audio timer */
void stopM8Audio() {
  if (M8_timer_enabled && M8_timer != NULL) {
    timerEnd(M8_timer);
    M8_timer = NULL;
    M8_audio_callback = nullptr;
    M8_timer_enabled = false;
  }
}

/** Start the audio timer with a callback function
* @callback Function that returns the next audio sample (uint8_t)
* @sampleRate Sample rate in Hz (default 48000)
*/
void startM8Audio(M8AudioCallback callback, int sampleRate = SAMPLE_RATE) {
  if (M8_timer_enabled) {
    stopM8Audio(); // Stop existing timer if running
  }
  
  M8_audio_callback = callback;
  
  // Initialize timer with 1MHz frequency
  M8_timer = timerBegin(1000000);
  timerAttachInterrupt(M8_timer, &M8_onTimer);
  
  // Calculate timer interval in microseconds
  // 1,000,000 microseconds / sample_rate = microseconds per sample
  int intervalMicros = 1000000 / sampleRate;
  timerAlarm(M8_timer, intervalMicros, true, 0);
  
  M8_timer_enabled = true;
}

/** Check if audio timer is running */
bool isM8AudioRunning() {
  return M8_timer_enabled;
}

/** Return freq from a MIDI pitch 
* @pitch The MIDI pitch to be converted
*/
float mtof(int pitch){
  return 8.1757989 * pow(2.0, pitch * 0.083333);
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