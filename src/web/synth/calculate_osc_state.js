/**
 * Calculate osc state
 */
function calculate_osc_state(ch_state, osc_state) {
	// 0x2020 = 0x2028 (+0x8) in p8 binary
	const sfx_ptr_addr = ch_state + 0x2020;
	const sfx_ptr = c[sfx_ptr_addr >> 2];

	if (sfx_ptr == 0) {
		disable_osc(osc_state);
		return;
	}

	const sfx_spd = c[(sfx_ptr + 8) >> 2];
	const sfx_spd_min = sfx_spd > 1 ? sfx_spd : 1;
	// 0x2024 = 0x2030 (+0xc) in p8 binary
	const sfx_tick_addr = ch_state + 0x2024;
	const sfx_tick = c[sfx_tick_addr >> 2];
	const sfx_step = Math.floor(sfx_tick / sfx_spd_min);

	if (sfx_step > 31) {
		disable_osc(osc_state);
		return;
	}

	const sfx_step_tick = sfx_tick - B(sfx_step, sfx_spd_min);
	const is_lo_spd = sfx_spd < 9;
	// sfx_ptr + 0x14 = sfx_ptr + 0x20 in p8 binary
	const sfx_step_addr = sfx_ptr + 0x14 + sfx_step * 20;
	// sfx_ptr + 0x14 = sfx_ptr + 0x20 in p8 binary
	const sfx_step_waveform_addr = sfx_ptr + 0x14 + sfx_step * 20 + 4;
	const sfx_step_waveform = c[sfx_step_waveform_addr >> 2];

	c[osc_state >> 2] = sfx_step_waveform;

	// sfx_ptr + 0x14 = sfx_ptr + 0x20 in p8 binary
	const sfx_step_vol = c[(sfx_ptr + 0x14 + sfx_step * 20 + 8) >> 2];
	const sfx_step_vol_256 = sfx_step_vol << 8;
	const osc_step_target_vol_addr = osc_state + 28;

	c[osc_step_target_vol_addr >> 2] = sfx_step_vol_256;

	const sfx_step_pitch = c[sfx_step_addr >> 2];
	const osc_step_target_pitch = sfx_step_pitch << 16;
	const osc_step_target_pitch_addr = osc_state + 32;

	c[osc_step_target_pitch_addr >> 2] = osc_step_target_pitch;
	c[(osc_state + 0x38) >> 2] = sfx_step_vol;
	c[(osc_state + 0x3c) >> 2] = sfx_step_pitch;
	c[(osc_state + 0x40) >> 2] = sfx_step_waveform;

	// sfx_ptr + 0x14 = sfx_ptr + 0x20 in p8 binary
	const sfx_effect_addr = sfx_ptr + 0x14 + sfx_step * 20 + 12;
	const sfx_effect = c[sfx_effect_addr >> 2];

	let new_target_pitch = 0;
	let new_target_vol = 0;

	switch (sfx_effect) {
		// Slide
		case 1: {
			let prev_base_step_pitch = 0;
			let prev_step_target_vol = 0;

			if (sfx_step > 0) {
				// Previous(?) oscillator state base step pitch, osc_state + 0x140 + 0x18
				// 0x2e84 = 0x2e90 in p8 binary
				prev_base_step_pitch = c[(ch_state + 0x2e84) >> 2];
				// Previous(?) oscillator state target volume, osc_state + 0x140 + 0x1c
				// 0x2e8c = 0x2e98 in p8 binary
				prev_step_target_vol = c[(ch_state + 0x2e8c) >> 2] << 8;
			} else {
				prev_base_step_pitch = 24;
				prev_step_target_vol = sfx_step_vol_256;
			}

			const step_ticks_remaining = sfx_spd_min - sfx_step_tick;

			new_target_pitch =
				(B(step_ticks_remaining << 16, prev_base_step_pitch) +
					B(osc_step_target_pitch, sfx_step_tick)) /
				sfx_spd_min;

			new_target_vol =
				(B(prev_step_target_vol, step_ticks_remaining) +
					B(sfx_step_vol_256, sfx_step_tick)) /
				sfx_spd_min;

			c[osc_step_target_pitch_addr >> 2] = new_target_pitch;
			c[osc_step_target_vol_addr >> 2] = new_target_vol;

			break;
		}
		// Fade in
		case 4: {
			new_target_pitch = osc_step_target_pitch;
			new_target_vol = B(sfx_step_vol_256, sfx_step_tick) / sfx_spd_min;

			c[osc_step_target_vol_addr >> 2] = new_target_vol;

			break;
		}
		// Fade out
		case 5: {
			new_target_pitch = osc_step_target_pitch;
			new_target_vol = Math.floor(
				B(sfx_step_vol_256, sfx_spd_min - sfx_step_tick) / sfx_spd_min,
			);

			c[osc_step_target_vol_addr >> 2] = new_target_vol;

			break;
		}
		// Arp fast
		case 6: {
			const step_offset =
				((sfx_tick / (is_lo_spd ? 2 : 4)) % 4) + (sfx_step & 0x1c);

			// sfx_ptr + 0x14 = sfx_ptr + 0x20 in p8 binary
			new_target_pitch = c[(sfx_ptr + 0x14 + step_offset * 20) >> 2] << 16;
			new_target_vol = sfx_step_vol_256;

			c[osc_step_target_pitch_addr >> 2] = new_target_pitch;

			break;
		}
		// Arp slow
		case 7: {
			const step_offset =
				((sfx_tick / (is_lo_spd ? 4 : 8)) % 4) + (sfx_step & 0x1c);

			// sfx_ptr + 0x14 = sfx_ptr + 0x20 in p8 binary
			new_target_pitch = c[(sfx_ptr + 0x14 + step_offset * 20) >> 2] << 16;
			new_target_vol = sfx_step_vol_256;

			c[osc_step_target_pitch_addr >> 2] = new_target_pitch;

			break;
		}
		// No effect, vibrato, drop
		default: {
			new_target_pitch = osc_step_target_pitch;
			new_target_vol = sfx_step_vol_256;
		}
	}

	c[(osc_state + 0x24) >> 2] = sfx_step_pitch;
	c[(osc_state + 0x28) >> 2] = new_target_vol;

	const osc_effect_addr = osc_state + 68;

	c[osc_effect_addr >> 2] = 0;

	const osc_filter_addr = osc_state + 72;

	c[osc_filter_addr >> 2] = 0;

	// sfx_ptr + 0x14 = sfx_ptr + 0x20 in p8 binary
	const sfx_step_is_meta_addr = sfx_ptr + 0x14 + sfx_step * 20 + 0x10;

	// 0x201c = 0x2020 (+0x4) in p8 binary
	const loaded_cart_ptr = c[(ch_state + 0x201c) >> 2];

	let meta_inst_idx = sfx_step_waveform < 7 ? sfx_step_waveform : 7;
	meta_inst_idx = meta_inst_idx > 0 ? meta_inst_idx : 0;

	// loaded_cart_ptr + 0x10 = loaded_cart_ptr + 0x20 in p8 binary
	const loop_start =
		c[(loaded_cart_ptr + 0x10 + meta_inst_idx * 680 + 12) >> 2];

	// If step does not use meta instrument
	if (!c[sfx_step_is_meta_addr >> 2]) {
		// osc_state + 0x140 + 0x70
		// 0x2ed4 = 0x2ee0 in p8 binary
		c[(ch_state + 0x2ed4) >> 2] = 0;
	} else {
		// if (sfx_step_tick == 0)
		if (!sfx_step_tick) {
			let meta_inst_step_len = 0;
			let meta_inst_tick_len = 0;

			// @todo needs better name
			let is_meta = false;

			// If wavetable frame
			if (loop_start & 128) {
				is_meta = true;
			} else {
				// loaded_cart_ptr + 0x10 = loaded_cart_ptr + 0x20 in p8 binary
				const loop_end =
					c[(loaded_cart_ptr + 0x10 + meta_inst_idx * 680 + 0x10) >> 2];

				if (loop_end > loop_start) {
					meta_inst_tick_len = 0x1000000;
				} else {
					meta_inst_step_len =
						loop_start > 0 && loop_end == 0 ? loop_start : 32;
					is_meta = true;
				}
			}

			if (is_meta) {
				let meta_inst_spd =
					// loaded_cart_ptr + 0x10 = loaded_cart_ptr + 0x20 in p8 binary
					c[(loaded_cart_ptr + 0x10 + meta_inst_idx * 680 + 8) >> 2];
				meta_inst_spd = meta_inst_spd > 1 ? meta_inst_spd : 1;

				meta_inst_tick_len = B(meta_inst_spd, meta_inst_step_len);
			}

			const sfx_drop = sfx_effect == 3 ? 1 : 0;

			if (
				(sfx_effect != 1 &&
					// 0x2e84 = 0x2e90 (+0xc) in p8 binary, @todo meta instrument pitch?
					(sfx_step == 0 || sfx_step_pitch != c[(ch_state + 0x2e84) >> 2])) ||
				// 0x2ed4 = 0x2ee0 (+0xc) in p8 binary
				c[(ch_state + 0x2ed4) >> 2] >= meta_inst_tick_len
			) {
				// @todo swap xor operator
				if (!(sfx_effect == 4 ? 1 : 0 | (sfx_drop ^ 1))) {
					is_meta = false;
				}
			} else if (!(sfx_drop | (sfx_effect == 4 ? 1 : 0))) {
				is_meta = false;
			}

			// 0x2e88 = 0x2e94 in p8 binary
			if (is_meta || sfx_step_waveform != c[(ch_state + 0x2e88) >> 2]) {
				// 0x2ed4 = 0x2ee0 (+0xc) in p8 binary
				c[(ch_state + 0x2ed4) >> 2] = 0;
			}
		}

		// apply_meta_instrument()
		apply_meta_instrument(ch_state, sfx_step_addr, osc_state);

		new_target_pitch = c[osc_step_target_pitch_addr >> 2];
	}

	// 0x2030 = 0x203c (+0xc) in p8 binary
	const target_ch = c[(ch_state + 0x2030) >> 2];
	const hi_filter_mask = 1 << (target_ch + 4);
	const lo_filter_mask = 1 << target_ch;

	if (hi_filter_mask & c[AUDIO_CLOCK_MASK]) {
		new_target_pitch = new_target_pitch - 0xc0000;
		c[osc_step_target_pitch_addr >> 2] = new_target_pitch;
	}

	const new_target_pitch_partial = new_target_pitch & 0xffff;

	const new_target_pitch_64 = new_target_pitch >> 16;

	const new_target_pitch_64_clamped =
		new_target_pitch > -1
			? new_target_pitch_64
			: 12 - (-new_target_pitch_64 % 12);

	const new_scale_degree = new_target_pitch_64_clamped % 12;

	const freq1 = B(
		c[(note_dx + (new_scale_degree << 2)) >> 2],
		0x10000 - new_target_pitch_partial,
	);

	const freq2 = B(
		c[(note_dx + ((new_scale_degree + 1) << 2)) >> 2],
		new_target_pitch_partial,
	);

	/*
	 * Phase increment (dt)
	 */
	let dt = (freq1 + freq2) / 22050;
	let octave_target = Math.floor((new_target_pitch_64 + 48) / 12) - 4;

	if (new_target_pitch < 0x240000) {
		while (octave_target < 3) {
			dt /= 2;
			octave_target += 1;
		}
	} else {
		while (octave_target > 3) {
			dt = dt << 1;
			octave_target -= 1;
		}
	}

	dt = dt < 0x8000 ? dt : 0x8000;
	dt = dt > 0x8 ? dt : 0x8;

	// c[(osc_state + 8) >> 2] = dt;

	/*
	 * Vibrato
	 */
	const osc_effect = c[osc_effect_addr >> 2];
	const vibratos = [0, 1, 2, 1, 0, -1, -2, -1];

	let vibrato = 0;

	if (sfx_effect == 2 || osc_effect == 2) {
		vibrato = vibratos[(c[sfx_tick_addr >> 2] >>> 1) & 7];

		if (sfx_effect == 2 && osc_effect == 2) {
			vibrato *= 2;
		}

		dt = (dt * (128 + vibrato)) >>> 7;
	}

	c[(osc_state + 8) >> 2] = dt;

	/*
	 * Drop
	 */
	let drop_dt = dt;

	if (
		sfx_effect == 3 &&
		!(c[sfx_step_is_meta_addr >> 2] && (loop_start & 128) == 0)
	) {
		drop_dt = B(dt, sfx_spd_min - sfx_step_tick) / sfx_spd_min;
		c[(osc_state + 8) >> 2] = drop_dt;
	}

	if (osc_effect == 3) {
		drop_dt = B(c[(osc_state + 0x4c) >> 2], drop_dt) / 256;
		c[(osc_state + 8) >> 2] = drop_dt;
	}

	/*
	 * Apply MUSIC VOLUME
	 */
	// 0x2d1c = 0x2d28 in p8 binary
	if (c[(ch_state + 0x2d1c) >> 2]) {
		const new_target_vol =
			B(c[fade_vol] >> 8, c[(osc_state + 0x1c) >> 2]) / 256;

		c[(osc_state + 0x1c) >> 2] = B(new_target_vol, c[MUSIC_VOLUME]) / 256;
	}

	/*
	 * Filters
	 */
	const sfx_filter_byte = c[sfx_ptr >> 2];
	const osc_filter_byte = c[osc_filter_addr >> 2];

	const sfx_detune = (sfx_filter_byte >> 3) % 3;
	const osc_detune = (osc_filter_byte >> 3) % 3;
	const detune = sfx_detune > osc_detune ? sfx_detune : osc_detune;

	c[(osc_state + 0x50) >> 2] = detune;

	const sfx_buzz = (sfx_filter_byte >>> 2) & 1;
	const osc_buzz = (osc_filter_byte >>> 2) & 1;
	const buzz = sfx_buzz > osc_buzz ? sfx_buzz : osc_buzz;

	c[(osc_state + 0x54) >> 2] = buzz;

	const sfx_noiz = (sfx_filter_byte >>> 1) & 1;
	const osc_noiz = (osc_filter_byte >>> 1) & 1;
	const noiz = sfx_noiz >>> 0 > osc_noiz >>> 0 ? sfx_noiz : osc_noiz;

	c[(osc_state + 0x58) >> 2] = noiz;

	/*
	 * Detune
	 */
	const waveform = c[osc_state >> 2];
	let detune_dt = c[(osc_state + 8) >> 2];
	let detune_amount = detune == 0 ? 256 : 255;

	if (waveform != 6 || noiz == 0) {
		if (waveform == 0) {
			if (detune == 1) {
				detune_amount = 193;
			} else if (detune == 2) {
				detune_amount = 384;
			}
		} else if (waveform < 6 && detune > 0) {
			// lower target volume
			c[(osc_state + 0x1c) >> 2] = (c[(osc_state + 0x1c) >> 2] * 5) / 4;
		} else if (waveform == 7) {
			if (detune == 0) {
				detune_amount = 254;
			} else if (detune == 1) {
				detune_amount = 250;
			} else if (detune == 2) {
				detune_amount = 508;
			}
		}
		// waveform == 6 && noiz != 0
	} else {
		// if total dampen == 2 or 3 (0x5f43 hi+lo)
		// 0x2edc = 0x2ee8 in p8 binary
		if (c[(ch_state + 0x2edc) >> 2] > 11) {
			// set noiz to 2
			c[(osc_state + 88) >> 2] = 2;

			// set dampen to 0
			// 0x2edc = 0x2ee8 in p8 binary
			c[(ch_state + 0x2edc) >> 2] = 0;
		}
	}

	c[(osc_state + 0x10) >> 2] = B(detune_dt, detune_amount) / 256;

	/*
	 * Reverb
	 */
	const sfx_reverb = sfx_ptr ? ((c[sfx_ptr >> 2] >> 3) / 3) % 3 : 0;
	// 0x2d6c = 0x2d78 in p8 binary
	let osc_reverb = ((c[(ch_state + 0x2d6c) >> 2] >> 3) / 3) % 3;
	let reverb = sfx_reverb > osc_reverb ? sfx_reverb : osc_reverb;

	const global_reverb = c[GLOBAL_REVERB];

	if (global_reverb & hi_filter_mask) {
		reverb = reverb > 1 ? reverb : 1;
	}

	if (global_reverb & lo_filter_mask) {
		reverb = reverb > 2 ? reverb : 2;
	}

	c[(osc_state + 92) >> 2] = reverb;
}
