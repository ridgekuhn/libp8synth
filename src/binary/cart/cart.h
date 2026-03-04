#include <stdint.h>

/*
 * Cart Data Step
 */
struct CDataStep {
  // +0x0000
  int32_t pitch;
  // +0x0004
  int32_t oscillator;
  // +0x0008
  int32_t vol;
  // +0x000c
  int32_t effect;
  // +0x0010
  int32_t use_meta;
};

typedef struct CDataStep CDataStep;

/*
 * Cart Data SFX
 */
struct CDataSFX {
  // +0x0000
  int32_t filter_byte;
  // +0x0004
  int32_t editor_byte;
  // +0x0008
  int32_t spd;
  // +0x000c
  int32_t loop_start;
  // +0x0010
  int32_t loop_end;
  // +0x0014
  struct CDataStep step[32];
};

typedef struct CDataSFX CDataSFX;

/*
 * Cart Data
 */
struct CData {
  // +0x0000
  int32_t UNKNOWN_0[8];
  // +0x0020
  struct CDataSFX sfx[64];
  // +0xaa20
  int32_t pattern[64][4];
  // +0xab20
  int32_t UNKNOWN_0xab20[0xc0];
  // +0xae20
  int32_t pattern_loop_mask[64][4];
};

typedef struct CData CData;
