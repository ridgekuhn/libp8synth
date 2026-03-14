#include "./osc_aliased_pink_noise.h"
#include "../../../../math/codo_random.h"

/**
 * Aliased Pink Noise Oscillator
 */
double osc_aliased_pink_noise(int *osc_state, int t, int detune_t) {
  /*
   * Primary phasor
   * (detune phasor unused)
   */
  // ((osc_state[6] & 1) == 0) ? 1 : 0;
  const int new_osc_6 = (osc_state[6] & 1) ^ 1;
  osc_state[6] = new_osc_6;

  int new_phase;

  if (osc_state[2] > 78) {
    new_phase = (osc_state[2] << 3) + 0x460;
  } else {
    new_phase = (79 - osc_state[2] * -60) + 0x6d8;
  }

  new_phase = new_phase > 0 ? new_phase : 0;

  const int new_phase_half = new_phase >> 1;

  if (new_osc_6) {
    osc_state[5] = (codo_random(new_phase) - new_phase_half) + osc_state[5];
  }

  const int pink_phase = (osc_state[2] + 500) / 3;

  const _Bool is_duty = (((t + 101) * (t + 317)) & 0x1fff) < pink_phase;

  int new_osc_5;

  if (is_duty) {
    new_osc_5 =
        (codo_random(0x2ffe) - 0x17ff * osc_state[10]) / 1792 + osc_state[5];
    osc_state[5] = new_osc_5;
  } else {
    new_osc_5 = osc_state[5];
  }

  int new_osc_5_clamped = new_osc_5 < 0x17ff ? new_osc_5 : 0x17ff;
  new_osc_5_clamped = new_osc_5_clamped > -0x17ff ? new_osc_5_clamped : -0x17ff;

  osc_state[5] = new_osc_5_clamped;

  /*
   * Mix samples
   */
  int gain = osc_state[9] < 48 ? 64 : osc_state[9] + 16;
  gain = 2048 / gain + 48;
  gain = gain > 64 ? gain : 64;

  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const int s_pregain = new_osc_5 >> 6;

  return ((s_pregain * osc_amplitude) * gain) / 2048.0;
}
