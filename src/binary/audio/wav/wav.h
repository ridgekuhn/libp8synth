#ifndef WAV
#define WAV

#include <stdint.h>

struct WavHeader {
  char chunk_id[4];
  int32_t chunk_len;
  char format[4];
  char subchunk1_id[4];
  int32_t subchunk1_len;
  int16_t format_id;
  int16_t channels;
  int32_t sample_rate;
  int32_t byte_rate;
  int16_t block_align;
  int16_t sample_depth;
  char subchunk2_id[4];
  int32_t subchunk2_len;
};

typedef struct WavHeader WavHeader;

struct WavHeader make_wav_header(int data_len, int sample_rate,
                                 int sample_depth, int channels);

#endif
