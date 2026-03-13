#include "./osc_hq_sawtooth.h"
#include "../../phasors/hq/phasor_hq_sawtooth.h"

/**
 * HQ Sawtooth Oscillator
 */
double osc_hq_sawtooth(int *osc_state, int t, int detune_t) {
  /*
   * Primary phasor
   */
  const int freq = (osc_state[2] * 22050) >> 16;
  double amplitude = phasor_hq_sawtooth(t, freq);

  /*
   * Detune phasor
   */
  const int osc_detune_m1 = osc_state[20] == 2 ? 1 : 0;
  const int detune_phase = detune_t << osc_detune_m1;
  const int detune_freq = ((osc_state[4] * 22050) >> 16) << osc_detune_m1;

  double detune_amplitude =
      phasor_hq_sawtooth(detune_phase & 0xffff, detune_freq);

  /*
   * Buzz phasors
   *
   * If buzz enabled, overlay additional saw waves, pitched down one octave
   */
  if (osc_state[21]) {
    // Primary phasor
    amplitude += phasor_hq_sawtooth(t, freq) / 2;

    // Detune phasor
    detune_amplitude +=
        (phasor_hq_sawtooth(detune_phase / 4, detune_freq / 4)) * 2;

    amplitude /= 2;
    detune_amplitude /= 2;
  }

  /*
   * Mix sample
   */
  amplitude *= 0x7000;
  detune_amplitude *= 0x3800;

  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const double s_prefader = amplitude + detune_amplitude;

  return (s_prefader * osc_amplitude) / 3072;
}
