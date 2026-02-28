/**
 * Set oscillator wavetable bytes
 */
function set_osc_wavetable(cart_ptr, osc_state, wavetable_idx) {
	// cart_ptr + 0x10 = cart_ptr + 0x20 in p8 binary
	const sfx_addr = cart_ptr + 0x10 + wavetable_idx * 680 + 0x14;

	// Set phasor
	c[osc_state >> 2] = 8;

	// Has bass flag
	// cart_ptr + 0x10 = cart_ptr + 0x20 in p8 binary
	if (c[(cart_ptr + 0x10 + wavetable_idx * 680 + 8) >> 2] & 1) {
		// Subtract an octave from oscillator target pitch
		c[(osc_state + 0x20) >> 2] -= 0xc0000;
	}

	// Populate wavetable
	for (let h = 0; h < 32; h += 1) {
		const sample_addr = sfx_addr + h * 0x14;

		/*
		 * Re-encode sfx bytes
		 */
		const waveform_bits = c[(sample_addr + 4) >> 2];
		const hi_waveform_bit = (waveform_bits >>> 2) & 1;
		const lo_waveform_bits = (waveform_bits << 6) & 0xc0;
		const vol_bits = (c[(sample_addr + 8) >> 2] << 1) & 0xe;
		const effect_bits = (c[(sample_addr + 0xc) >> 2] << 4) & 0x70;
		const meta_bit = c[(sample_addr + 0x10) >> 2] & 0x80;
		const pitch_bits = c[sample_addr >> 2];

		const lo_byte = lo_waveform_bits | pitch_bits;
		const hi_byte = hi_waveform_bit | vol_bits | effect_bits | meta_bit;

		/*
		 * Convert signed 8-bit samples to native format
		 */
		const even_sample = lo_byte - ((lo_byte << 1) & 256);
		const odd_sample = hi_byte - ((hi_byte << 1) & 256);

		const even_offset = h << 3;
		const odd_offset = even_offset + 4;

		c[(osc_state + 0x60 + even_offset) >> 2] = even_sample << 7;
		c[(osc_state + 0x60 + odd_offset) >> 2] = odd_sample << 7;
	}
}
