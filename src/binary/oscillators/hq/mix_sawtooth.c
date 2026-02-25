// #include "./phasors/sample_sawtooth.c"

/**
 * Mix sawtooth oscillator
 *
 * @param osc_state Mixer channel oscillator state
 * @param tick_buffer Mixer channel tick buffer
 * @param chunk_len Length of tick_buffer in samples
 */
void mix_sawtooth(int *osc_state, short *tick_buffer, int chunk_len) {
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
   * Buffer Constants
   */
  const int freq = (osc_phase_inc * 22050) >> 16;
  const int detune_freq = ((osc_detune_phase_inc * 22050) >> 16)
                          << osc_detune_m1;

  /*
   * Populate buffer
   */
  int cur_phase = osc_phase;
  int cur_detune_phase = osc_detune_phase;

  for (int i = 0; i < chunk_len; i += 1) {
    /*
     * Primary phasor
     */
    double amplitude = (double)sample_sawtooth(freq, cur_phase) / 0x100000;

    /*
     * Detune phasor
     */
    const int detune_phase = cur_detune_phase << osc_detune_m1;

    double detune_amplitude =
        (double)sample_sawtooth(detune_freq, detune_phase & 0xffff) / 0x100000;

    /*
     * Buzz phasors
     *
     * If buzz enabled, overlay additional saw waves, pitched down one octave
     */
    if (osc_buzz) {
      // Primary phasor
      amplitude += (double)sample_sawtooth(freq, cur_phase) / 0x100000 / 2;

      // Detune phasor
      detune_amplitude +=
          ((double)sample_sawtooth(detune_freq / 4, detune_phase / 4) /
           0x100000) *
          2;

      amplitude /= 2;
      detune_amplitude /= 2;
    }

    /*
     * Mix sample
     */
    amplitude *= 0x7000;
    detune_amplitude *= 0x3800;

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
