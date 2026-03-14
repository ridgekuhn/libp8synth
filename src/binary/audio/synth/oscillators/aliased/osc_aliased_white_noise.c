#include "./osc_aliased_white_noise.h"
#include "../../../../math/codo_random.h"

/**
 * Aliased White Noise Oscillator
 */
double osc_aliased_white_noise(int *osc_state, int t, int detune_t) {
  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const int osc_pitch_decoded = osc_state[8] >> 16;

  int pitch_inverse = 64 - osc_pitch_decoded;
  pitch_inverse = pitch_inverse > 1 ? pitch_inverse : 1;
  pitch_inverse =
      pitch_inverse > 63 ? (pitch_inverse << 2) - 192 : pitch_inverse;

  /*
   * Primary phasor
   */
  double s_pregain;

  if (osc_state[11]) {
    if (osc_state[22] > 1) {
      const int x = (osc_state[11] * osc_amplitude) / pitch_inverse;
      const int y =
          osc_state[12] *
          (((pitch_inverse - osc_state[11]) * osc_amplitude) / pitch_inverse);

      s_pregain = (osc_state[13] * x) + y;
    } else {
      s_pregain = osc_state[12] * osc_amplitude;
    }
  } else {
    osc_state[12] = osc_state[13];
    osc_state[13] = codo_random(0x2ffe) - 0x17ff;

    s_pregain = osc_state[12] * osc_amplitude;
  }

  osc_state[11] = (osc_state[11] + 1) % pitch_inverse;

  /*
   * Mix sample
   */
  return s_pregain / 2048;
}
