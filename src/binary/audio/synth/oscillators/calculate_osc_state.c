#include "./calculate_osc_state.h"
#include "../../../globals.h"
#include "./apply_meta_instrument.h"
#include "./stop_osc.h"

static int note_dx[12] = {
    523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 984,
};

/**
 * Calculate Oscillator State
 */
void calculate_osc_state(int *ch_state, int *osc_state) {
  int *sfx_ptr = ch_state + 0x2028;

  if (sfx_ptr == 0) {
    stop_osc(osc_state);
    return;
  }

  /*
   * Step state
   */
  const int sfx_spd = *(sfx_ptr + 8);
  const int sfx_spd_min = sfx_spd > 1 ? sfx_spd : 1;
  const int sfx_tick = *(ch_state + 0x2030);
  const int sfx_step = sfx_tick / sfx_spd_min;

  if (sfx_step > 31) {
    stop_osc(osc_state);
    return;
  }

  int *sfx_step_ptr = sfx_ptr + 0x14 + sfx_step * 0x14;
  const int sfx_step_tick = sfx_tick - (sfx_step * sfx_spd_min);
  const int is_lo_spd = sfx_spd < 9;
  const int sfx_step_waveform = *(sfx_step_ptr + 4);

  *osc_state = sfx_step_waveform;

  const int sfx_step_vol = *(sfx_step_ptr + 8);
  const int sfx_step_vol_256 = sfx_step_vol << 8;

  int *osc_step_target_vol = osc_state + 28;

  *osc_step_target_vol = sfx_step_vol_256;

  const int sfx_step_pitch = *sfx_step_ptr;

  int *osc_step_target_pitch = osc_state + 32;

  *osc_step_target_pitch = sfx_step_pitch << 16;
  *(osc_state + 0x38) = sfx_step_vol;
  *(osc_state + 0x3c) = sfx_step_pitch;
  *(osc_state + 0x40) = sfx_step_waveform;

  const int sfx_effect = *(sfx_step_ptr + 0xc);

  /*
   * Apply effect command
   */
  int new_target_pitch = 0;
  int new_target_vol = 0;

  switch (sfx_effect) {
  // Slide
  case 1: {
    int prev_base_step_pitch = 0;
    int prev_step_target_vol = 0;

    if (sfx_step > 0) {
      prev_base_step_pitch = *(ch_state + 0x2e90);
      prev_step_target_vol = *(ch_state + 0x2e98) << 8;
    } else {
      prev_base_step_pitch = 24;
      prev_step_target_vol = sfx_step_vol_256;
    }

    const int step_ticks_remaining = sfx_spd_min - sfx_step_tick;

    new_target_pitch = ((step_ticks_remaining << 16 * prev_base_step_pitch) +
                        (*osc_step_target_pitch * sfx_step_tick)) /
                       sfx_spd_min;

    new_target_vol = ((prev_step_target_vol * step_ticks_remaining) +
                      (sfx_step_vol_256 * sfx_step_tick)) /
                     sfx_spd_min;

    *osc_step_target_pitch = new_target_pitch;
    *osc_step_target_vol = new_target_vol;

    break;
  }
  // Fade in
  case 4: {
    new_target_pitch = *osc_step_target_pitch;
    new_target_vol = (sfx_step_vol_256 * sfx_step_tick) / sfx_spd_min;

    *osc_step_target_vol = new_target_vol;

    break;
  }
  // Fade out
  case 5: {
    new_target_pitch = *osc_step_target_pitch;
    new_target_vol =
        (sfx_step_vol_256 * (sfx_spd_min - sfx_step_tick)) / sfx_spd_min;

    *osc_step_target_vol = new_target_vol;

    break;
  }
  // Arp fast
  case 6: {
    const int step_offset =
        ((sfx_tick / (is_lo_spd ? 2 : 4)) % 4) + (sfx_step & 0x1c);

    new_target_pitch = *(sfx_ptr + 0x14 + step_offset * 0x14) << 16;
    new_target_vol = sfx_step_vol_256;

    *osc_step_target_pitch = new_target_pitch;

    break;
  }
  // Arp slow
  case 7: {
    const int step_offset =
        ((sfx_tick / (is_lo_spd ? 4 : 8)) % 4) + (sfx_step & 0x1c);

    new_target_pitch = *(sfx_ptr + 0x14 + step_offset * 0x14) << 16;
    new_target_vol = sfx_step_vol_256;

    *osc_step_target_pitch = new_target_pitch;

    break;
  }
  // No effect, vibrato, drop
  default: {
    new_target_pitch = *osc_step_target_pitch;
    new_target_vol = sfx_step_vol_256;
  }
  }

  *(osc_state + 0x24) = sfx_step_pitch;
  *(osc_state + 0x28) = new_target_vol;

  int *osc_effect_ptr = osc_state + 0x44;

  *osc_effect_ptr = 0;

  int *osc_filter_ptr = osc_state + 0x48;

  *osc_filter_ptr = 0;

  /*
   * Apply Meta Instrument
   */
  const _Bool sfx_step_is_meta = *(sfx_step_ptr + 0x10);

  const int *cart_ptr = ch_state + 0x2020;

  int meta_inst_idx = sfx_step_waveform < 7 ? sfx_step_waveform : 7;
  meta_inst_idx = meta_inst_idx > 0 ? meta_inst_idx : 0;

  const int *meta_sfx_ptr = cart_ptr + 0x20 + meta_inst_idx * 680;
  const int loop_start = *(meta_sfx_ptr + 0xc);

  if (!sfx_step_is_meta) {
    *(ch_state + 0x2ee0) = 0;
  } else {
    if (sfx_step_tick == 0) {
      int meta_inst_step_len = 0;
      int meta_inst_tick_len = 0;

      _Bool is_meta = 0;

      // If wavetable frame
      if (loop_start & 128) {
        is_meta = 1;
      } else {
        const int loop_end = *(meta_sfx_ptr + 0x10);

        if (loop_end > loop_start) {
          meta_inst_tick_len = 0x1000000;
        } else {
          meta_inst_step_len =
              loop_start > 0 && loop_end == 0 ? loop_start : 32;
          is_meta = 1;
        }
      }

      if (is_meta) {
        int meta_inst_spd = *(meta_sfx_ptr + 8);
        meta_inst_spd = meta_inst_spd > 1 ? meta_inst_spd : 1;

        meta_inst_tick_len = meta_inst_spd * meta_inst_step_len;
      }

      if ((sfx_effect != 1 &&
           (sfx_step == 0 || sfx_step_pitch != *(ch_state + 0x2e90))) ||
          *(ch_state + 0x2ee0) >= meta_inst_tick_len) {
        if (sfx_effect == 3) {
          is_meta = 0;
        }
      } else if (sfx_effect != 3 && sfx_effect != 4) {
        is_meta = 0;
      }

      if (is_meta || sfx_step_waveform != *(ch_state + 0x2e94)) {
        *(ch_state + 0x2ee0) = 0;
      }
    }

    apply_meta_instrument(ch_state, sfx_step_ptr, osc_state);

    new_target_pitch = *osc_step_target_pitch;
  }

  /*
   * Phase increment (dt)
   */
  const int target_ch = *(ch_state + 0x203c);
  const int hi_filter_mask = 1 << (target_ch + 4);
  const int lo_filter_mask = 1 << target_ch;

  // Global octave mask (0x5f40)
  if (hi_filter_mask & audio_clock_mask) {
    new_target_pitch = new_target_pitch - 0xc0000;
    *osc_step_target_pitch = new_target_pitch;
  }

  const int new_target_pitch_partial = new_target_pitch & 0xffff;
  const int new_target_pitch_64 = new_target_pitch >> 16;
  const int new_target_pitch_64_clamped =
      new_target_pitch > -1 ? new_target_pitch_64
                            : 12 - (-new_target_pitch_64 % 12);

  const int new_scale_degree = new_target_pitch_64_clamped % 12;

  const int freq1 = *(note_dx + (new_scale_degree << 2)) *
                    (0x10000 - new_target_pitch_partial);

  const int freq2 =
      *(note_dx + ((new_scale_degree + 1) << 2)) * new_target_pitch_partial;

  int dt = (freq1 + freq2) / 22050;
  int octave_target = ((new_target_pitch_64 + 48) / 12) - 4;

  if (new_target_pitch < 0x240000) {
    while (octave_target < 3) {
      dt /= 2;
      octave_target += 1;
    }
  } else {
    while (octave_target > 3) {
      dt = dt << 1;
      octave_target -= 1;
    }
  }

  dt = dt < 0x8000 ? dt : 0x8000;
  dt = dt > 0x8 ? dt : 0x8;

  // *(osc_state + 8) = dt;

  /*
   * Vibrato
   */
  const int osc_effect = *osc_effect_ptr;
  const int vibratos[8] = {0, 1, 2, 1, 0, -1, -2, -1};

  int vibrato = 0;

  if (sfx_effect == 2 || osc_effect == 2) {
    vibrato = vibratos[(sfx_tick >> 1) & 7];

    if (sfx_effect == 2 && osc_effect == 2) {
      vibrato *= 2;
    }

    dt = (dt * (128 + vibrato)) >> 7;
  }

  *(osc_state + 8) = dt;

  /*
   * Drop
   */
  int drop_dt = dt;

  if (sfx_effect == 3 && !(sfx_step_is_meta && (loop_start & 128) == 0)) {
    drop_dt = (dt * (sfx_spd_min - sfx_step_tick)) / sfx_spd_min;
    *(osc_state + 8) = drop_dt;
  }

  if (osc_effect == 3) {
    drop_dt = (*(osc_state + 0x4c) * drop_dt) / 256;
    *(osc_state + 8) = drop_dt;
  }

  /*
   * Apply global music volume
   */
  if (*(ch_state + 0x2d28)) {
    const int new_target_vol = ((fade_vol >> 8) * *(osc_state + 0x1c)) / 256;

    *(osc_state + 0x1c) = (new_target_vol * music_volume) / 256;
  }

  /*
   * Filters
   */
  const int sfx_filter_byte = *sfx_ptr;
  const int osc_filter_byte = *osc_filter_ptr;
  const int sfx_detune = (sfx_filter_byte >> 3) % 3;
  const int osc_detune = (osc_filter_byte >> 3) % 3;
  const int detune = sfx_detune > osc_detune ? sfx_detune : osc_detune;

  *(osc_state + 0x50) = detune;

  const int sfx_buzz = (sfx_filter_byte >> 2) & 1;
  const int osc_buzz = (osc_filter_byte >> 2) & 1;
  const int buzz = sfx_buzz > osc_buzz ? sfx_buzz : osc_buzz;

  *(osc_state + 0x54) = buzz;

  const int sfx_noiz = (sfx_filter_byte >> 1) & 1;
  const int osc_noiz = (osc_filter_byte >> 1) & 1;
  const int noiz = (sfx_noiz >> 0) > (osc_noiz >> 0) ? sfx_noiz : osc_noiz;

  *(osc_state + 0x58) = noiz;

  /*
   * Detune
   */
  const int waveform = *osc_state;

  int detune_dt = *(osc_state + 8);
  int detune_amount = detune == 0 ? 256 : 255;

  if (waveform != 6 || noiz == 0) {
    if (waveform == 0) {
      if (detune == 1) {
        detune_amount = 193;
      } else if (detune == 2) {
        detune_amount = 384;
      }
    } else if (waveform < 6 && detune > 0) {
      // lower target volume
      *(osc_state + 0x1c) = (*(osc_state + 0x1c) * 5) / 4;
    } else if (waveform == 7) {
      if (detune == 0) {
        detune_amount = 254;
      } else if (detune == 1) {
        detune_amount = 250;
      } else if (detune == 2) {
        detune_amount = 508;
      }
    }
    // waveform == 6 && noiz != 0
  } else {
    // if total dampen == 2 or 3 (0x5f43 hi+lo)
    // 0x2edc = 0x2ee8 in p8 binary
    if (*(ch_state + 0x2edc) > 11) {
      // set noiz to 2
      *(osc_state + 88) = 2;

      // set dampen to 0
      // 0x2edc = 0x2ee8 in p8 binary
      *(ch_state + 0x2edc) = 0;
    }
  }

  *(osc_state + 0x10) = (detune_dt * detune_amount) / 256;

  /*
   * Reverb
   */
  const int sfx_reverb = sfx_ptr ? ((*sfx_ptr >> 3) / 3) % 3 : 0;

  int osc_reverb = ((*(ch_state + 0x2d78) >> 3) / 3) % 3;
  int reverb = sfx_reverb > osc_reverb ? sfx_reverb : osc_reverb;

  if (global_reverb & hi_filter_mask) {
    reverb = reverb > 1 ? reverb : 1;
  }

  if (global_reverb & lo_filter_mask) {
    reverb = reverb > 2 ? reverb : 2;
  }

  *(osc_state + 0x5c) = reverb;
}
