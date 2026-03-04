/**
 * Music is playing
 */
function codo_music_is_playing() {
  codo_lock_audio_plat();

	for (let i = 0; i < 16; i += 1) {
		// 0x34f0 = 0x3700 in p8 binary
		const ch_state = ms0 + i * 0x34f0;

  // 0x2d1c = 0x2d28 in p8 binary
  // 0x201c = 0x2020 in p8 binary
		if (c[(ch_state + 0x2d1c) >> 2] != 0 && c[(ch_state + 0x201c) >> 2] != 0) {
			// 0x2d20 = 0x2d2c in p8 binary
			if (c[(ch_state + 0x2d20)] != -1) {
				return true;
			}
		}
	}

	// voxatron only
  // if (
	// 	c[codo_current_music] != 0 &&
	// 	c[ps0] != 0 &&
	// 	(fade1 != 0 || fade_vol != 0)
	// ) {
	// 	return MUSIC_PLAYING != 0;
	// }

  return false;
}
