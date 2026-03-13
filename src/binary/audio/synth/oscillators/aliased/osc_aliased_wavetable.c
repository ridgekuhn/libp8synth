#include "./osc_aliased_wavetable.h"

/**
 * Aliased Wavetable Oscillator
 */
double osc_aliased_wavetable(int *osc_state, int t, int detune_t) {
  /*
   * Primary phasor
   */
  const int cur_frame_s = osc_state[24 + ((t >> 10) & 63)];
  const int next_frame_s = osc_state[24 + (((t + 0x400) >> 10) & 63)];
  const double amplitude =
      (((next_frame_s - cur_frame_s) * (t & 1023)) + (cur_frame_s << 10)) >> 10;

  /*
   * Detune phasor
   */
  const int osc_detune_m1 = osc_state[20] == 2 ? 1 : 0;
  const int detune_phase = detune_t << osc_detune_m1;
  const int detune_s = osc_state[24 + ((detune_phase >> 10) & 63)];
  const int next_detune_s =
      osc_state[24 + (((detune_phase + 1024) >> 10) & 63)];

  const double detune_amplitude =
      (((next_detune_s - detune_s) * (detune_phase & 1023)) +
       (detune_s << 10)) >>
      10;

  /*
   * Mix sample
   */
  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const double s_prefader = amplitude + detune_amplitude / 2;

  return (s_prefader * osc_amplitude) / 3072;
}
