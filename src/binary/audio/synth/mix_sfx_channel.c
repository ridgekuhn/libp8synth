#include "../../globals.h"
#include "../../time/codo_get_time.h"
#include "./mix_sfx_tick.h"
#include "add_mixer_state_history_record.h"
#include <string.h>

/**
 * Mix SFX Channel
 */
void mix_sfx_channel(long *ch_state, unsigned long chunk_len) {
  *(ch_state + 0x2ef4) = *(ch_state + 0x2ef8);

  const int program_channels =
      codo_state[664 >> 2] == 0 ? 1 : codo_state[664 >> 2] << 1;
  const int t =
      (((chunk_len << 1) / program_channels) * 1000) / codo_state[660 >> 2];

  const int now = codo_get_time();

  const int expire = ((*(ch_state + 0x2ef8) + t) * 6 + (now << 1)) / 8;

  const int max_expire = now - 200;

  *(ch_state + 0x2ef8) = expire > max_expire ? expire : max_expire;

  /*
   * Init mixer history
   */
  *(ch_state + 0x2ef0) = 0;

  memset(ch_state + 0x2f00, 0, 0x600);

  int tick_samples_remaining = chunk_len;
  long *chunk_buffer_pos = ch_state;

  while (tick_samples_remaining > 0) {
    const short samples_remaining = *(ch_state + 0x2d24);

    if (samples_remaining < SAMPLES_PER_TICK) {
      int samples_to_mix = SAMPLES_PER_TICK - samples_remaining;
      samples_to_mix = samples_to_mix < tick_samples_remaining
                           ? samples_to_mix
                           : tick_samples_remaining;

      memcpy(chunk_buffer_pos, ch_state + 0x2040 + (samples_remaining << 1),
             samples_to_mix << 1);

      *(short *)(ch_state + 0x2d24) =
          *(unsigned short *)(ch_state + 0x2d24) + samples_to_mix;

      tick_samples_remaining -= samples_to_mix;
      chunk_buffer_pos += samples_to_mix << 1;
    }

    /*
     * Add history record
     */
    if (*(ch_state + 0x2ef0) < 64) {
      add_mixer_state_history_record(*ch_state);
    }

    if (tick_samples_remaining <= 0) {
      break;
    }

    /*
     * Filter constants
     */
    const long *sfx_ptr = ch_state + 0x2028;
    const int osc_drd = *(ch_state + 0x2d78) >> 3;
    const int ch_target = *(ch_state + 0x203c);
    const int hi_filter_mask = 1 << (ch_target + 4);
    const int lo_filter_mask = 1 << ch_target;
    const int filter_mask = 0x11 << ch_target;

    /*
     * Reverb
     */
    const int sfx_reverb = *sfx_ptr ? ((*sfx_ptr >> 3) / 3) % 3 : 0;
    const int osc_reverb = (osc_drd / 3) % 3;

    int total_reverb = sfx_reverb > osc_reverb ? sfx_reverb : osc_reverb;

    if ((global_reverb & hi_filter_mask) != 0 && total_reverb <= 1) {
      total_reverb = 1;
    }

    if ((global_reverb & lo_filter_mask) != 0) {
      total_reverb = 2;
    }

    *(ch_state + 0x2ee4) = total_reverb;

    /*
     * Dampen
     */
    const int sfx_dampen = *sfx_ptr ? ((*sfx_ptr >> 3) / 9) % 3 : 0;
    const int osc_dampen = (osc_drd / 9) % 3;

    int total_dampen = sfx_dampen > osc_dampen ? sfx_dampen : osc_dampen;

    if (total_dampen == 2) {
      total_dampen = 12;
    } else if (total_dampen == 1) {
      total_dampen = 8;
    }

    if ((global_dampen & hi_filter_mask) != 0) {
      total_dampen = total_dampen > 8 ? total_dampen : 8;
    }

    if ((global_dampen & lo_filter_mask) != 0) {
      total_dampen = total_dampen > 12 ? total_dampen : 12;
    }

    if ((global_dampen & filter_mask) == filter_mask) {
      total_dampen = total_dampen > 15 ? total_dampen : 15;
    }

    *(ch_state + 0x2ee8) = total_dampen;

    /*
     * Mix
     */
    short *tick_buffer = (short *)(ch_state + 0x2040);

    mix_sfx_tick(*ch_state, tick_buffer);

    /*
     * Bitcrush
     */
    // distort
    if (lo_filter_mask & global_bitcrush) {
      for (int i = 0; i < SAMPLES_PER_TICK; i += 1) {
        short *sample_addr = tick_buffer + (i << 1);
        const short old_sample = *sample_addr;
        int new_sample = (old_sample << 16) >> 16;

        if ((old_sample << 16) >> 16 > -1) {
          new_sample = new_sample & -0x1000;
        } else {
          new_sample = 0 - (((0 - (new_sample << 16)) >> 16) & 0xf000);
        }

        *sample_addr = new_sample;
      }
    } else if (hi_filter_mask & global_bitcrush) {
      for (int i = 0; i < SAMPLES_PER_TICK; i += 1) {
        short *sample_addr = tick_buffer + (i << 1);

        *sample_addr = *sample_addr & -0xff9;
      }
    }

    if (total_dampen > 0) {
      const int dampen_dx = 16 - total_dampen;
      const int dampen_seed =
          ((dampen_dx * *tick_buffer) +
           (total_dampen * ((*(ch_state + 0x21ac) << 16) >> 16))) /
          16;

      *tick_buffer = dampen_seed;

      for (int i = 1; i < SAMPLES_PER_TICK; i += 1) {
        short *sample_addr = tick_buffer + (i << 1);

        const short sample = ((dampen_dx * *sample_addr) +
                              (total_dampen * ((dampen_seed << 16) >> 16))) /
                             16;
        *sample_addr = sample;
      }
    }

    /*
     * Update mix state
     */
    *(short *)(ch_state + 0x2d24) = 0;

    memcpy(ch_state + 0x21ae + (*(ch_state + 0x2d20) % 8) * bytes_per_tick,
           tick_buffer, bytes_per_tick);

    *(ch_state + 0x2d20) = (*(ch_state + 0x2d20) + 1) % 8;

    if (tick_samples_remaining <= 0) {
      break;
    }
  }

  if (*(ch_state + 0x2ef0) > 63) {
    return;
  }

  add_mixer_state_history_record(*ch_state);
}
