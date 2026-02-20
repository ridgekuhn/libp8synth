// #include "./mix_pulse.c"
// #include "./phasors/sample_square.c"

/**
 * Mix square oscillator
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in samples
 */
void mix_square(int *osc_state, short *chunk_buffer, int chunk_len) {
  // If osc_buzz, use mix_pulse instead
  if (osc_state[21]) {
    mix_pulse(osc_state, chunk_buffer, chunk_len, 0x8000);
    return;
  }

  /*
   * Oscillator state
   */
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
  const int sfx_step_pitch = osc_state[15];

  /*
   * Buffer constants
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
    double amplitude = (double)sample_square(freq, cur_phase) / 0x100000;

    /*
     * Detune phasor
     */
    double detune_amplitude =
        (double)sample_square(detune_freq << osc_detune_m1,
                              (cur_detune_phase << osc_detune_m1) & 0xffff) /
        0x100000;

    /*
     * Mix sample
     */
    amplitude *= 0x5ffc - ((0x2ffe / 64.0) * sfx_step_pitch);
    detune_amplitude *= 0x2ffc - ((0x17fe / 64.0) * sfx_step_pitch);

    // Write new sample
    const double s_prefader = amplitude + detune_amplitude;
    const double s = (s_prefader * osc_amplitude) / 3072;
    chunk_buffer[i] = (short)s;

    // Increment phase
    cur_phase = (cur_phase + osc_phase_inc) & 0xffff;
    cur_detune_phase = (cur_detune_phase + osc_detune_phase_inc) & 0x1ffff;
  }

  // Update oscillator state
  osc_state[1] = cur_phase;
  osc_state[3] = cur_detune_phase;
}
