#include "./mix_sfx_tick.h"
#include "../../globals.h"
#include "./get_pattern_ticks_length.h"
#include "./mix_osc_tick.h"
#include "./oscillators/calculate_osc_state.h"
#include "./stop_ch.h"
#include <string.h>

short ramp_buf[64];

/**
 * Mix sfx tick
 */
void mix_sfx_tick(long ch_state, short *tick_buffer) {
  /*
   * Mix oscillator tick
   */
  memset(tick_buffer, 0, 366);

  long *sfx_ptr = &ch_state + 0x2028;
  long *cur_sfx_tick = &ch_state + 0x2030;

  int cur_pat_tick = 0;
  int cur_step = 0;
  int spd = 1;

  if (*sfx_ptr) {
    int prev_osc_state[90];

    spd = *(sfx_ptr + 8);
    spd = spd > 1 ? spd : 1;

    int *osc_state = (int *)(&ch_state + 0x2d30);

    memcpy(prev_osc_state, osc_state, 0x160);

    if ((*cur_sfx_tick % spd) == 0) {
      // step waveform
      *(&ch_state + 0x2e94) = *(&ch_state + 0x2d70);
      // step pitch
      *(&ch_state + 0x2e90) = *(&ch_state + 0x2d6c);
      // step vol
      *(&ch_state + 0x2e98) = *(&ch_state + 0x2d68);
    }

    calculate_osc_state(ch_state, osc_state);

    mix_osc_tick(osc_state, tick_buffer, SAMPLES_PER_TICK, ch_state);

    for (int *i = (int *)ramp_buf; i < (int *)(ramp_buf + 0xc8); i += 4) {
      *i = 0;
    }

    // mix_osc_tick()
    mix_osc_tick(prev_osc_state, ramp_buf, 64, ch_state);

    for (int i = 0; i < 64; i += 1) {
      short *sample_addr = tick_buffer + (i << 1);
      const short osc_sample = *sample_addr;
      const short ramp_sample = *(ramp_buf + (i << 1));
      const short new_ramp_sample =
          (((64 - i) * ramp_sample) + (i * osc_sample)) / 64;

      *sample_addr = new_ramp_sample;
    }

    cur_pat_tick = *(&ch_state + 0x2ee0);
    cur_step = (*cur_sfx_tick + 1) / spd;
  } else {
    // @TODO didn't we already do this at the top?
    memset(tick_buffer, 0, 366);

    int *osc_state = (int *)(&ch_state + 0x2d24);
    long *osc_target_vol_addr = &ch_state + 0x2d4c;

    // if phase increment or target volume != 0
    if (*(&ch_state + 0x2d38) ? *osc_target_vol_addr : 0) {
      // mix_osc_tick()
      mix_osc_tick(osc_state, tick_buffer, 64, ch_state);

      for (int i = 0; i < 64; i += 1) {
        short *sample_addr = tick_buffer + (i << 1);
        const short old_sample = *sample_addr;
        const short new_sample = ((64 - i) * old_sample) / 64;

        *sample_addr = new_sample;
      }
    }

    *osc_target_vol_addr = 0;

    // current pattern tick
    *(&ch_state + 0x2ee0) = 0;

    cur_step = *cur_sfx_tick + 1;
  }

  /*
   * Update mixer channel state
   */
  // increment pat ticks remaining
  long *pat_ticks_remaining = &ch_state + 0x2eec;
  *pat_ticks_remaining -= 1;

  // increment cur pattern tick
  *(&ch_state + 0x2ee0) = cur_pat_tick + 1;

  // increment cur step tick
  long *cur_step_tick = &ch_state + 0x2034;
  *cur_step_tick += 1;

  // increment cur sfx tick
  const int loop_start = *(sfx_ptr + 0xc);
  const int loop_end = *(sfx_ptr + 0x10);

  int next_sfx_tick = *cur_sfx_tick + 1;

  if (*sfx_ptr &&
      // step spd != 0
      *(&ch_state + 0x2038) != 0 &&
      // not wavetable instrument
      (loop_start & 128) == 0 && loop_start < loop_end &&
      // sfx has no ticks remaining
      next_sfx_tick >= (loop_end * spd)) {
    next_sfx_tick = loop_start * spd;
  }

  *cur_sfx_tick = next_sfx_tick;

  // update sfx ptr
  if (*sfx_ptr && *(&ch_state + 0x2d28) == 0) {
    const _Bool is_wavetable = (loop_start & 128) != 0;

    if (*pat_ticks_remaining == 0) {
      *sfx_ptr = 0;
    } else if (loop_end <= loop_start || is_wavetable) {
      int sfx_len = 0;

      if (!is_wavetable) {
        sfx_len = loop_start > 0 && loop_end == 0 ? loop_start : 32;
      }

      if (cur_step < sfx_len) {
        _Bool is_audible = 0;

        if (cur_step < 32) {
          _Bool has_vol = 0;

          for (int i = cur_step; i < 31; i += 1) {
            if (*(sfx_ptr + 0x14 + i * 0x14 + 8) > 0) {
              has_vol = 1;
              break;
            }
          }

          if (cur_step < 31 &&
              // step has slide cmd
              *(sfx_ptr + 0x14 + cur_step * 0x14 + 0xc) == 1)
            is_audible = 1;
          else {
            is_audible = has_vol;
          }
        }

        if (!is_audible || (*(&ch_state + 0x2ee4) == 0 &&
                            // @TODO is this correct?
                            next_sfx_tick >= (*cur_step_tick << 5) + 0xc8)) {
          *sfx_ptr = 0;
        }
      }
    }
  }

  /*
   * Return early (continue playing pattern)
   */
  const long *cart_ptr = &ch_state + 0x2020;

  if (!cart_ptr) {
    return;
  }

  const long *ch_enabled = &ch_state + 0x2d28;

  if (!(*pat_ticks_remaining == 0 && *ch_enabled != 0)) {
    return;
  }

  long *cur_pat_idx = &ch_state + 0x2d2c;

  if (*cur_pat_idx > 63) {
    return;
  }

  /*
   * Advance to next pattern
   */
  advanced_pattern = 1;

  const int loop_mask = *(cart_ptr + 0xae20 + (*cur_pat_idx << 2));

  // Stop loop set
  if ((loop_mask & 4) != 0) {
    *cur_pat_idx = -1;
    codo_state[640 >> 2] = -1;

    stop_ch(&ch_state);

    return;
  }

  int next_pat_idx = *cur_pat_idx;

  // end loop unset
  if ((loop_mask & 2) == 0) {
    next_pat_idx = *cur_pat_idx + 1;
    // begin loop unset
  } else if ((loop_mask & 1) == 0) {
    while (next_pat_idx > 0 &&
           // next_pat begin loop unset
           // loaded_cart_rom + 0xae10 = loaded_cart_rom + 0xae20 in p8 binary
           (*(cart_ptr + 0xae10 + (next_pat_idx << 2)) & 1) == 0) {
      next_pat_idx -= 1;
    }
  }

  *cur_pat_idx = next_pat_idx;
  codo_state[640 >> 2] = next_pat_idx;

  if (next_pat_idx > 63) {
    return;
  }

  const long *next_pat = cart_ptr + 0xaa20 + (next_pat_idx << 4);

  if (*next_pat > 63 && *(next_pat + 4) > 63 && *(next_pat + 8) > 63 &&
      *(next_pat + 0xc) > 63) {
    stop_ch(&ch_state);

    return;
  }

  // cur_cart_pat addr + ch target
  int sfx_idx =
      *(cart_ptr + 0xaa20 + (next_pat_idx << 4) + (*(&ch_state + 0x203c) << 2));

  sfx_idx = sfx_idx > 0 ? sfx_idx : 0;

  *sfx_ptr = sfx_idx <= 63 ? *(cart_ptr + 0x20 + sfx_idx * 680) : 0;
  *cur_sfx_tick = 0;
  *cur_step_tick = 0;
  // chunk buffer samples remaining
  *(&ch_state + 0x2d24) = SAMPLES_PER_TICK;
  *pat_ticks_remaining = get_pattern_ticks_length(*cart_ptr, (int *)next_pat);
}
