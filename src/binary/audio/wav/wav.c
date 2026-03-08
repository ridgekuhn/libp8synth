#include "./wav.h"
#include <string.h>

/**
 * Make wav header
 */
WavHeader make_wav_header(int data_len, int sample_rate, int sample_depth,
                          int channels) {
  WavHeader h;

  /*
   * RIFF
   */
  // byte 0
  strncpy(h.chunk_id, "RIFF", 4);
  // byte 8
  strncpy(h.format, "WAVE", 4);

  /*
   * WAVE
   */
  // byte 12
  strncpy(h.subchunk1_id, "fmt ", 4);
  // byte 16
  h.subchunk1_len = 16;
  // byte 20
  h.format_id = 1;
  // byte 22
  h.channels = channels ? channels : 1;
  // byte 24
  h.sample_rate = sample_rate ? sample_rate : 44100;
  // byte 28
  h.byte_rate = sample_rate * sample_depth / 8 * channels;
  // byte 32
  h.block_align = sample_depth / 8 * channels;
  // byte 34
  h.sample_depth = sample_depth ? sample_depth : 16;

  /*
   * Data
   */
  // byte 36
  strncpy(h.subchunk2_id, "data", 4);
  // byte 40
  h.subchunk2_len = data_len;

  // byte 4
  h.chunk_len = 4 + (8 + h.subchunk1_len) + (8 + data_len);

  return h;
}
