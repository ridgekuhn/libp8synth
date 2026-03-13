#include "./osc_hq_tilted.h"
#include "../../phasors/hq/phasor_hq_tilted.h"

/**
 * HQ Tilted Oscillator
 */
double osc_hq_tilted(int *osc_state, int t, int detune_t) {
  const int freq = (osc_state[2] * 22050) >> 16;
  const int detune_freq = (osc_state[4] * 22050) >> 16;
  const int osc_detune_m1 = osc_state[20] == 2 ? 1 : 0;
  const int detune_phase = detune_t << osc_detune_m1;

  double amplitude = 0;
  double detune_amplitude = 0;

  if (osc_state[21]) {
    amplitude = phasor_hq_tilted(t, freq, 0x1001);

    detune_amplitude =
        phasor_hq_tilted(detune_phase & 0xffff, detune_freq, 0x1001);

    amplitude *= 0x5554;
    detune_amplitude *= 0x2aaa;
  } else {
    amplitude = phasor_hq_tilted(t, freq, 0x2001);

    detune_amplitude =
        phasor_hq_tilted(detune_phase & 0xffff, detune_freq, 0x2001);

    amplitude *= 0x4380;
    detune_amplitude *= 0x21c0;
  }

  /*
   * Mix sample
   */
  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const double s_prefader = amplitude + detune_amplitude;

  return (s_prefader * osc_amplitude) / 3072;
}
