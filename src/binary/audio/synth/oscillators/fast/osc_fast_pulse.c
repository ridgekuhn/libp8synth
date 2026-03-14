#include "./osc_fast_pulse.h"
#include "../../filters/polyblep.h"

/**
 * Fast Pulse Oscillator
 */
double osc_fast_pulse(int *osc_state, int t, int detune_t) {
  int duty_cycle = *osc_state == 4 ? 0xb000 : 0x8000;

  if (osc_state[21]) {
    duty_cycle += 0x1800;
  }

  const _Bool is_duty = t < duty_cycle;

  /*
   * Primary phasor
   */
  int amplitude = is_duty ? -0x17ff : 0x17ff;
  amplitude +=
      (-amplitude * polyblep((t + duty_cycle) & 0xffff, osc_state[1])) >> 16;
  amplitude -= (amplitude * polyblep(t, osc_state[2])) >> 16;

  /*
   * Detune phasor
   */
  const int detune_partial = detune_t & -0xffff;

  const _Bool is_detune_duty = osc_state[20] == 2
                                   ? ((detune_t << 1) & 0xfffe) < duty_cycle
                                   : detune_partial < duty_cycle;

  int detune_amplitude = is_detune_duty ? -0xbff : 0xbff;
  detune_amplitude +=
      (-detune_amplitude *
       polyblep((detune_partial + duty_cycle) & 0xffff, osc_state[4])) >>
      16;
  detune_amplitude -=
      (detune_amplitude * polyblep(detune_partial, osc_state[4])) >> 16;

  /*
   * Mix samples
   */
  const int osc_amplitude = (osc_state[7] * 3) / 2;
  const int s_pregain = amplitude + detune_amplitude;

  return (s_pregain * osc_amplitude) / 3072.0;
}
