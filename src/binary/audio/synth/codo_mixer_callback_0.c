#include "./codo_mixer_callback_0.h"
#include "../../globals.h"
#include "../../time/codo_get_time.h"
#include "./mix_channel_with_clockrate.h"
#include <string.h>

int prev_time = 0;

/**
 * Internal Mixer Callback
 */
void codo_mixer_callback_0(int _userdata, short *chunk_buffer, int chunk_len) {
  /*
   * Init mixer buffer
   */
  last_callback_len = chunk_len;

  // @TODO asm.js only?
  codo_state[608 >> 2] += 1;
  if (!(codo_state[608 >> 2] < 3 || codo_state[612 >> 2] != 0)) {
    memset(chunk_buffer, 0, chunk_len);
    return;
  }

  inside_codo_mixer_callback = 1;

  const int time = codo_get_time();

  if (prev_time != -1) {
    const _Bool expired = time - prev_time > 500;

    prev_time = codo_get_time();

    if (expired) {
      if (chunk_len <= 0) {
        return;
      }

      memset(chunk_buffer, 0, chunk_len << 1);

      return;
    }
  } else {
    prev_time = time;
  }

  advanced_pattern = 0;

  /*
   * Mix channels
   */
  const int program_channels = codo_state[664 >> 2];

  int ch_chunk_len =
      chunk_len / (program_channels == 0 ? 1 : program_channels << 1);
  ch_chunk_len =
      codo_state[660 >> 2] == 44100 ? ch_chunk_len / 2 : ch_chunk_len;

  for (int i = 0; i < 16; i += 1) {
    mix_channel_with_clockrate((long *)&ms0 + 0x3700 * i, ch_chunk_len);
  }

  // @see mix_sfx_tick()
  if (advanced_pattern) {
    codo_state[636 >> 2] = codo_state[636 >> 2] + 1;
  }

  // @TODO Voxatron only?
  if (sound_initialized) {
    for (int i = 0; i < 3; i += 1) {
      // if ch i has sfx pointer, zero out ch i + 4
      if (*(&ms0 + 0x3700 * i + 0x2028)) {
        memset(&ms0 + 0x3700 * (i + 4), 0, chunk_len);
      }
    }
  }

  const _Bool has_chunks = ch_chunk_len > 0;
  const int max_channels = sound_initialized ? 8 : 16;

  if (has_chunks) {
    // sum channels in pairs until mixed down
    // (ie, 0+1, 2+3, 4+5, 6+7, then 0+3, 4+5, then 0+4)
    for (int i = 2; i <= max_channels; i *= 2) {
      int x = i / 2;

      for (int j = 0; j < max_channels; j += i) {
        const long base_ch_offset = (long)&ms0 + j * 0x3700;
        const long unmixed_ch_offset = (long)&ms0 + (j + x) * 0x3700;

        for (int s = 0; s < ch_chunk_len; s += 1) {
          const int sample_offset = s << 1;
          short *base_sample = (short *)(base_ch_offset + sample_offset);
          const short *unmixed_sample =
              (short *)(unmixed_ch_offset + sample_offset);
          const short summed_sample = *base_sample + *unmixed_sample;

          int normalized_sample = 0;

          if (summed_sample <= 0x5fff)
            if (summed_sample < -0x5fff) {
              normalized_sample = (summed_sample + 0x6000) / 5 - 0x6000;
            } else {
              normalized_sample = summed_sample;
            }
          else {
            normalized_sample = (summed_sample - 0x6000) / 5 + 0x6000;
          }

          *base_sample = normalized_sample;
        }
      }
    }
  }

  /*
   * Apply volume
   */
  if (sound_volume != 256 && has_chunks) {
    for (int i = 0; i < ch_chunk_len; i += 1) {
      short *sample = (short *)&ms0 + (i << 1);
      *sample = (sound_volume * *sample) >> 8;
    }
  }

  // voxatron only
  // if (music_playing && ps0 && fade_vol) {
  //   const int new_vol = (music_volume * fade_vol) >> 16;

  //   if (codo_current_music) {
  //     codo_mix_xm_chunk(ps0, xmbuf, ch_chunk_len);
  //   }

  //   if (has_chunks) {
  //     for (int i = 0; i < ch_chunk_len; i += 1) {
  //       short *sample = (short *)&xmbuf + (i << 1);
  //       *sample = (new_vol * *sample) >> 8;
  //     }

  //     for (int i = 0; i < ch_chunk_len; i += 1) {
  //       short *mixer_sample = (short *)&ms0 + (i << 1);
  //       const int summed_samples = *(xmbuf + (i << 1)) + *mixer_sample;

  //       int new_sample = 0;

  //       if (summed_samples <= 0x5fff) {
  //         if (summed_samples < -0x5fff) {
  //           new_sample = (summed_samples + 0x6000) / 5 - 0x6000;
  //         } else {
  //           new_sample = summed_samples;
  //         }
  //       } else {
  //         new_sample = (summed_samples - 0x6000) / 5 + 0x6000;
  //       }

  //       *mixer_sample = new_sample;
  //     }
  //   }
  // }

  if (has_chunks) {
    if (codo_state[660 >> 2] == 22050 && codo_state[664 >> 2] == 1) {
      for (int i = 0; i < ch_chunk_len; i += 1) {
        *(chunk_buffer + (i << 1)) = *(ms0 + (i << 1));
      }
    } else if (codo_state[660 >> 2] == 44100 && codo_state[664 >> 2] == 2) {
      for (int i = 0; i < ch_chunk_len << 2; i += 1) {
        *(chunk_buffer + (i << 1)) = *(ms0 + ((i >> 2) << 1));
      }
    }
  }

  if (&codo_post_mix_func) {
    // set in codo_main_init()
    codo_post_mix_func(chunk_buffer, chunk_len / 2);
  }

  // runtime recording
  // if (codo_state[660 >> 2] == 22050 && codo_state[664 >> 2] == 1 &&
  //     codo_state[632 >> 2] != 0) {
  //   const int max_samples = chunk_len / (codo_state[664 >> 2] * 2);

  //   if (max_samples > 0) {
  //     for (int i = 0; i < max_samples; i += 1) {
  //       codo_fwrite_int16(*(chunk_buffer + (i << 1)), (int *)codo_audio_buffer);
  //     }
  //   }
  // }

  inside_codo_mixer_callback = 0;
}
