/**
 * Update music fade
 */
function codo_update_music_fading() {
	codo_lock_audio_plat();

	if (c[fade_vol] == c[fade1]) {
		if (c[fade_vol] == 0) {
			c[MUSIC_PLAYING] = 0;
			codo_stop_pico8_song(0);
		}

		codo_unlock_audio_plat();

		return;
	}

	if (c[fade_len] == 0) {
		c[fade_vol] = c[fade1];

		if (c[fade1] == 0) {
			c[MUSIC_PLAYING] = 0;
			codo_stop_pico8_song(0);
		}

		codo_unlock_audio_plat();

		return;
	}

	const fade_progress = codo_get_time() - c[fade_start_t];

	if (c[fade_len] > fade_progress) {
		let new_fade_vol =
			(B(c[fade1], fade_progress) + B(c[fade_0], c[fade_len] - fade_progress)) /
			c[fade_len];
		new_fade_vol = new_fade_vol < 65536 ? new_fade_vol : 65536;
		new_fade_vol = new_fade_vol > 0 ? new_fade_vol : 0;

		c[fade_vol] = new_fade_vol;

		codo_unlock_audio_plat();

		return;
	}

	c[fade_vol] = c[fade1];

	if (c[fade1] == 0) {
		c[MUSIC_PLAYING] = 0;
		codo_stop_pico8_song(0);
	}

	codo_unlock_audio_plat();
}
