/**
 * Stop all sounds
 */
function codo_stop_all_sounds() {
	codo_lock_audio_plat();

	for (let i = 0; i < 16; i += 1) {
		// 0x34f0 = 0x3700 in p8 binary
		const ch_state = ms0 + i * 0x34f0;

		init_ch_state(ch_state);
	}

	codo_unlock_audio_plat();
}
