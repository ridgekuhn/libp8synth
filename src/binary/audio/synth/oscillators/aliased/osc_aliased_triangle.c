#include "./osc_aliased_triangle.h"

/**
 * Aliased Triangle Oscillator
 */
double osc_aliased_triangle(int *osc_state, int t, int detune_t) {
  const _Bool is_duty = (t & 0x8000) == 0;
  const _Bool is_detune_duty = (detune_t & 0x8000) == 0;
  const int cur_detune_partial = detune_t & 0xffff;

  int amplitude = is_duty ? t * 3 - 0xc000 : (0xc000 - t) * 3;

  int detune_amplitude = is_detune_duty ? cur_detune_partial * 3 - 0xc000
                                        : (0xc000 - cur_detune_partial) * 3;

  if (osc_state[21]) {
    const _Bool is_buzz_duty = t > 0xdfff;
    const _Bool is_buzz_detune_duty = cur_detune_partial > 0xdfff;
    const int buzz_amp =
        is_buzz_duty ? ((0xffff - t) * 0x5ffc) >> 13 : (t * 0x5ffc) / 0xe000;
    const int detune_buzz_amp =
        is_buzz_detune_duty ? ((cur_detune_partial ^ 0xffff) * 0x5ffc) >> 13
                            : (cur_detune_partial * 0x5ffc) / 0xe000;

    amplitude = (amplitude / 4) * 3 - 0x2ffe + buzz_amp;
    detune_amplitude = (detune_amplitude / 4) * 3 - 0x2ffe + detune_buzz_amp;
  }

  /*
   * Mix samples
   */
  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const int s_pregain = amplitude / 4 + detune_amplitude / 8;

  return (s_pregain * osc_amplitude) / 3072.0;
}
