#include "./osc_hq_triangle.h"
#include "../../phasors/hq/phasor_hq_tilted.h"
#include "../../phasors/hq/phasor_hq_triangle.h"

/**
 * HQ Triangle Oscillator
 */
double osc_hq_triangle(int *osc_state, int t, int detune_t) {
  /*
   * Primary phasor
   */
  const int freq = (osc_state[2] * 22050) >> 16;
  double amplitude = phasor_hq_triangle(t, freq);

  /*
   * Detune phasor
   */
  const int detune_freq = (osc_state[4] * 22050) >> 16;
  double detune_amplitude = phasor_hq_triangle(detune_t & 0xffff, detune_freq);

  /*
   * Buzz phasors
   */
  if (osc_state[21]) {
    const double tilt_amp = phasor_hq_tilted(t, freq, 0x2001);

    const double detune_tilt_amp =
        phasor_hq_tilted(detune_t & 0xffff, detune_freq, 0x2001);

    amplitude = amplitude * 0.75 + tilt_amp / 2.1;
    detune_amplitude = detune_amplitude * 0.75 + detune_tilt_amp / 2.1;
  }

  /*
   * Mix sample
   */
  amplitude *= 0x2380;
  detune_amplitude *= 0x11c0;

  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const double s_prefader = amplitude + detune_amplitude;

  return (s_prefader * osc_amplitude) / 3072;
}
