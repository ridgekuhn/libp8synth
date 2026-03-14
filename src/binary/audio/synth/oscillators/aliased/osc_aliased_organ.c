#include "./osc_aliased_organ.h"

/**
 * Aliased Organ Oscillator
 */
double osc_aliased_organ(int *osc_state, int t, int detune_t) {
  const _Bool is_duty = (t & 0x8000) == 0;
  const _Bool is_sub_duty = (t & 0x4000) != 0;

  /*
   * Primary phasor
   */
  int amplitude;

  if (is_duty) {
    if (is_sub_duty) {
      amplitude = 0x8000 - t;
    } else {
      amplitude = t;
    }
  } else {
    if (is_sub_duty) {
      amplitude = 0x10000 - t;
    } else {
      amplitude = t - 0x8000;
    }

    amplitude = (amplitude << 1) / 3;
  }

  /*
   * Detune phasor
   */
  const int osc_detune_m1 = osc_state[20] == 2 ? 1 : 0;

  int detune_amplitude;

  if (osc_state[21]) {
    const int buzz_detune = 0x8000 >> osc_detune_m1;
    const _Bool is_detune_duty = (detune_t & buzz_detune) == 0;

    detune_amplitude = is_detune_duty ? -0x5ff : 0x5ff;
  } else {
    const int detune = detune_t << osc_detune_m1;
    const int detune_partial = detune & 0xffff;
    const _Bool is_detune_duty = (detune & 0x8000) == 0;
    const _Bool is_detune_sub_duty = (detune & 0x4000) != 0;

    if (is_detune_duty) {
      if (is_detune_sub_duty) {
        detune_amplitude = 0x8000 - detune_partial;
      } else {
        detune_amplitude = detune_partial;
      }
    } else {
      if (is_detune_sub_duty) {
        detune_amplitude = 0x10000 - detune_partial;
      } else {
        detune_amplitude = detune_partial - 0x8000;
      }

      detune_amplitude = (detune_amplitude << 1) / 3;
    }

    detune_amplitude = (detune_amplitude - 0x2000) / 2;
  }

  /*
   * Mix samples
   */
  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const int s_pregain = amplitude + detune_amplitude - 0x2000;

  return (s_pregain * osc_amplitude) / 3072.0;
}
