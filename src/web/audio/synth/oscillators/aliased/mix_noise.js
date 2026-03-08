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
 * @param chunk_len Length of tick_buffer in bytes
 */
function mix_noise(osc_state, tick_buffer, chunk_len) {
	/*
	 * Oscillator state
	 */
	const osc_buzz = c[(osc_state + 84) >> 2] != 0;
	const osc_noiz = c[(osc_state + 88) >> 2];

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
