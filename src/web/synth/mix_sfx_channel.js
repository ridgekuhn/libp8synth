/**
 * Mix sfx channel
 */
function mix_sfx_channel(ch_state, chunk_len) {
	// 0x2ee8 = 0x2ef4 in p8 binary
	// 0x2eec = 0x2ef8 in p8 binary
	c[(ch_state + 0x2ee8) >> 2] = c[(ch_state + 0x2eec) >> 2];

	const program_channels = c[codo_state_664] == 0 ? 1 : c[codo_state_664] << 1;
	const t = (((chunk_len << 1) / program_channels) * 1000) / c[codo_state_660];

	const now = codo_get_time();

	// 0x2eec = 0x2ef8 in p8 binary
	const expire = ((c[(ch_state + 0x2eec) >> 2] + t) * 6 + (now << 1)) / 8;

	const max_expire = now - 200;

	// 0x2eec = 0x2ef8 in p8 binary
	c[(ch_state + 0x2eec) >> 2] = expire > max_expire ? expire : max_expire;

	/*
	 * Init mixer history
	 */
	// 0x2ee4 = 0x2ef0 in p8 binary
	c[(ch_state + 0x2ee4) >> 2] = 0;

	// 0x2ef0 = 0x2f00 in p8 binary
	memset(ch_state + 0x2ef0, 0, 0x600);

	let tick_samples_remaining = chunk_len;
	let chunk_buffer_pos = ch_state;

	while (tick_samples_remaining > 0) {
		// 0x2d18 = 0x2d24 in p8 binary
		const samples_remaining = b[(ch_state + 0x2d18) >> 1];

		if (samples_remaining < 183) {
			let samples_to_mix = 183 - samples_remaining;
			samples_to_mix =
				samples_to_mix < tick_samples_remaining
					? samples_to_mix
					: tick_samples_remaining;

			// 0x2034 = 0x2040 in p8 binary
			memcpy(
				chunk_buffer_pos,
				ch_state + 0x2034 + (samples_remaining << 1),
				samples_to_mix << 1,
			);

			// 0x2d18 = 0x2d24 in p8 binary
			b[(ch_state + 0x2d18) >> 1] =
				e[(ch_state + 0x2d18) >> 1] + samples_to_mix;

			tick_samples_remaining -= samples_to_mix;
			chunk_buffer_pos += samples_to_mix << 1;
		}

		/*
		 * Add history record
		 */
		if (c[(ch_state + 0x2ee4) >> 2] < 64) {
			add_mixer_state_history_record(ch_state);
		}

		if (tick_samples_remaining <= 0) {
			break;
		}

		/*
		 * Filter constants
		 */
		// 0x2020 = 0x2028 in p8 binary
		const sfx_ptr = c[(ch_state + 0x2020) >> 2];
		// 0x2d6c = 0x2d78 in p8 binary
		const osc_drd = c[(ch_state + 0x2d6c) >> 2] >> 3;
		// 0x2030 = 0x203c in p8 binary, (channel target)
		const ch_target = c[(ch_state + 0x2030) >> 2];
		const hi_filter_mask = 1 << (ch_target + 4);
		const lo_filter_mask = 1 << ch_target;
		const filter_mask = 0x11 << ch_target;

		/*
		 * Reverb
		 */
		const sfx_reverb = sfx_ptr ? ((c[sfx_ptr >> 2] >> 3) / 3) % 3 : 0;
		const osc_reverb = (osc_drd / 3) % 3;

		const global_reverb = c[GLOBAL_REVERB];

		let total_reverb = sfx_reverb > osc_reverb ? sfx_reverb : osc_reverb;

		if ((global_reverb & hi_filter_mask) != 0 && total_reverb <= 1) {
			total_reverb = 1;
		}

		if ((global_reverb & lo_filter_mask) != 0) {
			total_reverb = 2;
		}

		// 0x2ed8 = 0x2ee4 in p8 binary
		c[(ch_state + 0x2ed8) >> 2] = total_reverb;

		/*
		 * Dampen
		 */
		const sfx_dampen = sfx_ptr ? ((c[sfx_ptr >> 2] >> 3) / 9) % 3 : 0;
		const osc_dampen = (osc_drd / 9) % 3;

		const global_dampen = c[GLOBAL_DAMPEN] | 0;

		let total_dampen = sfx_dampen > osc_dampen ? sfx_dampen : osc_dampen;

		if (total_dampen == 2) {
			total_dampen = 12;
		} else if (total_dampen == 1) {
			total_dampen = 8;
		}

		if ((global_dampen & hi_filter_mask) != 0) {
			total_dampen = total_dampen > 8 ? total_dampen : 8;
		}

		if ((global_dampen & lo_filter_mask) != 0) {
			total_dampen = total_dampen > 12 ? total_dampen : 12;
		}

		if ((global_dampen & filter_mask) == filter_mask) {
			total_dampen = total_dampen > 15 ? total_dampen : 15;
		}

		// 0x2edc = 0x2ee8 in p8 binary
		c[(ch_state + 0x2edc) >> 2] = total_dampen;

		/*
		 * Mix
		 */
		// 0x2034 = 0x2040 in p8 binary
		const tick_buffer = ch_state + 0x2034;

		// @TODO this overwrites step state?
		mix_sfx_tick(ch_state, tick_buffer);

		/*
		 * Bitcrush
		 */
		const global_bitcrush = c[GLOBAL_BITCRUSH];

		// distort
		if (lo_filter_mask & global_bitcrush) {
			for (let i = 0; i < 183; i += 1) {
				const sample_addr = tick_buffer + (i << 1);
				const old_sample = b[sample_addr >> 1];
				let new_sample = (old_sample << 16) >> 16;

				if ((old_sample << 16) >> 16 > -1) {
					new_sample = new_sample & -0x1000;
				} else {
					new_sample = 0 - (((0 - (new_sample << 16)) >>> 16) & 0xf000);
				}

				b[sample_addr >> 1] = new_sample;
			}
		} else if (hi_filter_mask & global_bitcrush) {
			for (let i = 0; i < 183; i += 1) {
				const sample_addr = tick_buffer + (i << 1);

				b[sample_addr >> 1] = b[sample_addr >> 1] & -0xff9;
			}
		}

		if (total_dampen > 0) {
			const dampen_dx = 16 - total_dampen;
			// 0x21a0 = 0x21ac in p8 binary
			const dampen_seed =
				(B(dampen_dx, b[tick_buffer >> 1]) +
					B(total_dampen, (b[(ch_state + 0x21a0) >> 1] << 16) >> 16)) /
				16;

			b[tick_buffer >> 1] = dampen_seed;

			for (let i = 1; i < 183; i += 1) {
				const sample_addr = tick_buffer + (i << 1);

				const sample =
					(B(dampen_dx, b[sample_addr >> 1]) +
						B(total_dampen, (dampen_seed << 16) >> 16)) /
					16;
				b[sample_addr >> 1] = sample;
			}
		}

		/*
		 * Update mix state
		 */
		// 0x2d18 = 0x2d24 in p8 binary
		b[(ch_state + 0x2d18) >> 1] = 0;

		// 0x21a2 = 0x21ae in p8 binary
		// 0x2d14 = 0x2d20 in p8 binary
		memcpy(
			ch_state + 0x21a2 + (c[(ch_state + 0x2d14) >> 2] % 8) * 366,
			tick_buffer,
			366,
		);

		// 0x2d14 = 0x2d20 in p8 binary
		c[(ch_state + 0x2d14) >> 2] = (c[(ch_state + 0x2d14) >> 2] + 1) % 8;

		if (tick_samples_remaining <= 0) {
			break;
		}
	}

	// 0x2ee4 = 0x2ef0 in p8 binary
	if (c[(ch_state + 0x2ee4) >> 2] > 63) {
		return;
	}

	add_mixer_state_history_record(ch_state);
}
