/**
 * Update music fade
 */
function codo_update_music_fading() {
	codo_lock_audio_plat();

	let new_fade_vol = c[fade_vol];

	if (c[fade_vol] != c[fade1] && c[fade_len] != 0) {
		const time = codo_get_time();
		const fade_progress = time - c[fade_start_t];

		new_fade_vol = c[fade1];

		if (c[fade_len] > fade_progress) {
			new_fade_vol =
				(B(c[fade1], fade_progress) +
					B(c[fade0], c[fade_len] - fade_progress)) /
				c[fade_len];

			new_fade_vol = new_fade_vol < 0x10000 ? new_fade_vol : 0x10000;
			new_fade_vol = new_fade_vol > 0 ? new_fade_vol : 0;

			c[fade_vol] = new_fade_vol;

			codo_unlock_audio_plat();

			return;
		}
	}

	if (c[fade1] == 0) {
		codo_lock_audio_plat();

		c[MUSIC_PLAYING] = 0;

		codo_stop_pico8_song(0);
		codo_unlock_audio_plat();
	}

	codo_unlock_audio_plat();

	return;
}
