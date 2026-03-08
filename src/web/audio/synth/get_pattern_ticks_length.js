/**
 * Get pattern ticks length
 *
 * @returns {number} length of pattern in ticks
 */
function get_pattern_ticks_length(cart_ptr, pattern_ptr) {
	let prev_tick_len = 0;

	for (let i = 0; i < 4; i += 1) {
		const sfx_idx = c[(pattern_ptr + 4 * i) >> 2];
		// cart_ptr + 0x10 = cart_ptr + 0x20 in p8 binary
		const loop_start = c[(cart_ptr + 0x10 + sfx_idx * 680 + 0xc) >> 2];
		// cart_ptr + 0x10 = cart_ptr + 0x20 in p8 binary
		const loop_end = c[(cart_ptr + 0x10 + sfx_idx * 680 + 0x10) >> 2];
		const not_wavetable = (loop_start & 128) == 0;

		// cart_ptr + 0x10 = cart_ptr + 0x20 in p8 binary
		let spd = c[(cart_ptr + 0x10 + sfx_idx * 680 + 8) >> 2];
		spd = spd > 1 ? spd : 1;

		let len = 0;

		if (not_wavetable) {
			len = loop_start > 0 && loop_end == 0 ? loop_start : 32;
		}

		const tick_len = B(len, spd);

		if (sfx_idx < 64) {
			if (loop_end <= loop_start) {
				return tick_len;
			} else if (prev_tick_len <= tick_len) {
				prev_tick_len = tick_len;
			} else {
				return prev_tick_len;
			}
		}
	}

	return prev_tick_len;
}
