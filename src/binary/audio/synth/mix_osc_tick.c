#include "./mix_osc_tick.h"
#include "./mix_reverb.h"
#include "./mixers/aliased/mix_noise.h"
#include "./oscillators/aliased/osc_aliased_wavetable.h"
#include "./oscillators/hq/osc_hq_organ.h"
#include "./oscillators/hq/osc_hq_pulse.h"
#include "./oscillators/hq/osc_hq_sawtooth.h"
#include "./oscillators/hq/osc_hq_square.h"
#include "./oscillators/hq/osc_hq_tilted.h"
#include "./oscillators/hq/osc_hq_triangle.h"
#include <string.h>

/**
 * Mix oscillator tick
 *
 * Mix oscillator state to mixer channel buffer
 *
 * @see mix_wavetable()
 * @see mix_triangle()
 * @see mix_supersaw()
 * @see mix_sawtooth()
 * @see mix_pulse()
 * @see mix_organ()
 * @see mix_noise()
 * @see mix_reverb()
 *
 * @param osc_state Mixer channel oscillator state
 * @param tick_buffer Mixer channel tick buffer
 * @param chunk_len Length of tick_buffer in samples
 * @param ch_state Mixer channel state
 */
void mix_osc_tick(int *osc_state, short *tick_buffer, int chunk_len,
                  long ch_state) {
  // Nothing to do
  if (chunk_len <= 0) {
    return;
  }

  /*
   * Oscillator state
   */
  const int osc_vol = osc_state[7];
  const int waveform = *osc_state;

  // If no audio to mix, zero-out buffer and return early
  // 0x2ee4 = 0x2ed8 (-0xc) in asm.js export
  if (osc_vol == 0 && *(int *)(ch_state + 0x2ee4) == 0) {
    osc_state[1] = 0;
    memset(tick_buffer, 0, chunk_len << 1);
    return;
  }

  /*
   * Select and mix oscillator
   */
  // Noise waveform
  if (waveform == 6) {
    mix_noise(osc_state, tick_buffer, chunk_len);
  }

  int t = osc_state[1];
  int detune_t = osc_state[3];

  for (int i = 0; i < chunk_len; i += 1) {
    int sample = 0;

    // Osc selection must be made this way for patch function
    // @TODO Make separate function using jump array for standalone binary
    if (waveform == 1) {
      sample = osc_hq_tilted(osc_state, t, detune_t);
    } else if (waveform == 2) {
      sample = osc_hq_sawtooth(osc_state, t, detune_t);
    } else if (waveform == 3) {
      sample = osc_hq_square(osc_state, t, detune_t);
    } else if (waveform == 4) {
      sample = osc_hq_pulse(osc_state, t, detune_t);
    } else if (waveform == 5) {
      sample = osc_hq_organ(osc_state, t, detune_t);
    } else if (waveform == 8) {
      sample = osc_aliased_wavetable(osc_state, t, detune_t);
    } else {
      sample = osc_hq_triangle(osc_state, t, detune_t);
    }

    tick_buffer[i] = (short)sample;

    t = (t + osc_state[2]) & 0xffff;
    detune_t = (detune_t + osc_state[4]) & 0x1ffff;
  }

  osc_state[1] = t;
  osc_state[3] = detune_t;

  /*
   * Apply buffer reverb
   */
  mix_reverb(osc_state, tick_buffer, chunk_len, ch_state);
}
