/**
 * codo_mixer_callback_0()
 */
function codo_mixer_callback_0(_userdata, chunk_buffer, chunk_len) {
	/*
	 * Init mixer buffer
	 */
	c[last_callback_len] = chunk_len;

	// ka(28) checks if document.hidden in asm.js
	// @TODO asm.js only
	if (ka(28)) {
		memset(chunk_buffer, 0, chunk_len);
		return;
	}

	// @TODO asm.js only?
	c[codo_state_608] += 1;
	if (!(c[codo_state_608] < 3 || c[codo_state_612] != 0)) {
		memset(chunk_buffer, 0, chunk_len);
		return;
	}

	c[inside_codo_mixer_callback] = 1;

	// @TODO no such global in p8 binary, possibly due to compiler optimization?
	const has_timer = c[0x2221] != -1;

	const time = codo_get_time();

	if (has_timer) {
		const expired = time - c[0x2221] > 500;

		c[0x2221] = codo_get_time();

		if (expired) {
			if (ch_chunk_len <= 0) {
				return;
			}

			memset(chunk_buffer, 0, ch_chunk_len << 1);

			return;
		}
	} else {
		c[0x2221] = time;
	}

	c[advanced_pattern] = 0;

	/*
	 * Mix channels
	 */
	const program_channels = c[codo_state_664];

	let ch_chunk_len =
		chunk_len / (program_channels == 0 ? 1 : program_channels << 1);
	ch_chunk_len = c[codo_state_660] == 44100 ? ch_chunk_len / 2 : ch_chunk_len;

	for (let i = 0; i < 16; i += 1) {
		// 0x34f0 = 0x3700 in p8 binary
		mix_channel(ms0 + 0x34f0 * i, ch_chunk_len);
	}

	// @see mix_sfx_tick()
	if (c[advanced_pattern]) {
		c[codo_state_636] = c[codo_state_636] + 1;
	}

	// @TODO this name probably isn't accurate
	const sound_initialized = c[SOUND_INITIALIZED] != 0;

	// @TODO Voxatron only?
	if (sound_initialized) {
		for (let i = 0; i < 3; i += 1) {
			// if ch i has sfx pointer, zero out ch i + 4
			// 0x34f0 = 0x3700 in p8 binary
			// 0x2020 = 0x2028 in p8 binary
			if (c[(ms0 + 0x34f0 * i + 0x2020) >> 2]) {
				// 0x34f0 = 0x3700 in p8 binary
				memset(ms0 + 0x34f0 * (i + 4), 0, chunk_len);
			}
		}
	}

	const has_chunks = ch_chunk_len > 0;
	const max_channels = sound_initialized ? 8 : 16;

	if (has_chunks) {
		// sum channels in pairs until mixed down
		// (ie, 0+1, 2+3, 4+5, 6+7, then 0+3, 4+5, then 0+4)
		for (let i = 2; i <= max_channels; i *= 2) {
			let x = i / 2;

			for (let j = 0; j < max_channels; j += i) {
				// 0x34f0 = 0x3700 in p8 binary
				const base_ch_offset = ms0 + j * 0x34f0;
				// 0x34f0 = 0x3700 in p8 binary
				const unmixed_ch_offset = ms0 + (j + x) * 0x34f0;

				for (let sample = 0; sample < ch_chunk_len; sample += 1) {
					const sample_offset = sample << 1;

					// 0x34f0 = 0x3700 in p8 binary
					const sample_addr = base_ch_offset + sample_offset;

					// 0x34f0 = 0x3700 in p8 binary
					const summed_sample =
						b[sample_addr >> 1] + b[(unmixed_ch_offset + sample_offset) >> 1];

					let normalized_sample = 0;

					if (summed_sample <= 0x5fff)
						if (summed_sample < -0x5fff) {
							normalized_sample = (summed_sample + 0x6000) / 5 - 0x6000;
						} else {
							normalized_sample = summed_sample;
						}
					else {
						normalized_sample = (summed_sample - 0x6000) / 5 + 0x6000;
					}

					b[sample_addr >> 1] = normalized_sample;
				}
			}
		}
	}

	/*
	 * Apply volume
	 */
	if (c[SOUND_VOLUME] != 256 && has_chunks) {
		for (let i = 0; i < ch_chunk_len; i += 1) {
			const sample_addr = ms0 + (i << 1);

			b[sample_addr >> 1] = B(c[SOUND_VOLUME], b[sample_addr >> 1]) >>> 8;
		}
	}

	// voxatron only
	// if (c[MUSIC_PLAYING] && c[ps0] && c[fade_vol]) {
	//   // 0x878F0 = 0xa74d80 (music volume) in p8 binary
	//   const new_vol = B(c[MUSIC_VOLUME], c[fade_vol]) >> 16;

	//   if (codo_current_music) {
	//   	codo_mix_xm_chunk(ps0, xmbuf, ch_chunk_len);
	//   }

	//   if (has_chunks) {
	//     for (let i = 0; i < ch_chunk_len; i += 1) {
	//       const sample_addr = xmbuf + (i << 1);

	//       b[sample_addr >> 1] = B(new_vol, b[sample_addr >> 1]) >>> 8;
	//     }

	//     for (let i = 0; i < ch_chunk_len; i += 1) {
	//       const mixer_sample_addr = ms0 + (i << 1);
	//       const summed_samples =
	//         b[(xmbuf + (i << 1)) >> 1] + b[mixer_sample_addr >> 1];

	//       let new_sample = 0;

	//       if (summed_samples <= 0x5fff) {
	//         if (summed_samples < -0x5fff) {
	//           new_sample = (summed_samples + 0x6000) / 5 - 0x6000;
	//         } else {
	//           new_sample = summed_samples;
	//         }
	//       } else {
	//         new_sample = (summed_samples - 0x6000) / 5 + 0x6000;
	//       }

	//       b[mixer_sample_addr >> 1] = new_sample;
	//     }
	//   }
	// }

	if (has_chunks) {
		if (c[codo_state_660] == 22050 && c[codo_state_664] == 1) {
			for (let i = 0; i < ch_chunk_len; i += 1) {
				b[(chunk_buffer + (i << 1)) >> 1] = b[(ms0 + (i << 1)) >> 1];
			}
		} else if (c[codo_state_660] == 44100 && c[codo_state_664] == 2) {
			for (let i = 0; i < ch_chunk_len << 2; i += 1) {
				b[(chunk_buffer + (i << 1)) >> 1] = b[(ms0 + ((i >>> 2) << 1)) >> 1];
			}
		}
	}

	if (c[codo_post_mix_func]) {
		// Ta[c[codo_post_mix_func] & 15] points to mix_serial_sound_buffer(),
		// set in codo_main_init()
		// @TODO in p8 binary, we should call (*codo_post_mix_func)()
		Ta[c[codo_post_mix_func] & 15](chunk_buffer, chunk_len / 2);
	}

	// only used for runtime recording?
	if (
		c[codo_state_660] == 22050 &&
		c[codo_state_664] == 1 &&
		c[codo_state_632] != 0
	) {
		const max_samples = chunk_len / (c[codo_state_664] * 2);

		if (max_samples > 0) {
			for (let i = 0; i < max_samples; i += 1) {
				codo_fwrite_int16(
					b[(chunk_buffer + (i << 1)) >> 1],
					c[codo_audio_buffer]
				);
			}
		}
	}

	// 0xb5553 = inside_codo_mixer_callback in p8 binary
	c[inside_codo_mixer_callback] = 0;
}
