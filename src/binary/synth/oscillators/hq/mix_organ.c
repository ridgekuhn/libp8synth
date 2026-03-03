/**
 * Mix organ oscillator
 *
 * @param osc_state Mixer channel oscillator state
 * @param tick_buffer Mixer channel tick buffer
 * @param chunk_len Length of tick_buffer in samples
 */
void mix_organ(int *osc_state, short *tick_buffer, int chunk_len) {
  /*
   * Oscillator state
   */
  const int osc_phase = osc_state[1];
  const int osc_phase_inc = osc_state[2];
  const int osc_phase_detuned = osc_state[3];
  const int osc_detune_phase_inc = osc_state[4];
  const int osc_vol = osc_state[7];
  const int osc_detune = osc_state[20];
  const _Bool osc_buzz = osc_state[21] != 0;
  const int osc_detune_m1 = osc_detune == 2 ? 1 : 0;
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
    double amplitude = (double)sample_organ(freq, cur_phase) / 0x100000;

    /*
     * Detune phasor
     */
    const int detune_partial = cur_detune_phase & 0xffff;

    double detune_amplitude = 0;

    if (osc_buzz) {
      const int m = 1 << osc_detune_m1;

      detune_amplitude =
          (double)sample_square(detune_freq * m, detune_partial * m) / 0x100000;
    } else {
      detune_amplitude =
          (double)sample_organ(detune_freq, detune_partial << osc_detune_m1) /
          0x100000 / 2;
    }

    /*
     * Mix sample
     */
    amplitude *= 0x1800;
    detune_amplitude *= osc_buzz ? 0x17fc : 0x1800;

    const double s_prefader = amplitude + detune_amplitude;
    const double s = (s_prefader * osc_amplitude) / 3072;
    tick_buffer[i] = (short)s;

    // Increment phase
    cur_phase = (cur_phase + osc_phase_inc) & 0xffff;
    cur_detune_phase = (cur_detune_phase + osc_detune_phase_inc) & 0xffff;
  }

  // Update oscillator state
  osc_state[1] = cur_phase;
  osc_state[3] = cur_detune_phase;
}
