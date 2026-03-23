#include "./mix_channel_with_clockrate.h"
#include "../../globals.h"
#include "../../memory/codo_memset.h"
#include "./mix_sfx_channel.h"

/**
 * Mix channel with global clock rate
 */
void mix_channel_with_clockrate(long *ch_state, unsigned long chunk_len) {
  codo_memset(ch_state, 0, 0x2000);

  // if channel clock rate halved
  if ((1 << *(ch_state + 0x203c)) & audio_clock_mask) {
    mix_sfx_channel(ch_state, chunk_len / 2 + 1);

    if (chunk_len > 0) {
      for (int i = chunk_len - 1; i >= 0; i -= 1) {
        *(short *)(ch_state + (i << 1)) = *(ch_state + ((i / 2) << 1));
      }
    }
  } else {
    mix_sfx_channel(ch_state, chunk_len);
  }

  // if (*(ch_state + 0x2010) != 0 && chunk_len > 0) {
  // 	mix_voxatron_channel(ch_state, chunk_len);
  // }
}
