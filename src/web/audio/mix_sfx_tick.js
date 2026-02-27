/**
 * Mix sfx tick
 */
function mix_sfx_tick(ch_state, tick_buffer) {
	// @TODO asm.js only
	const e = Ia;
	Ia = Ia + 352;

	/*
	 * Mix oscillator tick
	 */
	// memset()
	memset(tick_buffer, 0, 366);

	// 0x2020 = 0x2028 in p8 binary
	const sfx_ptr_addr = ch_state + 0x2020;
	const sfx_ptr = c[sfx_ptr_addr >> 2];
	const has_sfx_ptr = sfx_ptr != 0;

	// 0x2024 = 0x2030 in p8 binary
	const cur_sfx_tick_addr = ch_state + 0x2024;
	const cur_sfx_tick = c[cur_sfx_tick_addr >> 2];

	let cur_pat_tick = 0;
	let cur_step = 0;
	let spd = 1;

	if (has_sfx_ptr) {
		spd = c[(sfx_ptr + 8) >> 2];
		spd = spd > 1 ? spd : 1;

		// 0x2d24 = 0x2d30 in p8 binary
		const osc_state_addr = ch_state + 0x2d24;

		memcpy(e, osc_state_addr, 0x160);

		if (!(cur_sfx_tick % spd)) {
			// step waveform
			// 0x2e88 = 0x2e94, 0x2d64 = 0x2d70 in p8 binary
			c[(ch_state + 0x2e88) >> 2] = c[(ch_state + 0x2d64) >> 2];
			// step pitch
			// 0x2e84 = 0x2e90, 0x2d60 = 0x2d6c in p8 binary
			c[(ch_state + 0x2e84) >> 2] = c[(ch_state + 0x2d60) >> 2];
			// step vol
			// 0x2e8c = 0x2e98, 0x2d5c = 0x2d68 in p8 binary
			c[(ch_state + 0x2e8c) >> 2] = c[(ch_state + 0x2d5c) >> 2];
		}

		// calculate_osc_state_addr()
		calculate_osc_state(ch_state, osc_state_addr);

		// mix_osc_tick()
		mix_osc_tick(osc_state_addr, tick_buffer, 183, ch_state);

		for (let i = ramp_buf; i < ramp_buf + 0xc8; i += 4) {
			c[i >> 2] = 0;
		}

		// mix_osc_tick()
		mix_osc_tick(e, ramp_buf, 64, ch_state);

		for (let i = 0; i < 64; i += 1) {
			const sample_addr = tick_buffer + (i << 1);
			const osc_sample = b[sample_addr >> 1];
			const ramp_sample = b[(ramp_buf + (i << 1)) >> 1];
			const new_ramp_sample = (B(64 - i, ramp_sample) + B(i, osc_sample)) / 64;

			b[sample_addr >> 1] = new_ramp_sample;
		}

		// 0x2ed4 = 0x2ee0 in p8 binary
		cur_pat_tick = c[(ch_state + 0x2ed4) >> 2];
		cur_step = Math.floor((cur_sfx_tick + 1) / spd);
	} else {
		// @TODO didn't we already do this at the top?
		// memset()
		memset(tick_buffer, 0, 366);

		// 0x2d24 = 0x2d30 in p8 binary
		const osc_state_addr = ch_state + 0x2d24;

		// 0x2d40 = 0x2d4c in p8 binary
		const osc_target_vol_addr = ch_state + 0x2d40;

		// 0x2d2c = 0x2d38 in p8 binary
		// if phase increment or target volume != 0
		if (c[(ch_state + 0x2d2c) >> 2] ? c[osc_target_vol_addr >> 2] : 0) {
			// mix_osc_tick()
			mix_osc_tick(osc_state_addr, tick_buffer, 64, ch_state);

			for (let i = 0; i < 64; i += 1) {
				const sample_addr = tick_buffer + (i << 1);
				const old_sample = b[sample_addr >> 1];
				const new_sample = B(64 - i, old_sample) / 64;

				b[sample_addr >> 1] = new_sample;
			}
		}

		c[osc_target_vol_addr >> 2] = 0;

		// current pattern tick
		// 0x2ed4 = 0x2ee0 in p8 binary
		c[(ch_state + 0x2ed4) >> 2] = 0;

		cur_step = cur_sfx_tick + 1;
	}

	/*
	 * Update mixer channel state
	 */
	// increment pat ticks remaining
	// 0x2ee0 = 0x2eec in p8 binary
	const pat_ticks_remaining_addr = ch_state + 0x2ee0;
	const pat_ticks_remaining = c[pat_ticks_remaining_addr >> 2] - 1;
	c[pat_ticks_remaining_addr >> 2] = pat_ticks_remaining;

	// increment cur pattern tick
	// 0x2ed4 = 0x2ee0 in p8 binary
	c[(ch_state + 0x2ed4) >> 2] = cur_pat_tick + 1;

	// increment cur step tick
	// 0x2028 = 0x2034 in p8 binary
	const cur_step_tick_addr = ch_state + 0x2028;
	c[cur_step_tick_addr >> 2] = c[cur_step_tick_addr >> 2] + 1;

	// increment cur sfx tick
	const loop_start = c[(sfx_ptr + 0xc) >> 2];
	const loop_end = c[(sfx_ptr + 0x10) >> 2];

	let next_sfx_tick = c[cur_sfx_tick_addr >> 2] + 1;

	if (
		has_sfx_ptr &&
		// step spd != 0
		// 0x202c = 0x2038 in p8 binary
		c[(ch_state + 0x202c) >> 2] != 0 &&
		// not wavetable instrument
		(loop_start & 128) == 0 &&
		loop_start < loop_end &&
		// sfx has no ticks remaining
		next_sfx_tick >= B(loop_end, spd)
	) {
		next_sfx_tick = B(loop_start, spd);
	}

	c[cur_sfx_tick_addr >> 2] = next_sfx_tick;

	// update sfx ptr
	// 0x2d1c = 0x2d28 in p8 binary
	if (has_sfx_ptr && c[(ch_state + 0x2d1c) >> 2] == 0) {
		const is_wavetable = (loop_start & 128) != 0;

		if (!pat_ticks_remaining) {
			c[sfx_ptr_addr >> 2] = 0;
		} else if (loop_end <= loop_start || is_wavetable) {
			let sfx_len = 0;

			if (!is_wavetable) {
				sfx_len = loop_start > 0 && loop_end == 0 ? loop_start : 32;
			}

			if (cur_step < sfx_len) {
				let is_audible = 0;

				if (cur_step < 32) {
					let has_vol = 0;

					for (let i = cur_step; i < 31; i += 1) {
						// sfx_ptr + 0x14 = sfx_ptr + 0x20 in p8 binary
						if (c[(sfx_ptr + 0x14 + i * 0x14 + 8) >> 2] > 0) {
							has_vol = 1;
							break;
						}
					}

					if (
						cur_step < 31 &&
						// step has slide cmd
						// sfx_ptr + 0x14 = sfx_ptr + 0x20 in p8 binary
						c[(sfx_ptr + 0x14 + cur_step * 0x14 + 0xc) >> 2] == 1
					)
						is_audible = 1;
					else {
						is_audible = has_vol;
					}
				}

				if (
					!is_audible ||
					// 0x2ed8 = 0x2ee4 in p8 binary
					(c[(ch_state + 0x2ed8) >> 2] == 0 &&
						// @TODO is this correct?
						next_sfx_tick >= (c[cur_step_tick_addr >> 2] << 5) + 0xc8)
				) {
					c[sfx_ptr_addr >> 2] = 0;
				}
			}
		}
	}

	/*
	 * Return early (continue playing pattern)
	 */
	// 0x201c = 0x2020 in p8 binary
	const loaded_cart_rom_addr = ch_state + 0x201c;
	const loaded_cart_rom_ptr = c[loaded_cart_rom_addr >> 2];

	if (!loaded_cart_rom_ptr) {
		Ia = e;
		return;
	}

	// 0x2d1c = 0x2d28 in p8 binary
	const ch_enabled_addr = ch_state + 0x2d1c;

	if (!(pat_ticks_remaining == 0 && c[ch_enabled_addr >> 2] != 0)) {
		Ia = e;
		return;
	}

	// 0x2d20 = 0x2d2c in p8 binary
	const cur_pat_idx_addr = ch_state + 0x2d20;
	const cur_pat_idx = c[cur_pat_idx_addr >> 2];

	if (cur_pat_idx > 63) {
		Ia = e;
		return;
	}

	/*
	 * Advance to next pattern
	 */
	// 0x88048 = advanced_pattern global in p8 binary
	c[0x88048] = 1;

	// loaded_cart_rom + 0xae10 = loaded_cart_rom + 0xae20 in p8 binary
	const loop_mask = c[(loaded_cart_rom_ptr + 0xae10 + (cur_pat_idx << 2)) >> 2];

	// Stop loop set
	if ((loop_mask & 4) != 0) {
		c[cur_pat_idx_addr >> 2] = -1;
		// 0x877db = codo_state[640_4] in p8 binary
		c[0x877db] = -1;

		init_ch_state(ch_state);

		// @TODO asm.js only
		Ia = e;

		return;
	}

	let next_pat_idx = cur_pat_idx;

	// end loop unset
	if ((loop_mask & 2) == 0) {
		next_pat_idx = cur_pat_idx + 1;
		// begin loop unset
	} else if ((loop_mask & 1) == 0) {
		while (
			next_pat_idx > 0 &&
			// next_pat begin loop unset
			// loaded_cart_rom + 0xae10 = loaded_cart_rom + 0xae20 in p8 binary
			(c[(loaded_cart_rom_ptr + 0xae10 + (next_pat_idx << 2)) >> 2] & 1) == 0
		) {
			next_pat_idx -= 1;
		}
	}

	c[cur_pat_idx_addr >> 2] = next_pat_idx;
	// 0x877db = codo_state[640_4]? in p8 binary
	c[0x877db] = next_pat_idx;

	if (next_pat_idx > 63) {
		return;
	}

	// loaded_cart_rom + 0xaa10 = loaded_cart_rom + 0xaa20 in p8 binary
	const next_pat_addr = loaded_cart_rom_ptr + 0xaa10 + (next_pat_idx << 4);

	if (
		c[next_pat_addr >> 2] > 63 &&
		c[(next_pat_addr + 4) >> 2] > 63 &&
		c[(next_pat_addr + 8) >> 2] > 63 &&
		c[(next_pat_addr + 0xc) >> 2] > 63
	) {
		init_ch_state(ch_state);

		// @TODO asm.js only
		Ia = e;

		return;
	}

	// cur_cart_pat addr + ch target
	// loaded_cart_rom + 0xaa10 = loaded_cart_rom + 0xaa20 in p8 binary
	// 0x2030 = 0x203c in p8 binary
	let sfx_idx =
		c[
			(loaded_cart_rom_ptr +
				0xaa10 +
				(next_pat_idx << 4) +
				(c[(ch_state + 0x2030) >> 2] << 2)) >>
				2
		];

	sfx_idx = sfx_idx > 0 ? sfx_idx : 0;

	c[sfx_ptr_addr >> 2] =
		// loaded_cart_rom_ptr + 0x10 = loaded_cart_ptr + 0x20 in p8 binary
		sfx_idx <= 63 ? loaded_cart_rom_ptr + 0x10 + sfx_idx * 680 : 0;
	c[cur_sfx_tick_addr >> 2] = 0;
	c[cur_step_tick_addr >> 2] = 0;
	// chunk buffer samples remaining
	// 0x2d18 = 0x2d24 in p8 binary
	b[(ch_state + 0x2d18) >> 1] = 183;
	// get_pattern_ticks_length()
	c[pat_ticks_remaining_addr >> 2] = Pq(loaded_cart_rom_ptr, next_pat_addr);

	// @TODO asm.js only
	Ia = e;
}
