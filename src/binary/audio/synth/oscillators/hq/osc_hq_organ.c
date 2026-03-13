#include "./osc_hq_organ.h"
#include "../../phasors/hq/phasor_hq_organ.h"
#include "../../phasors/hq/phasor_hq_square.h"

/**
 * HQ Organ Oscillator
 */
double osc_hq_organ(int *osc_state, int t, int detune_t) {
  /*
   * Primary phasor
   */
  const int freq = (osc_state[2] * 22050) >> 16;
  double amplitude = phasor_hq_organ(t, freq);

  /*
   * Detune phasor
   */
  const int detune_freq = (osc_state[4] * 22050) >> 16;
  const int osc_detune_m1 = osc_state[20] == 2 ? 1 : 0;
  const int detune_partial = detune_t & 0xffff;

  double detune_amplitude = 0;

  if (osc_state[21]) {
    const int m = 1 << osc_detune_m1;

    detune_amplitude = phasor_hq_square(detune_partial * m, detune_freq * m);
  } else {
    detune_amplitude = phasor_hq_organ(detune_partial << osc_detune_m1,
                                       detune_freq << osc_detune_m1) /
                       2;
  }

  /*
   * Mix sample
   */
  amplitude *= 0x1800;
  detune_amplitude *= osc_state[21] ? 0x17fc : 0x1800;

  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const double s_prefader = amplitude + detune_amplitude;

  return (s_prefader * osc_amplitude) / 3072;
}
