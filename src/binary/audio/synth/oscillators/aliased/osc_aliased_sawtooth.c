#include "./osc_aliased_sawtooth.h"

/**
 * Aliased Sawtooth Oscillator
 */
double osc_aliased_sawtooth(int *osc_state, int t, int detune_t) {
  const int osc_detune_m1 = osc_state[20] == 2 ? 1 : 0;
  const int detune_phase = detune_t << osc_detune_m1;
  const int detune_partial = detune_phase & 0xffff;

  int amplitude;
  int detune_amplitude;

  if (osc_state[21]) {
    const int cur_phase_partial = t & 0xffff;

    amplitude = (t / 2 - 0x8000 + (cur_phase_partial - 0x8000)) / 8;
    detune_amplitude =
        (detune_phase / 2 - 0x8000 + (detune_partial - 0x8000)) / 16;
  } else {
    amplitude = (t - 0x8000) / 4;
    detune_amplitude = (detune_partial - 0x8000) / 8;
  }

  /*
   * Mix samples
   */
  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const int s_pregain = amplitude + detune_amplitude;

  return (s_pregain * osc_amplitude) / 3072.0;
}
