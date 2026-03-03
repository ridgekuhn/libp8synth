/**
 * Mix noise oscillator
 *
 * Select and call appropriate noise mixing function
 *
 * @see mix_brown_noise()
 * @see mix_pink_noise()
 * @see mix_white_noise()
 *
 * @param osc_state Mixer channel oscillator state
 * @param tick_buffer Mixer channel tick buffer
 * @param chunk_len Length of tick_buffer in samples
 */
void mix_noise(int *osc_state, short *tick_buffer, int chunk_len) {
  /*
   * Oscillator state
   */
  const _Bool osc_buzz = osc_state[21] != 0;
  const int osc_noiz = osc_state[22];

  /*
   * Populate buffer
   */
  if (osc_noiz) {
    mix_white_noise(osc_state, tick_buffer, chunk_len);
    return;
  }

  if (osc_buzz) {
    mix_brown_noise(osc_state, tick_buffer, chunk_len);
    return;
  }

  mix_pink_noise(osc_state, tick_buffer, chunk_len);
}
