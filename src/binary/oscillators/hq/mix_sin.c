#include <math.h>

/**
 * Mix sin oscillator
 *
 * @param osc_state Mixer channel oscillator state
 * @param tick_buffer Mixer channel tick buffer
 * @param chunk_len Length of tick_buffer in samples
 */
void mix_sin(int *osc_state, short *tick_buffer, int chunk_len) {
  /*
   * Oscillator state
   */
  const _Bool osc_buzz = osc_state[21] != 0;
  const int osc_phase = osc_state[1];
  const int osc_phase_detuned = osc_state[3];
  const int osc_phase_inc = osc_state[2];
  const int osc_vol = osc_state[7];
  const int osc_detune_phase_inc = osc_state[4];
  const int osc_detune = osc_state[20];
  const int osc_detune_m1 = osc_detune == 2 ? 1 : 0;
  const int osc_detune_phase =
      osc_phase_inc == osc_detune_phase_inc ? osc_phase : osc_phase_detuned;
  const int osc_amplitude = (osc_vol * 3) / 2;

  /*
   * Populate buffer
   */
  int cur_phase = osc_phase;
  int cur_detune_phase = osc_detune_phase;

  for (int i = 0; i < chunk_len; i += 1) {
    const double radians = (((double)cur_phase / 0x10000) * M_PI * 2);
    const double amplitude = sin(radians) * 0xc000;

    const double detune_radians =
        (((double)(cur_detune_phase & 0xffff) / 0x10000) * M_PI * 2);
    const double detune_amplitude = sin(radians) * 0xc000;

    const int s_pregain = amplitude / 4 + detune_amplitude / 8;
    const int s = (s_pregain * osc_amplitude) / 3072;
    tick_buffer[i] = (short)s;

    // Increment phase
    cur_phase = (cur_phase + osc_phase_inc) & 0xffff;
    cur_detune_phase = (cur_detune_phase + osc_detune_phase_inc) & 0x1ffff;
  }

  // Update oscillator state
  osc_state[1] = cur_phase;
  osc_state[3] = cur_detune_phase;
}
