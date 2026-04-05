/*
 * M8Osc.h
 *
 * A wavetable oscillator class. Contains generators for common wavetables.
 * by Andrew R. Brown 2025
 * This file is part of the M8 audio library.
 * M8 is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 */

#ifndef M8OSC_H_
#define M8OSC_H_

enum M8WaveType {
  M8_WAVE_SINE,
  M8_WAVE_TRIANGLE,
  M8_WAVE_SAWTOOTH,
  M8_WAVE_SQUARE,
  M8_WAVE_NOISE
};

class M8Osc {

public:
  /** Constructor.
	* Generates default sawtooth wavetable using integer math only — safe during static init.
	*/
  M8Osc() {
    // Default sawtooth — no String, no Arduino calls, pure integer math
    int prevVal = 0;
    for (int i = 0; i < table_size; ++i) {
      int phase = (i + wav_phase_offset) % table_size;
      prevVal = (phase + prevVal) / 2;
      wavetable[i] = (uint8_t)prevVal;
    }
  }

  void setWave(String wave) {
    if (wave == "sine") {
      wave_type = M8_WAVE_SINE;
      for (int i = 0; i < table_size; i++) {
        wavetable[i] = floor(sin(2 * 3.1459 * i * table_size_inv) * 127.5 + 127.5);
      }
    } else if (wave == "triangle") {
      wave_type = M8_WAVE_TRIANGLE;
      for (int i = 0; i < table_size; ++i) {
        int phase = (i + wav_phase_offset) % table_size;
        if (phase < table_size / 2) {
            wavetable[i] = (uint8_t)(phase * 2);
        } else {
            wavetable[i] = (uint8_t)(255 - (phase - table_size / 2) * 2);
        }
      }
    } else if (wave == "sawtooth") {
      wave_type = M8_WAVE_SAWTOOTH;
      int prevVal = 0;
      for (int i = 0; i < table_size; ++i) {
        int phase = (i + wav_phase_offset) % table_size;
        prevVal = (phase + prevVal)/2;
        wavetable[i] = (uint8_t)prevVal;
      }
    } else if (wave == "square") {
      wave_type = M8_WAVE_SQUARE;
      int prevVal = 255;
      for (int i = 0; i < table_size; ++i) {
        if (i < table_size / 2) {
          prevVal = (0 + prevVal*7)/8;
          wavetable[i] = prevVal;
        } else {
          prevVal = (255 + prevVal*7)/8;
          wavetable[i] = prevVal;
        }
      }
    } else if (wave == "noise") {
      wave_type = M8_WAVE_NOISE;
    } else {
      Serial.println("M8 error: waveform " + String(wave) + " is not supported.");
    }
  }

  void setFreq(float freq) {
    phase_fractional = freq / 390.0f;
    phase_fractional2 = phase_fractional * (1 - detune);
  }

  void setPitch(float pitch) {
    setFreq(mtof(pitch));
  }

  uint8_t IRAM_ATTR next() {
    if (wave_type == M8_WAVE_NOISE) {
      return (uint8_t)(esp_random() & 0xFF);
    } else {
      uint8_t nextVal = wavetable[(int)table_index];
      table_index += phase_fractional;
      if (table_index >= (float)table_size) table_index -= (float)table_size; // wrap
      return nextVal;
    }
  }

  void setDetune(float tune) {
    detune = tune * 0.01f;
    phase_fractional2 = phase_fractional * (1.0f - detune);
  }

  uint8_t IRAM_ATTR nextDual() {
    if (wave_type == M8_WAVE_NOISE) {
      return (uint8_t)(esp_random() & 0xFF);
    } else {
      uint8_t nextVal = wavetable[(int)table_index];
      uint8_t nextVal2 = wavetable[(int)table_index2];
      table_index += phase_fractional;
      if (table_index >= (float)table_size) table_index -= (float)table_size; // wrap
      table_index2 += phase_fractional2;
      if (table_index2 >= (float)table_size) table_index2 -= (float)table_size; // wrap
      return (nextVal + nextVal2)>>1;
    }
  }


// private:
  M8WaveType wave_type = M8_WAVE_SAWTOOTH;
  int table_size = 256;
  float table_size_inv = 0.00390625; // 1/TABLE_SIZE;
  uint8_t wavetable [256];
  uint8_t wav_phase_offset = 127;
  float phase_fractional = 1.0;
  float table_index = 0.0;
  float phase_fractional2 = 0.999;
  float table_index2 = 0.0;
  float detune = 0.002;
};

#endif /* M8OSC_H_ */
