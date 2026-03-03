/**
 * Unlock SDL audio device
 */
function codo_unlock_audio_plat() {
	c[codo_audio_is_locked] -= 1;

	if (c[codo_audio_is_locked] == 0) {
		SDL_UnlockAudio();
	}
}
