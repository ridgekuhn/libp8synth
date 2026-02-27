/**
 * Add mixer state history record
 */
function add_mixer_state_history_record(ch_state) {
	// 0x2ee4 = 0x2ef0 in p8 binary
	const history_idx = c[(ch_state + 0x2ee4) >> 2];

	// 0x2024 = 0x2030 in p8 binary
	const sfx_tick = c[(ch_state + 0x2024) >> 2];

	// 0x2ef0 = 0x2f00 in p8 binary
	c[(ch_state + 0x2ef0 + history_idx * 0x18) >> 2] = sfx_tick;

	// step tick
	// 0x2ef0 = 0x2f00 in p8 binary
	// 0x2028 = 0x2034 in p8 binary
	c[(ch_state + 0x2ef0 + history_idx * 0x18 + 4) >> 2] =
		c[(ch_state + 0x2028) >> 2];

	const sfx_ptr = c[(ch_state + 0x2020) >> 2];

	// sfx ptr
	// 0x2ef0 = 0x2f00 in p8 binary
	// 0x2020 = 0x2028 in p8 binary
	c[(ch_state + 0x2ef0 + history_idx * 0x18 + 8) >> 2] = sfx_ptr;

	// step idx
	// 0x2020 = 0x2028 in p8 binary
	if (sfx_ptr) {
		let spd = c[(sfx_ptr + 8) >> 2];
		spd = spd > 1 ? spd : 1;

		// 0x2ef0 = 0x2f00 in p8 binary
		c[(ch_state + 0x2ef0 + history_idx * 0x18 + 0xc) >> 2] = Math.floor(
			sfx_tick / spd,
		);
	} else {
		// 0x2ef0 = 0x2f00 in p8 binary
		c[(ch_state + 0x2ef0 + history_idx * 0x18 + 0xc) >> 2] = 0;
	}

	// pat idx
	// 0x2ef0 = 0x2f00 in p8 binary
	// 0x2d20 = 0x2d2c in p8 binary
	c[(ch_state + 0x2ef0 + history_idx * 0x18 + 0x10) >> 2] =
		c[(ch_state + 0x2d20) >> 2];

	// patterns played
	// 0x2ef0 = 0x2f00 in p8 binary
	c[(ch_state + 0x2ef0 + history_idx * 0x18 + 0x14) >> 2] = c[codo_state_636];

	// 0x2ee4 = 0x2ef0 in p8 binary
	c[(ch_state + 0x2ee4) >> 2] = history_idx + 1;
}
