#include "./osc_aliased_noise.h"
#include "./osc_aliased_brown_noise.h"
#include "./osc_aliased_pink_noise.h"
#include "./osc_aliased_white_noise.h"

/**
 * Aliased Noise Oscillator
 */
double osc_aliased_noise(int *osc_state, int t, int detune_t) {
  // noiz
  if (osc_state[22]) {
    return osc_aliased_white_noise(osc_state, t, detune_t);
  }

  // buzz
  if (osc_state[21]) {
    return osc_aliased_brown_noise(osc_state, t, detune_t);
  }

  return osc_aliased_pink_noise(osc_state, t, detune_t);
}
