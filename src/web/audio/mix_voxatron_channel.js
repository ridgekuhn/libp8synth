/**
 * Mix Voxatron channel
 */
function mix_voxatron_channel(ch_state, chunk_len) {
	// 0x2010 = 0x2010 in p8 binary
	const voxatron_ch_ptr = c[(ch_state + 0x2010) >> 2];

	let i = 0;

	while (i < chunk_len) {
		const samples_mixed = chunk_len - i;

		// 0x2018 = 0x201c in p8 binary
		let voxatron_samples_mixed =
			c[voxatron_ch_ptr >> 2] - c[(ch_state + 0x2018) >> 2];
		voxatron_samples_mixed =
			samples_mixed < voxatron_samples_mixed
				? samples_mixed
				: voxatron_samples_mixed;

		if (samples_mixed < 1) {
			// 0x2018 = 0x201c in p8 binary
			if (c[voxatron_ch_ptr >> 2] > c[(ch_state + 0x2018) >> 2]) {
				return;
			} else {
				init_ch_state();

				return;
			}
		}

		for (let j = 0; j < samples_mixed; j += 1) {
			// 0x2018 = 0x201c in p8 binary
			b[(ch_state + ((i + j) << 1)) >> 1] =
				b[
					(c[(voxatron_ch_ptr + 0x14) >> 2] +
						((c[(ch_state + 0x2018) >> 2] + j) << 1)) >>
						1
				];
		}

		let samples_remaining = i - chunk_len - 1;

		// 0x2018 = 0x201c in p8 binary
		const voxatron_samples_remaining =
			c[(ch_state + 0x2018) >> 2] - c[voxatron_ch_ptr >> 2] - 1;

		samples_remaining =
			samples_remaining > voxatron_samples_remaining
				? samples_remaining
				: voxatron_samples_remaining;

		// 0x2018 = 0x201c in p8 binary
		const new_voxatron_samples_remaining =
			c[(ch_state + 0x2018) >> 2] - samples_remaining - 1;

		// 0x2018 = 0x201c in p8 binary
		c[(ch_state + 0x2018) >> 2] =
			c[(voxatron_ch_ptr + 0x1c) >> 2] == 0 ||
			new_voxatron_samples_remaining < c[voxatron_ch_ptr >> 2]
				? new_voxatron_samples_remaining
				: 0;

		i = i - samples_remaining - 1;

		if (i >= chunk_len) {
			return;
		}
	}
}
