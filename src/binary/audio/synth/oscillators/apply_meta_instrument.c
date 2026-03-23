#include "./apply_meta_instrument.h"
#include "./calculate_osc_wavetable.h"

/**
 * Apply Meta Instrument
 */
void apply_meta_instrument(long ch_state, long sfx_step, int *osc_state) {
  long *cart_ptr = &ch_state + 0x2020;

  if (*cart_ptr == 0) {
    return;
  }

  /*
   * Filter
   */
  int meta_idx = *(&sfx_step + 4);
  meta_idx = meta_idx < 7 ? meta_idx : 7;
  meta_idx = meta_idx > 0 ? meta_idx : 0;

  const long *meta_sfx_ptr = cart_ptr + 0x20 + meta_idx * 680;

  *(osc_state + 0x48) = *meta_sfx_ptr;

  /*
   * Wavetable frames
   */
  const int loop_start = *(meta_sfx_ptr + 0xc);

  // is wavetable
  if (loop_start & 0x80) {
    calculate_osc_wavetable(cart_ptr, osc_state, meta_idx);

    *(osc_state + 0x44) = 0;

    return;
  }

  /*
   * Oscillator Meta Instrument State
   */
  int spd = *(meta_sfx_ptr + 8);
  spd = spd > 1 ? spd : 1;

  const int loop_end = *(meta_sfx_ptr + 0x10);

  long *cur_pat_tick = &ch_state + 0x2ee0;
  int meta_tick = 0;

  if (loop_end <= loop_start || *cur_pat_tick < (spd * loop_end)) {
    meta_tick = *cur_pat_tick;
  } else {
    meta_tick = spd * loop_start;
    *cur_pat_tick = meta_tick;
  }

  int step = meta_tick / spd;
  step = step < 31 ? step : 31;
  step = step > 0 ? step : 0;

  const int step_tick = meta_tick - (step * spd);
  const int loop_len = loop_start > 0 && loop_end == 0 ? loop_start : 32;

  if (step > 31 || step >= loop_len) {
    *(osc_state + 0x1c) = 0;
    *osc_state = 0;

    return;
  }

  const long *step_ptr = meta_sfx_ptr + 0x14 + step * 0x14;
  const int meta_pitch = *step_ptr;
  const int meta_target_pitch = meta_pitch << 16;
  const int meta_vol = *(step_ptr + 8);
  const int meta_target_vol = meta_vol << 8;

  if (step_tick == spd - 1) {
    *(&ch_state + 0x2ea4) = meta_pitch;
    *(&ch_state + 0x2ea8) = *(step_ptr + 4);
    *(&ch_state + 0x2eac) = meta_vol;
  }

  /*
   * Apply Effects
   */
  const int meta_effect = *(step_ptr + 0xc);
  const int fast_spd = spd < 9;

  int new_target_pitch = 0;
  int new_target_vol = 0;

  switch (meta_effect) {
  // slide
  case 1: {
    int slide_target_pitch = 0;
    int slide_target_vol = 0;

    if (step > 0) {
      slide_target_pitch = *(&ch_state + 0x2ea4) << 16;
      slide_target_vol = *(&ch_state + 0x2eac) << 8;
    } else {
      slide_target_pitch = 0x180000;
      slide_target_vol = meta_target_vol;
    }

    const int ticks_remaining = spd - step_tick;

    new_target_pitch = ((slide_target_pitch * ticks_remaining) +
                        (step_tick << 16 * meta_pitch)) /
                       spd;

    new_target_vol =
        ((slide_target_vol * ticks_remaining) + (meta_target_vol * step_tick)) /
        spd;

    break;
  }
  // drop
  case 3: {
    new_target_pitch = meta_target_pitch;
    new_target_vol = meta_target_vol;

    *(osc_state + 0x4c) = ((spd - step_tick) << 8) / spd;

    break;
  }
  // fade in
  case 4: {
    new_target_pitch = meta_target_pitch;
    new_target_vol = (meta_target_vol * step_tick) / spd;

    break;
  }
  // fade out
  case 5: {
    new_target_pitch = meta_target_pitch;
    new_target_vol = (meta_target_vol * (spd - step_tick)) / spd;

    break;
  }
  // arp fast
  case 6: {
    const int arp_spd = fast_spd ? 2 : 4;
    const int arp_step = ((meta_tick / arp_spd) % 4) + (step & 0x1c);

    new_target_pitch = *(meta_sfx_ptr + 0x14 + arp_step * 0x14) << 16;

    new_target_vol = meta_target_vol;

    break;
  }
  // arp slow
  case 7: {
    const int arp_spd = fast_spd ? 4 : 8;
    const int arp_step = ((meta_tick / arp_spd) % 4) + (step & 28);

    new_target_pitch = *(meta_sfx_ptr + 0x14 + arp_step * 0x14) << 16;

    new_target_vol = meta_target_vol;

    break;
  }
  default: {
    new_target_pitch = meta_target_pitch;
    new_target_vol = meta_target_vol;
  }
  }

  /*
   * Nested Wavetable Frames
   */
  int meta_waveform = *(step_ptr + 4);
  meta_waveform = meta_waveform < 7 ? meta_waveform : 7;
  meta_waveform = meta_waveform > 0 ? meta_waveform : 0;

  if (
      // step has meta instrument bit
      *(step_ptr + 0x10) != 0 &&
      // step is wavetable
      (*(cart_ptr + 0x10 + meta_waveform * 680 + 0xc) & 0x80) != 0) {
    *(osc_state + 0x48) = *(cart_ptr + 0x10 + meta_waveform * 680);

    calculate_osc_wavetable(cart_ptr, osc_state, meta_waveform);
  } else {
    *osc_state = meta_waveform;
  }

  /*
   * Oscillator State
   */
  *(osc_state + 0x20) += new_target_pitch - 0x180000;
  *(osc_state + 0x24) += meta_pitch - 24;
  *(osc_state + 0x28) = (new_target_vol * 7) / (meta_vol > 1 ? meta_vol : 1);

  int *osc_target_vol = osc_state + 0x1c;

  *osc_target_vol = (*osc_target_vol * new_target_vol) / 1792;
  *(osc_state + 0x44) = meta_effect;
}
