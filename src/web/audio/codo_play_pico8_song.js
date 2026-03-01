/**
 * Play Pico-8 song
 */
function codo_play_pico8_song(cart_ptr, pat_idx, new_fade_len) {
	codo_lock_audio_plat();
	init_cart_audio(cart_ptr);

	if (cart_ptr == 0 || pat_idx > 63) {
		codo_unlock_audio_plat();
		return;
	}

	// cart_ptr + 0xaa10 = cart_ptr + 0xaa20 in p8 binary
	const pattern_addr = cart_ptr + 0xaa10 + (pat_idx << 4);
	const pat_ticks_len = get_pattern_ticks_length(cart_ptr, pattern_addr);

	if (pat_ticks_len == 0) {
		codo_unlock_audio_plat();
		return;
	}

	c[codo_state_636] = 0;
	c[codo_state_640] = pat_idx;

	for (let i = 0; i < 4; i += 1) {
		const sfx_idx = c[(pattern_addr + i * 4) >> 2];

		let sfx_idx_clamped = sfx_idx < 63 ? sfx_idx : 63;
		sfx_idx_clamped = sfx_idx_clamped > 0 ? sfx_idx_clamped : 0;

		// codo_play_sfx_ex
		const ch_idx = codo_play_sfx_ex(
			// cart_ptr + 0x10 = cart_ptr + 0x20 in p8 binary
			cart_ptr + 0x10 + sfx_idx_clamped * 680,
			0,
			32,
			cart_ptr,
			i + 4,
		);

		if (ch_idx != -1) {
			// 0x34f0 = 0x3700 in p8 binary
			const ch_state = ms0 + ch_idx * 0x34f0;

			if (sfx_idx > 63) {
				// 0x2020 = 0x2028 in p8 binary
				c[(ch_state + 0x2020) >> 2] = 0;
			}

			// 0x2d1c = 0x2d28 in p8 binary
			c[(ch_state + 0x2d1c) >> 2] = 1;
			// 0x201c = 0x2020 in p8 binary
			c[(ch_state + 0x201c) >> 2] = cart_ptr;
			// 0x2d20 = 0x2d2c in p8 binary
			c[(ch_state + 0x2d20) >> 2] = pat_idx;
			// 0x2030 = 0x203c in p8 binary
			c[(ch_state + 0x2030) >> 2] = i;
			// 0x2ee0 = 0x2eec in p8 binary
			c[(ch_state + 0x2ee0) >> 2] = pat_ticks_len;
			// 0x2ee4 = 0x2ef0 in p8 binary
			c[(ch_state + 0x2ee4) >> 2] = 0;
		}
	}

	c[fade_len] = new_fade_len;
	c[fade_start_t] = codo_get_time();
	c[fade_0] = 0;
	c[fade_1] = 0x10000;

	codo_update_music_fading();

	codo_unlock_audio_plat();
}
