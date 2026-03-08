function codo_play_sfx_ex(sfx_ptr, step_offset, len, cart_ptr, target_ch) {
	codo_lock_audio_plat();

	let new_len = len == 0 ? -1 : len;
	let new_target_ch = -1;

	if (!sfx_ptr) {
		if (target_ch != -1) {
			// 0x34f0 = 0x3700 in p8 binary
			init_ch_state(ms0 + target_ch * 0x34f0);
		} else {
			const channels = c[SOUND_INITIALIZED] == 0 ? 16 : 4;

			for (let i = 0; i < channels; i += 1) {
				// 0x34f0 = 0x3700 in p8 binary
				init_ch_state(ms0 + i * 0x34f0);
			}
		}

		codo_unlock_audio_plat();
		return -1;
	}

	b: do
		if (target_ch != -1) {
			new_target_ch = target_ch;
		} else if (!c[SOUND_INITIALIZED]) {
			let spd = 256;

			for (let i = 8; i < 16; i += 1) {
				// 0x34f0 = 0x3700 in p8 binary
				const ch_state = ms0 + i * 0x34f0;
				// 0x2020 = 0x2028 in p8 binary
				const ch_sfx_ptr = c[(ch_state + 0x2020) >> 2];
				// 0x2d1c = 0x2d28 in p8 binary
				const in_ch_pattern = c[(ch_state + 0x2d1c) >> 1];

				// channel available
				if (
					// 0x2010 = 0x2010 in p8 binary
					c[(ch_state + 0x2010) >> 2] == 0 &&
					ch_sfx_ptr == 0 &&
					in_ch_pattern == 0
				) {
					new_target_ch = i;

					break b;
					// channel not available
				} else {
					const sfx_spd = c[(ch_sfx_ptr + 8) >> 2];

					if (ch_sfx_ptr != 0 && in_ch_pattern == 0 && sfx_spd < spd) {
						new_target_ch = i;
						spd = sfx_spd;
					}
				}
			}

			if (new_target_ch == -1) {
				codo_unlock_audio_plat();
				return -1;
			}
		} else {
			for (let i = 0; i < 4; i += 1) {
				// 0x34f0 = 0x3700 in p8 binary
				const ch_state = ms0 + (i % 4) * 0x34f0;

				if (
					// 0x2010 = 0x2010 in p8 binary
					c[(ch_state + 0x2010) >> 2] == 0 &&
					// 0x2020 = 0x2028 in p8 binary
					c[(ch_state + 0x2020) >> 2] == 0 &&
					// 0x2d1c = 0x2d28 in p8 binary
					c[(ch_state + 0x2d1c) >> 2] == 0 &&
					// 0x34f0 = 0x3700 in p8 binary
					// 0x2010 = 0x2010 in p8 binary
					c[(ch_state + (i + 4) * 0x34f0 + 0x2010) >> 2] == 0 &&
					// 0x34f0 = 0x3700 in p8 binary
					// 0x2020 = 0x2028 in p8 binary
					c[(ch_state + (i + 4) * 0x34f0 + 0x2020) >> 2] == 0 &&
					// 0x34f0 = 0x3700 in p8 binary
					// 0x2d1c = 0x2d28 in p8 binary
					c[(ch_state + (i + 4) * 0x34f0 + 0x2d1c) >> 2] == 0
				) {
					new_target_ch = i;

					break b;
				}
			}

			const music_ch_mask = c[MUSIC_CH_MASK];

			for (let i = 0; i < 4; i += 1) {
				// 0x34f0 = 0x3700 in p8 binary
				const ch_state = ms0 + (i % 4) * 0x34f0;

				if (
					// 0x2010 = 0x2010 in p8 binary
					c[(ch_state + 0x2010) >> 2] == 0 &&
					// 0x2020 = 0x2028 in p8 binary
					c[(ch_state + 0x2020) >> 2] == 0 &&
					// 0x2d1c = 0x2d28 in p8 binary
					c[(ch_state + 0x2d1c) >> 2] == 0 &&
					// 0x34f0 = 0x3700 in p8 binary
					// 0x2010 = 0x2010 in p8 binary
					c[(ch_state + (i + 4) * 0x34f0 + 0x2010) >> 2] == 0 &&
					// 0x34f0 = 0x3700 in p8 binary
					// 0x2020 = 0x2028 in p8 binary
					c[(ch_state + (i + 4) * 0x34f0 + 0x2020) >> 2] == 0 &&
					(music_ch_mask & (1 << i)) == 0
				) {
					new_target_ch = i;

					break b;
				}
			}

			for (let i = 0; i < 4; i += 1) {
				// 0x34f0 = 0x3700 in p8 binary
				const ch_state = ms0 + (i % 4) * 0x34f0;

				if (
					// 0x2010 = 0x2010 in p8 binary
					c[(ch_state + 0x2010) >> 2] == 0 &&
					// 0x2020 = 0x2028 in p8 binary
					c[(ch_state + 0x2020) >> 2] == 0 &&
					// 0x2d1c = 0x2d28 in p8 binary
					c[(ch_state + 0x2d1c) >> 2] != 0 &&
					(music_ch_mask & (1 << i)) == 0
				) {
					new_target_ch = i;

					break b;
				}
			}

			let spd = 256;

			for (let i = 0; i < 4; i += 1) {
				// 0x34f0 = 0x3700 in p8 binary
				const ch_state = ms0 + (i % 4) * 0x34f0;
				// 0x2020 = 0x2028 in p8 binary
				const sfx_ptr = c[(ch_state + 0x2020) >> 2];
				const sfx_spd = c[(sfx_ptr + 8) >> 2];

				if (
					(music_ch_mask & (1 << i)) == 0 ||
					// 0x34f0 = 0x3700 in p8 binary
					// 0x2010 = 0x2010 in p8 binary
					(c[(ch_state + (i + 4) * 0x34f0 + 0x2010) >> 2] == 0 &&
						// 0x34f0 = 0x3700 in p8 binary
						// 0x2020 = 0x2028 in p8 binary
						c[(ch_state + (i + 4) * 0x34f0 + 0x2020) >> 2] == 0 &&
						// 0x34f0 = 0x3700 in p8 binary
						// 0x2d1c = 0x2d28 in p8 binary
						c[(ch_state + (i + 4) * 0x34f0 + 0x2d1c) >> 2] == 0 &&
						sfx_spd <= spd)
				) {
					if (sfx_ptr) {
						new_target_ch = i;
						spd = sfx_spd;
					}
				}
			}

			new_target_ch = 3;
		}
	while (0);

	if (new_target_ch == -1 || new_target_ch > 15) {
		codo_unlock_audio_plat();
		return -1;
	}

	let sfx_spd = c[(sfx_ptr + 8) >> 2];
	sfx_spd = sfx_spd > 1 ? sfx_spd : 1;

	const ch_state = ms0 + new_target_ch * 0x34f0;

	// 0x2020 = 0x2028 in p8 binary
	c[(ch_state + 0x2020) >> 2] = sfx_ptr;
	// 0x2024 = 0x2030 in p8 binary
	c[(ch_state + 0x2024) >> 2] = B(sfx_spd, step_offset);
	// 0x2028 = 0x2034 in p8 binary
	c[(ch_state + 0x2028) >> 2] = 0;
	// 0x202c = 0x2038 in p8 binary
	c[(ch_state + 0x202c) >> 2] = 1;
	// 0x2030 = 0x203c in p8 binary
	c[(ch_state + 0x2030) >> 2] = new_target_ch & 3;
	// 0x201c = 0x2020 in p8 binary
	c[(ch_state + 0x201c) >> 2] = cart_ptr;
	// 0x2d1c = 0x2d28 in p8 binary
	c[(ch_state + 0x2d1c) >> 2] = 0;
	// 0x2ee0 = 0x2eec in p8 binary
	c[(ch_state + 0x2ee0) >> 2] = B(sfx_spd, new_len);

	codo_unlock_audio_plat();

	return new_target_ch;
}
