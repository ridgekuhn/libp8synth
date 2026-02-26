/**
 * codo_mixer_callback()
 */
function codo_mixer_callback(_userdata, chunk_buffer, chunk_len) {
  if (chunk_len <= 0) {
    return;
  }

  if (c[codo_state_612] || c[codo_state_604]) {
    memset(chunk_buffer, 0, chunk_len);
    return;
  }

  if (
    c[codo_state_668] == c[codo_state_660] &&
    c[codo_state_672] == c[codo_state_664]
  ) {
    codo_mixer_callback_0(0, chunk_buffer, chunk_len);
    return;
  }

  if (chunk_len <= 0) {
    return;
  }

  memset(chunk_buffer, 0, chunk_len);
}
