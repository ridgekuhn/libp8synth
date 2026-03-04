#include "./globals.h"

CodoState codo_state = {
  .is_paused = 0,
  .audio_playing = 0,
  .codo_audio_buffer_init = 0,
  .patterns_played = 0,
  .cur_pat_idx = 0,
  .prog_sample_rate = 22050,
  .prog_channels = 1,
  .sdl_sample_rate = 0,
  .sdl_channels = 0,
};

int m_high = 0x1234567;
int m_low = 0xdeadbeef;
