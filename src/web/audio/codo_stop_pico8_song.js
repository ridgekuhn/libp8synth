/**
 * Stop Pico-8 song
 */
function codo_stop_pico8_song(cart_ptr) {
  codo_lock_audio_plat();

  if (cart_ptr) {
    for (let i = 0; i < 16; i += 1) {
      // 0x34f0 = 0x3700 in p8 binary
      const ch_state = ms0 + i * 0x34f0;
      // 0x201c = 0x2020 in p8 binary
      const ch_cart_ptr = c[(ch_state + 0x201c) >> 2];

      if (
        // 0x2d1c = 0x2d28 in p8 binary
        c[(ch_state + 0x2d1c) >> 2] &&
        ch_cart_ptr != 0 &&
        ch_cart_ptr == cart_ptr
      ) {
        init_ch_state(ch_state);
      }
    }
  } else {
    for (let i = 0; i < 16; i += 1) {
      // 0x34f0 = 0x3700 in p8 binary
      const ch_state = ms0 + i * 0x34f0;

      // 0x2d1c = 0x2d28 in p8 binary
      if (c[ch_state + 0x2d1c]) {
        init_ch_state(ch_state);
      }
    }
  }

  codo_unlock_audio_plat();
}
