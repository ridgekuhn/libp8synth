#include <stdint.h>

/*
 * Mixer channel step state
 */
struct StepState {
  // +0x0000
  int32_t sfx_ptr;
  // +0x0004
  int32_t UNKNOWN_0x4;
  // +0x0008
  int32_t cur_sfx_tick;
  // +0x000c
  int32_t cur_step_tick;
  // +0x0010
  int32_t spd;
  // +0x0014
  int32_t target_ch;
  // +0x0018
  int32_t UNKNOWN_0x18;
  // +0x001c
  int32_t UNKNOWN_0x1c[3];
  // +0x0028
  int32_t pitch;
  // +0x0030
  int32_t oscillator;
  // +0x0038
  int32_t vol;
  // +0x0040
  int32_t effect;
};

typedef struct StepState StepState;

/*
 * Mixer channel pattern state
 */
struct PatternState {
  // +0x0000
  int32_t cur_pat_tick;
  // +0x0004
  int32_t samples_remaining;
  // +0x0008
  int32_t enabled;
  // +0x000c
  int32_t cur_pat_idx;
};

typedef struct PatternState PatternState;

/*
 * Mixer channel oscillator state
 */
struct OscState {
  // +0x0000
  int32_t osc;
  // +0x0004
  int32_t t;
  // +0x0008
  int32_t dt;
  // +0x000c
  int32_t detune_t;
  // +0x0010
  int32_t detune_dt;
  // +0x0014
  int32_t UNKNOWN_0x14;
  // +0x0018
  int32_t base_slide_pitch;
  // +0x001c
  int32_t target_vol;
  // +0x0020
  int32_t target_pitch;
  // +0x0024
  int32_t cur_pitch;
  // +0x0028
  int32_t cur_vol;
  // +0x002c
  int32_t noise_seed;
  // +0x0030
  int32_t noise_acc_1;
  // +0x0034
  int32_t noise_acc_2;
  // +0x0038
  int32_t sfx_step_vol;
  // +0x003c
  int32_t sfx_step_pitch;
  // +0x0040
  int32_t sfx_step_osc;
  // +0x0044
  int32_t meta_effect;
  // +0x0048
  int32_t meta_filter_byte;
  // +0x004c
  int32_t drop_dt;
  // +0x0050
  int32_t detune;
  // +0x0054
  int32_t buzz;
  // +0x0058
  int32_t noiz;
  // +0x005c
  int32_t reverb;
  // +0x0060
  int32_t wavetable_samples[0x40];
};

typedef struct OscState OscState;

/*
 * Channel state history entry
 */
struct ChStateHistory {
  //+0x0000
  int32_t sfx_tick;
  //+0x0004
  int32_t step_tick;
  //+0x0008
  int32_t sfx_ptr;
  //+0x000c
  int32_t UNKNOWN_0xc;
  //+0x0010
  int32_t step_idx;
  //+0x0014
  int32_t pat_idx;
  //+0x0018
  int32_t patterns_played;
  //+0x001c
  int32_t UNKNOWN_0x1c;
};

typedef struct ChStateHistory ChStateHistory;

/*
 * Mixer channel state
 */
struct ChState {
  // +0x0000
  int8_t mixdown_buffer[0x2000];
  // +0x2000
  int32_t UNKNOWN_0x2000[4];
  // +0x2010
  int32_t vox_ch_ptr;
  // +0x2014
  int32_t UNKNOWN_0x2014[2];
  // +0x201c
  int32_t vox_samples_remaining;
  // +0x2020
  int32_t cart_ptr;
  // +0x2024
  int32_t UNKNOWN_0x2024;
  // +0x2028
  struct StepState;
  // +0x206c
  int8_t multiuse[0xcb4];
  // +0x2d20
  struct PatternState;
  // +0x2d30
  struct OscState;
  // +0x2e90
  int32_t prev_tick_pitch;
  // +0x2e94
  int32_t prev_tick_osc;
  // +0x2e98
  int32_t prev_tick_vol;
  // +0x2e9c
  int32_t UNKNOWN_0x2e9c[2];
  // +0x2ea4
  int32_t meta_target_pitch;
  // +0x2ea8
  int32_t meta_osc;
  // +0x2eac
  int32_t meta_target_vol;
  // +0x2eb0
  int32_t UNKNOWN_0x2eb0[0xc];
  // +0x2ee0
  int32_t cur_pat_tick;
  // +0x2ee4
  int32_t total_reverb;
  // +0x2ee8
  int32_t total_dampen;
  // +0x2eec
  int32_t pat_ticks_remaining;
  // +0x2ef0
  int32_t cur_history_idx;
  // +0x2ef4
  int32_t prev_mix_expire;
  // +0x2ef8
  int32_t mix_expire;
  // +0x2efc
  int32_t UNKNOWN_0x2efc;
  // +0x2f00
  struct ChStateHistory;
};

typedef struct ChState ChState;
