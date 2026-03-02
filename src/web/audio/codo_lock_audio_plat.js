/**
 * Lock SDL audio device
 */
function codo_lock_audio_plat() {
	if (c[codo_audio_is_locked] == 0) {
		SDL_LockAudio();
	}

	c[codo_audio_is_locked] += 1;
}

