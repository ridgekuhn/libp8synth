#include "./phasors/phasor_hq_tilted.h"
#include "./phasors/phasor_hq_triangle.h"

/**
 * Mix triangle oscillator
 *
 * @param osc_state Mixer channel oscillator state
 * @param tick_buffer Mixer channel tick buffer
 * @param chunk_len Length of tick_buffer in samples
 */
void mix_triangle(int *osc_state, short *tick_buffer, int chunk_len) {
  /*
   * Oscillator state
   */
  const _Bool osc_buzz = osc_state[21] != 0;
  const int osc_phase = osc_state[1];
  const int osc_phase_detuned = osc_state[3];
  const int osc_phase_inc = osc_state[2];
  const int osc_vol = osc_state[7];
  const int osc_detune_phase_inc = osc_state[4];
  const int osc_detune_phase =
      osc_phase_inc == osc_detune_phase_inc ? osc_phase : osc_phase_detuned;
  const int osc_amplitude = (osc_vol * 3) / 2;

  /*
   * Buffer Constants
   */
  const int freq = (osc_phase_inc * 22050) >> 16;
  const int detune_freq = (osc_detune_phase_inc * 22050) >> 16;

  /*
   * Populate buffer
   */
  int cur_phase = osc_phase;
  int cur_detune_phase = osc_detune_phase;

  for (int i = 0; i < chunk_len; i += 1) {
    /*
     * Primary phasor
     */
    double amplitude = phasor_hq_triangle(cur_phase, freq);

    /*
     * Detune phasor
     */
    double detune_amplitude =
        phasor_hq_triangle(cur_detune_phase & 0xffff, detune_freq);

    /*
     * Buzz phasors
     */
    if (osc_buzz) {
      const double tilt_amp = phasor_hq_tilted(cur_phase, freq, 0x2001);

      const double detune_tilt_amp =
          phasor_hq_tilted(cur_detune_phase & 0xffff, detune_freq, 0x2001);

      amplitude = amplitude * 0.75 + tilt_amp / 2.1;
      detune_amplitude = detune_amplitude * 0.75 + detune_tilt_amp / 2.1;
    }

    /*
     * Mix sample
     */
    amplitude *= 0x2380;
    detune_amplitude *= 0x11c0;

    // Write new sample
    const double s_prefader = amplitude + detune_amplitude;
    const double s = (s_prefader * osc_amplitude) / 3072;
    tick_buffer[i] = (short)s;

    // Increment phase
    cur_phase = (cur_phase + osc_phase_inc) & 0xffff;
    cur_detune_phase = (cur_detune_phase + osc_detune_phase_inc) & 0x1ffff;
  }

  // Update oscillator state
  osc_state[1] = cur_phase;
  osc_state[3] = cur_detune_phase;
}
