#include "./osc_hq_pulse.h"
#include "../../phasors/hq/phasor_hq_pulse.h"

/**
 * HQ Pulse Oscillator
 */
double osc_hq_pulse(int *osc_state, int t, int detune_t) {
  int duty_cycle = *osc_state == 4 ? 0xb000 : 0x8000;

  if (osc_state[21]) {
    duty_cycle += 0x1800;
  }

  /*
   * Primary phasor
   */
  const int freq = (osc_state[2] * 22050) >> 16;
  double amplitude = phasor_hq_pulse(t, freq, duty_cycle);

  /*
   * Detune phasor
   */
  const int osc_detune_m1 = osc_state[20] == 2 ? 1 : 0;
  const int detune_freq = (osc_state[4] * 22050) >> 16;
  double detune_amplitude =
      phasor_hq_pulse((detune_t << osc_detune_m1) & 0xffff,
                      detune_freq << osc_detune_m1, duty_cycle);

  /*
   * Mix sample
   */
  amplitude *= 0x5ffc - ((0x2ffe / 64.0) * osc_state[15]);
  detune_amplitude *= 0x2ffc - ((0x17fe / 64.0) * osc_state[15]);

  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const double s_prefader = amplitude + detune_amplitude;
  return (s_prefader * osc_amplitude) / 3072;
};
