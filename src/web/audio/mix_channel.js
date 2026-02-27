/**
 * Mix channel
 */
function mix_channel(ch_state, chunk_len) {
  codo_memset(ch_state, 0, 0x2000);

  // if channel clock rate halved
  // 0x2030 = 0x203c in p8 binary, (channel target)
  if ((1 << c[(ch_state + 0x2030) >> 2]) & c[AUDIO_CLOCK_MASK]) {
    mix_sfx_channel(ch_state, chunk_len / 2 + 1);

    if (chunk_len > 0) {
      for (let i = chunk_len - 1; i >= 0; i -= 1) {
        b[(ch_state + (i << 1)) >> 1] = b[(ch_state + ((i / 2) << 1)) >> 1];
      }
    }
  } else {
    mix_sfx_channel(ch_state, chunk_len);
  }

  // if (c[(ch_state + 0x2010) >> 2] != 0 && chunk_len > 0) {
  // 	mix_voxatron_channel(ch_state, chunk_len);
  // }
}
