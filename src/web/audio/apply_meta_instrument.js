/**
 * Apply meta instrument
 */
function apply_meta_instrument(ch_state, sfx_step, osc_state) {
	// 0x201c = 0x2020 in p8 binary
	const cart_ptr = c[(ch_state + 0x201c) >> 2];
	const meta_sfx_addr = meta_sfx_addr;

	if (!cart_ptr) {
		return;
	}

	let meta_idx = c[(sfx_step + 4) >> 2];
	meta_idx = meta_idx < 7 ? meta_idx : 7;
	meta_idx = meta_idx > 0 ? meta_idx : 0;

	c[(osc_state + 0x48) >> 2] = c[meta_sfx_addr >> 2];

	const loop_start = c[(meta_sfx_addr + 0xc) >> 2];

	// is wavetable
	if (loop_start & 0x80) {
		set_osc_wavetable(cart_ptr, osc_state, meta_idx);

		c[(osc_state + 0x44) >> 2] = 0;

		return;
	}

	let spd = c[(meta_sfx_addr + 8) >> 2];
	spd = spd > 1 ? spd : 1;

	const fast_spd = spd < 9;
	const arp_slow_spd = fast_spd ? 4 : 8;
	const arp_fast_spd = fast_spd ? 2 : 4;
	const loop_end = c[(meta_sfx_addr + 0x10) >> 2];
	const cur_pat_tick_addr = ch_state + 0x2ed4;
	const cur_pat_tick = c[cur_pat_tick_addr >> 2];

	let meta_tick = 0;

	if (loop_end <= loop_start || cur_pat_tick < B(spd, loop_end)) {
		meta_tick = cur_pat_tick;
	} else {
		meta_tick = B(spd, loop_start);
		c[cur_pat_tick_addr >> 2] = meta_tick;
	}

	let step = Math.floor(meta_tick / spd);
	step = step < 31 ? step : 31;
	step = step > 0 ? step : 0;

	const step_tick = meta_tick - B(step, spd);
	const loop_len = loop_start > 0 && loop_end == 0 ? loop_start : 32;

	if (step > 31 || step >= loop_len) {
		c[(osc_state + 0x1c) >> 2] = 0;
		c[osc_state >> 2] = 0;

		return;
	}

	const step_addr = meta_sfx_addr + 0x14 + step * 0x14;
	const meta_pitch_addr = step_addr;
	const meta_pitch = c[meta_pitch_addr >> 2];
	const meta_target_pitch = meta_pitch << 16;

	const meta_vol_addr = step_addr + 8;
	const meta_vol = c[meta_vol_addr >> 2];
	const meta_target_vol = meta_vol << 8;

	if (step_tick == spd - 1) {
		// 0x2e98 = 0x2ea4 in p8 binary
		c[(ch_state + 0x2e98) >> 2] = meta_pitch;
		// 0x2e9c = 0x2ea8 in p8 binary
		c[(ch_state + 0x2e9c) >> 2] = c[(step_addr + 4) >> 2];
		// 0x2ea0 = 0x2eac in p8 binary
		c[(ch_state + 0x2ea0) >> 2] = meta_vol;
	}

	// cart_ptr + 0x10 = cart_ptr + 0x20 in p8 binary
	const meta_effect_addr = step_addr + 0xc;
	const meta_effect = c[meta_effect_addr >> 2];

	let new_target_pitch = 0;
	let new_target_vol = 0;

	switch (meta_effect) {
		// slide
		case 1: {
			let slide_target_pitch = 0;
			let slide_target_vol = 0;

			if (step > 0) {
				// 0x2e98 = 0x2ea4 in p8 binary
				slide_target_pitch = c[(ch_state + 0x2e98) >> 2] << 16;
				// 0x2ea0 = 0x2eac in p8 binary
				slide_target_vol = c[(ch_state + 0x2ea0) >> 2] << 8;
			} else {
				slide_target_pitch = 0x180000;
				slide_target_vol = meta_target_vol;
			}

			const ticks_remaining = spd - step_tick;

			new_target_pitch =
				(B(slide_target_pitch, ticks_remaining) +
					B(step_tick << 16, c[meta_pitch_addr >> 2])) /
				spd;

			new_target_vol =
				(B(slide_target_vol, ticks_remaining) + B(meta_target_vol, step_tick)) /
				spd;

			break;
		}
		// drop
		case 3: {
			new_target_pitch = meta_target_pitch;
			new_target_vol = meta_target_vol;

			c[(osc_state + 0x4c) >> 2] = ((spd - step_tick) << 8) / spd;

			break;
		}
		// fade in
		case 4: {
			new_target_pitch = meta_target_pitch;
			new_target_vol = B(meta_target_vol, step_tick) / spd;

			break;
		}
		// fade out
		case 5: {
			new_target_pitch = meta_target_pitch;
			new_target_vol = B(meta_target_vol, spd - step_tick) / spd;

			break;
		}
		// arp fast
		case 6: {
			const arp_step =
				(Math.floor(meta_tick / arp_fast_spd) % 4) + (step & 0x1c);

			new_target_pitch = c[(meta_sfx_addr + 0x14 + arp_step * 0x14) >> 2] << 16;

			new_target_vol = meta_target_vol;

			break;
		}
		// arp slow
		case 7: {
			const arp_step = (Math.floor(meta_tick / arp_slow_spd) % 4) + (step & 28);

			new_target_pitch = c[(meta_sfx_addr + 20 + arp_step * 0x14) >> 2] << 16;

			new_target_vol = meta_target_vol;

			break;
		}
		default: {
			new_target_pitch = meta_target_pitch;
			new_target_vol = meta_target_vol;
		}
	}

	let meta_waveform = c[(step_addr + 4) >> 2];
	meta_waveform = meta_waveform < 7 ? meta_waveform : 7;
	meta_waveform = meta_waveform > 0 ? meta_waveform : 0;

	if (
		// step has meta instrument bit
		c[(step_addr + 0x10) >> 2] != 0 &&
		// step is wavetable
		(c[(cart_ptr + 0x10 + meta_waveform * 680 + 0xc) >> 2] & 0x80) != 0
	) {
		c[(osc_state + 0x48) >> 2] =
			c[(cart_ptr + 0x10 + meta_waveform * 680) >> 2];

		set_osc_wavetable(cart_ptr, osc_state, meta_waveform);
	} else {
		c[osc_state >> 2] = meta_waveform;
	}

	c[(osc_state + 0x20) >> 2] += new_target_pitch - 0x180000;

	c[(osc_state + 0x24) >> 2] += c[meta_pitch_addr >> 2] - 24;

	c[(osc_state + 0x28) >> 2] =
		(new_target_vol * 7) / (meta_vol > 1 ? meta_vol : 1);

	const osc_target_vol_addr = osc_state + 0x1c;

	c[osc_target_vol_addr >> 2] =
		B(c[osc_target_vol_addr >> 2], new_target_vol) / 1792;

	c[(osc_state + 0x44) >> 2] = meta_effect;
}
