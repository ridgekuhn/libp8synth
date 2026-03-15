#include "./calculate_osc_wavetable.h"

/**
 * Calculate Oscillator Wavetable
 */
void calculate_osc_wavetable(int *cart_ptr, int *osc_state, int frame_idx) {
  const int *sfx_ptr = cart_ptr + 0x20 + frame_idx * 680;

  // Set phasor
  *osc_state = 8;

  // Has bass flag
  if (*(sfx_ptr + 8) & 1) {
    // Subtract an octave from oscillator target pitch
    *(osc_state + 0x20) -= 0xc0000;
  }

  // Populate wavetable
  for (int i = 0; i < 32; i += 1) {
    const int *step_ptr = sfx_ptr + 0x14 + i * 0x14;

    /*
     * Re-encode sfx bytes
     */
    const int waveform_bits = *(step_ptr + 4);
    const int hi_waveform_bit = (waveform_bits >> 2) & 1;
    const int lo_waveform_bits = (waveform_bits << 6) & 0xc0;
    const int vol_bits = (*(step_ptr + 8) << 1) & 0xe;
    const int effect_bits = (*(step_ptr + 0xc) << 4) & 0x70;
    const int meta_bit = *(step_ptr + 0x10) & 0x80;
    const int pitch_bits = *step_ptr;

    const int lo_byte = lo_waveform_bits | pitch_bits;
    const int hi_byte = hi_waveform_bit | vol_bits | effect_bits | meta_bit;

    /*
     * Convert signed 8-bit samples to native int
     */
    const int even_sample = lo_byte - ((lo_byte << 1) & 256);
    const int odd_sample = hi_byte - ((hi_byte << 1) & 256);

    const int even_offset = i << 3;
    const int odd_offset = even_offset + 4;

    *(osc_state + 0x60 + even_offset) = even_sample << 7;
    *(osc_state + 0x60 + odd_offset) = odd_sample << 7;
  }
}
