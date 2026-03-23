#include "./cart/cdata.h"
#include <stdbool.h>
#include <stdint.h>

/*
 * Named globals
 */
#ifndef advanced_pattern
extern _Bool advanced_pattern;
#endif

#ifndef codo_state
extern int codo_state[172];
#endif

#ifndef m_low
extern int m_low;
#endif

#ifndef m_high
extern int m_high;
#endif

#ifndef fade_vol
extern int fade_vol;
#endif

/*
 * Custom globals
 */
#ifndef GLOBALS
#define GLOBALS

extern int audio_clock_mask;
extern int bytes_per_tick;
extern int global_reverb;
extern int music_volume;
extern CData *g_cdata;
extern int g_version;
extern int SAMPLES_PER_TICK;

#endif
