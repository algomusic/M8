/*
 * M8Env.h
 *
 * An amplitude envelope class.
 * by Andrew R. Brown 2025
 * This file is part of the M8 audio library.
 * M8 is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 */

#ifndef M8ENV_H_
#define M8ENV_H_

class M8Env {

public:
  /** Constructor */
  M8Env() {}

  void setAttack(float att) { // 0.0 - 1.0
    raw_attack = att;
    attack = 0.01 * (1.0 - pow(att, 0.03)); 
  }

  float getAttack() {
    return raw_attack;
  }

  void setDecay(float dec) { // 0.0 - 1.0
    raw_decay = dec;
    decay = 0.999 + pow(dec, 0.4) * 0.001;
  }

  float getDecay() {
    return raw_decay;
  }

  void start() {
    attack_stage = true;
    env_val = 0.01; // reset
  }

  uint8_t next(uint8_t sample) {
    if (attack_stage) { //attack;
      env_val += attack; 
      if (env_val >= env_gain) attack_stage = false;
      return round(sample * env_val);
    } else { // decay
      env_val *= decay;
      return round(sample * env_val);
    }
  }

  float getValue() {
    return env_val;
  }

  void setGain(float val) {
    env_gain = min(1.0f, max(0.0f, val));
  }

private:
  float attack = 0;
  float raw_attack = 0;
  float decay = 0.999757858;
  float raw_decay = 0.5;
  bool attack_stage = false;
  float env_val = 0.01;
  float env_gain = 1.0;

};
  
#endif /* M8ENV_H_ */