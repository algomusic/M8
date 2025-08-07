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

class M8Osc {

public:
  /** Constructor.
	* Default wavetable specified
	*/
  M8Osc() {
    setWave("sawtooth"); // default waveform
    setM8PwmPin(M8_pwm_pin);
  }

  void setWave(String wave) {
    if (wave == "sine") {
      curr_wave = wave;
      for (int i = 0; i < table_size; i++) {
        wavetable[i] = floor(sin(2 * 3.1459 * i * table_size_inv) * 127.5 + 127.5);
      }
    } else if (wave == "triangle") {
      curr_wave = wave;
      for (int i = 0; i < table_size; ++i) {
        int phase = (i + wav_phase_offset) % table_size;
        if (phase < table_size / 2) {
            // Rising edge: 0 to 255
            wavetable[i] = (uint8_t)(phase * 2);
        } else {
            // Falling edge: 255 to 0
            wavetable[i] = (uint8_t)(255 - (phase - table_size / 2) * 2);
        }
      }
    } else if (wave == "sawtooth") {
      curr_wave = wave;
      int prevVal = 0;
      for (int i = 0; i < table_size; ++i) {
        int phase = (i + wav_phase_offset) % table_size;
        // wavetable[i] = (uint8_t)phase;  // Linear ramp from 0 to 255
        prevVal = (phase + prevVal)/2;
        wavetable[i] = (uint8_t)prevVal;
        
      }
    } else if (wave == "square") {
      curr_wave = wave;
      for (int i = 0; i < table_size; ++i) {
        if (i < table_size / 2) {
          wavetable[i] = 0;
        } else wavetable[i] = 255;
      }
    } else if (wave == "noise") {
      curr_wave = wave;
    } else {
      Serial.println("M8 error: waveform " + String(wave) + " is not supported.");
    }
  }

  void setFreq(float freq) {
    phase_fractional = freq / 434.0f;
    phase_fractional2 = phase_fractional * (1 - detune);
  }

  void setPitch(float pitch) {
    setFreq(mtof(pitch));
  }

  uint8_t next() {
    if (curr_wave == "noise") {
      return random() * 255;
    } else {
      uint8_t nextVal = wavetable[(int)table_index];
      table_index += phase_fractional;
      if (table_index >= table_size) table_index -= table_size; // wrap
      return nextVal;
    }
  }

  void setDetune(float tune) {
    detune = tune * 0.01;
    phase_fractional2 = phase_fractional * (1 - detune);
  }

  uint8_t nextDual() {
    if (curr_wave == "noise") {
      return random() * 255;
    } else {
      uint8_t nextVal = wavetable[(int)table_index];
      uint8_t nextVal2 = wavetable[(int)table_index2];
      table_index += phase_fractional;
      if (table_index >= table_size) table_index -= table_size; // wrap
      table_index2 += phase_fractional2;
      if (table_index2 >= table_size) table_index2 -= table_size; // wrap
      return (nextVal + nextVal2)>>1;
    }
  }

// private:
  int table_size = 256;
  float table_size_inv = 0.00390625; // 1/TABLE_SIZE;
  uint8_t wavetable [256];
  String curr_wave = "sawtooth";
  uint8_t wav_phase_offset = 127;
  float phase_fractional = 1.0;
  float table_index = 0.0;
  float phase_fractional2 = 0.999;
  float table_index2 = 0.0;
  float detune = 0.002;

};
  
#endif /* M8OSC_H_ */