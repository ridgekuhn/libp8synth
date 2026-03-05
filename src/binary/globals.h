#include <stdint.h>

#ifndef GLOBALS
#define GLOBALS

/*
 * Codo state
 */
struct CodoState {
  // +0
  int32_t UNKNOWN_0[0x97];
  // +604
  int32_t is_paused;
  // +608
  int32_t UNKNOWN_1;
  // +612
  int32_t audio_playing;
  // +616
  int32_t UNKNOWN_616[5];
  // +632
  int32_t codo_audio_buffer_init;
  // +636
  int32_t patterns_played;
  // +640
  int32_t cur_pat_idx;
  // +644
  int32_t UNKNOWN_644[4];
  // +660
  int32_t prog_sample_rate;
  // +664
  int32_t prog_channels;
  // +668
  int32_t sdl_sample_rate;
  // +672
  int32_t sdl_channels;
};

typedef struct CodoState CodoState;

/*
 * Variables
 */
extern CodoState codo_state;
extern int m_low;
extern int m_high;
extern int SAMPLES_PER_TICK;

#endif
