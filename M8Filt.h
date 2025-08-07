/*
 * M8Filt.h
 *
 * A simple low pass filter class.
 * by Andrew R. Brown 2025
 * This file is part of the M8 audio library.
 * M8 is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 */

#ifndef M8FILT_H_
#define M8FILT_H_

class M8Filt {

public:
  /** Constructor */
  M8Filt() {}

  void setCutoff(float val) { // 0.0 - 1.0
    raw_cutoff = val;
    cutoff = min(1.0, 0.012 + pow(val, 3.5)); // more perceptually salient outcome
  }

  float getCutoff() {
    return raw_cutoff;
  }

  uint8_t next(uint8_t sample) { 
    // exponential moving average filter y[i]=α⋅x[i]+(1−α)⋅y[i−1]  
    prev_sample = (sample * cutoff + prev_sample * (1.0 - cutoff));
    return prev_sample;
  }

   uint8_t nextEnvFollow(uint8_t sample, float followAmnt, float envValue) { 
    float curr_cutoff = min(1.0f, cutoff * (1 + envValue * followAmnt));
    // exponential moving average filter y[i]=α⋅x[i]+(1−α)⋅y[i−1]  
    prev_sample = min(252, (int)(sample * curr_cutoff + prev_sample * (1.0 - cutoff)));
    return prev_sample;
  }

  private:
    uint8_t prev_sample = 0;
    float raw_cutoff = 0.47;
    float cutoff = 0.8;
  };
  
#endif /* M8FILT_H_ */