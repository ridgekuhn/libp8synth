#include "./osc_aliased_tilted.h"

/**
 * Aliased Tilted Oscillator
 */
double osc_aliased_tilted(int *osc_state, int t, int detune_t) {
  const int osc_detune_m1 = osc_state[20] == 2 ? 1 : 0;
  const int detune_phase = detune_t << osc_detune_m1;
  const int detune_phase_partial = detune_phase & 0xffff;

  int amplitude;
  int detune_amplitude;

  if (osc_state[21]) {
    const _Bool is_duty = t > 0xefff;
    const _Bool is_detune_duty = detune_phase_partial > 0xefff;

    amplitude =
        is_duty ? ((0xffff - t) * 0x5ffc) / 0x1000 : (t * 0x5ffc) / 0xf000;

    detune_amplitude = is_detune_duty
                           ? ((detune_phase_partial ^ 0xffff) * 0x5ffc) >> 12
                           : (detune_phase_partial * 0x5ffc) / 0xf000;
  } else {
    const _Bool is_duty = t > 0xdfff;
    const _Bool is_detune_duty = detune_phase_partial > 0xdfff;

    amplitude =
        is_duty ? ((0xffff - t) * 0x5ffc) / 0x2000 : (t * 0x5ffc) / 0xe000;

    detune_amplitude = is_detune_duty
                           ? ((detune_phase_partial ^ 0xffff) * 0x5ffc) >> 13
                           : (detune_phase_partial * 0x5ffc) / 0xe000;
  }

  /*
   * Mix samples
   */
  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const int s_pregain = amplitude - 0x2ffe + (detune_amplitude - 0x2ffe) / 2;

  return (s_pregain * osc_amplitude) / 3072.0;
}
