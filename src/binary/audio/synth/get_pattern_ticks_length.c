#include "./get_pattern_ticks_length.h"

/**
 * Get pattern ticks length
 *
 * @returns length of pattern in ticks
 */
int get_pattern_ticks_length(long cart_ptr, int *pattern_ptr) {
  int result = 0;

  for (int i = 0; i < 4; i += 1) {
    const int sfx_idx = *(pattern_ptr + 4 * i);
    const int loop_start = *(&cart_ptr + 0x20 + sfx_idx * 680 + 0xc);
    const int loop_end = *(&cart_ptr + 0x20 + sfx_idx * 680 + 0x10);

    int spd = *(&cart_ptr + 0x20 + sfx_idx * 680 + 8);
    spd = spd > 1 ? spd : 1;

    int sfx_len = 0;

    if ((loop_start & 128) == 0) {
      sfx_len = loop_start > 0 && loop_end == 0 ? loop_start : 32;
    }

    const int step_len = sfx_len * spd;

    if (sfx_idx < 64) {
      if (loop_end <= loop_start) {
        return step_len;
      } else if (result <= step_len) {
        result = step_len;
      } else {
        return result;
      }
    }
  }

  return result;
}
