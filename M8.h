/*
 * M8.h
 *
 * M8 is a minimal 8-bit audio synthesis library for ESP32 microprocessors using PWM output on a GPIO pin.
 * by Andrew R. Brown 2025
 * This file is part of the M8 audio library.
 * M8 is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 */

#ifndef M8_H_
#define M8_H_

#include "Arduino.h"
#include "driver/ledc.h"

#if defined(CONFIG_IDF_TARGET_ESP32)
  #include "soc/ledc_struct.h"
  #define M8_LEDC_MODE LEDC_HIGH_SPEED_MODE
#else
  #define M8_LEDC_MODE LEDC_LOW_SPEED_MODE
#endif

// globals
#define SAMPLE_RATE 44100
int M8_pwm_pin = 5; // GPIO 5 suits most ESP32 types and boards

// Fixed LEDC channel/timer for ISR-safe access
#define M8_LEDC_CHANNEL LEDC_CHANNEL_0
#define M8_LEDC_TIMER LEDC_TIMER_0

// Timer-based audio system
hw_timer_t * M8_timer = NULL;
bool M8_timer_enabled = false;

// Callback function pointer for user audio processing
typedef uint8_t (*M8AudioCallback)(void);
M8AudioCallback M8_audio_callback = nullptr;

// Audio task state
volatile uint8_t M8_current_sample = 128;
TaskHandle_t M8_audioTaskHandle = NULL;

// ISR-safe sample output — direct register writes, no flash access
void IRAM_ATTR M8_writeSample(uint8_t sample) {
  #if defined(CONFIG_IDF_TARGET_ESP32)
    LEDC.channel_group[M8_LEDC_MODE].channel[M8_LEDC_CHANNEL].duty.duty = (uint32_t)sample << 4;
    LEDC.channel_group[M8_LEDC_MODE].channel[M8_LEDC_CHANNEL].conf0.sig_out_en = 1;
    LEDC.channel_group[M8_LEDC_MODE].channel[M8_LEDC_CHANNEL].conf1.duty_start = 1;
  #else
    ledcWrite(M8_pwm_pin, sample);
  #endif
}

// Minimal timer ISR — only writes pre-computed sample + notifies audio task
void IRAM_ATTR M8_onTimer() {
  M8_writeSample(M8_current_sample);
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR(M8_audioTaskHandle, &xHigherPriorityTaskWoken);
  if (xHigherPriorityTaskWoken) portYIELD_FROM_ISR();
}

// Audio processing task — runs in normal task context, safe to access all memory
void M8_audioTaskFunc(void * param) {
  while (true) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    if (M8_audio_callback != nullptr) {
      M8_current_sample = M8_audio_callback();
    }
  }
}

/** Set the PWM pin and initialize PWM
* @pin The GPIO pin to use for PWM output
*/
void setM8PwmPin(int pin) {
  M8_pwm_pin = pin;

  #if defined(CONFIG_IDF_TARGET_ESP32)
    // Original dual-core ESP32: use ESP-IDF LEDC directly for known channel
    // (needed for ISR-safe direct register writes)
    ledc_timer_config_t timer_conf = {};
    timer_conf.speed_mode = (ledc_mode_t)M8_LEDC_MODE;
    timer_conf.duty_resolution = LEDC_TIMER_8_BIT;
    timer_conf.timer_num = M8_LEDC_TIMER;
    timer_conf.freq_hz = SAMPLE_RATE;
    timer_conf.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t ch_conf = {};
    ch_conf.gpio_num = pin;
    ch_conf.speed_mode = (ledc_mode_t)M8_LEDC_MODE;
    ch_conf.channel = M8_LEDC_CHANNEL;
    ch_conf.timer_sel = M8_LEDC_TIMER;
    ch_conf.duty = 0;
    ch_conf.hpoint = 0;
    ledc_channel_config(&ch_conf);
  #else
    // Single-core variants (S2, S3, C3, etc.): Arduino API is fine
    // (no dual-core flash cache issue, ledcWrite works from any context)
    ledcAttach(M8_pwm_pin, SAMPLE_RATE, 8);
  #endif
}

/** Stop the audio timer */
void stopM8Audio() {
  if (M8_timer_enabled && M8_timer != NULL) {
    timerEnd(M8_timer);
    M8_timer = NULL;
    M8_audio_callback = nullptr;
    M8_timer_enabled = false;
  }
  if (M8_audioTaskHandle != NULL) {
    vTaskDelete(M8_audioTaskHandle);
    M8_audioTaskHandle = NULL;
  }
}

/** Start the audio timer with a callback function
* @callback Function that returns the next audio sample (uint8_t)
* @sampleRate Sample rate in Hz (default 44100)
*/
void startM8Audio(M8AudioCallback callback, int sampleRate = SAMPLE_RATE) {
  if (M8_timer_enabled) {
    stopM8Audio(); // Stop existing timer if running
  }

  M8_audio_callback = callback;

  // Create audio processing task on Core 0 at high priority
  xTaskCreatePinnedToCore(M8_audioTaskFunc, "M8Audio", 4096, NULL, configMAX_PRIORITIES - 1, &M8_audioTaskHandle, 0);

  // Initialize timer with 1MHz frequency
  M8_timer = timerBegin(1000000);
  timerAttachInterrupt(M8_timer, &M8_onTimer);

  // Calculate timer interval in microseconds
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

#endif /* M8_H_ */
