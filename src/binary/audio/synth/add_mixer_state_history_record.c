#include "../../globals.h"

/**
 * Add Mixer State History Record
 */
void add_mixer_state_history_record(long ch_state) {
  const int history_idx = *(&ch_state + 0x2ef0);

  const int sfx_tick = *(&ch_state + 0x2030);

  *(&ch_state + 0x2f00 + history_idx * 0x18) = sfx_tick;

  // step tick
  *(&ch_state + 0x2f00 + history_idx * 0x18 + 4) = *(&ch_state + 0x2034);

  const long *sfx_ptr = &ch_state + 0x2028;

  *(&ch_state + 0x2f00 + history_idx * 0x18 + 8) = *sfx_ptr;

  // step idx
  if (*sfx_ptr) {
    int spd = *(sfx_ptr + 8);
    spd = spd > 1 ? spd : 1;

    *(&ch_state + 0x2f00 + history_idx * 0x18 + 0xc) = sfx_tick / spd;
  } else {
    *(&ch_state + 0x2f00 + history_idx * 0x18 + 0xc) = 0;
  }

  // pat idx
  *(&ch_state + 0x2f00 + history_idx * 0x18 + 0x10) = *(&ch_state + 0x2d2c);

  // patterns played
  *(&ch_state + 0x2f00 + history_idx * 0x18 + 0x14) = codo_state[636 >> 2];

  // 0x2ee4 = 0x2ef0 in p8 binary
  *(&ch_state + 0x2ee4) = history_idx + 1;
}
