#include "./osc_aliased_brown_noise.h"
#include "../../../../math/codo_random.h"

/**
 * Aliased Brown Noise Oscillator
 */
double osc_aliased_brown_noise(int *osc_state, int t, int detune_t) {
  /*
   * Primary phasor
   * (detune phasor not used)
   */
  // ((osc_state[6] & 1) == 0) ? 1 : 0;
  const int new_osc_6 = (osc_state[6] & 1) ^ 1;
  osc_state[6] = new_osc_6;

  int new_phase;

  if (osc_state[2] > 78) {
    new_phase = (osc_state[2] << 3) + 0x460;
  } else {
    new_phase = ((79 - osc_state[2]) * -60) + 0x6d8;
  }

  new_phase = new_phase > 0 ? new_phase : 0;

  const int new_phase_half = new_phase >> 1;
  const int new_osc_5 =
      new_osc_6 ? codo_random(new_phase) - new_phase_half + osc_state[5]
                : osc_state[5];

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
