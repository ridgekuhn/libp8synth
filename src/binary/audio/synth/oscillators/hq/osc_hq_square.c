#include "./osc_hq_square.h"
#include "./../../phasors/hq/phasor_hq_square.h"
#include "./osc_hq_pulse.h"

/**
 * HQ Square Oscillator
 */
double osc_hq_square(int *osc_state, int t, int detune_t) {
  // Buzz
  if (osc_state[21]) {
    return osc_hq_pulse(osc_state, t, detune_t);
  }

  /*
   * Primary phasor
   */
  const int freq = (osc_state[2] * 22050) >> 16;
  double amplitude = phasor_hq_square(t, freq);

  /*
   * Detune phasor
   */
  const int osc_detune_m1 = osc_state[20] == 2 ? 1 : 0;
  const int detune_freq = (osc_state[4] * 22050) >> 16;
  double detune_amplitude = phasor_hq_square(
      (detune_t << osc_detune_m1) & 0xffff, detune_freq << osc_detune_m1);

  /*
   * Mix sample
   */
  amplitude *= 0x5ffc - ((0x2ffe / 64.0) * osc_state[15]);
  detune_amplitude *= 0x2ffc - ((0x17fe / 64.0) * osc_state[15]);

  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const double s_prefader = amplitude + detune_amplitude;

  return (s_prefader * osc_amplitude) / 3072;
}
