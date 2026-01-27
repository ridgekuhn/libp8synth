#include <math.h>

/**
 * Mix sawtooth oscillator
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in samples
 */
void mix_sawtooth(int *osc_state, short *chunk_buffer, int chunk_len) {
  /*
   * Oscillator state
   */
  const _Bool osc_buzz = osc_state[21] != 0;
  const int osc_phase = osc_state[1];
  const int osc_phase_detuned = osc_state[3];
  const int osc_phase_inc = osc_state[2];
  const int osc_vol = osc_state[7];
  const int osc_detune_phase_inc = osc_state[4];
  const int osc_detune = osc_state[20];
  const int osc_detune_m1 = osc_detune == 2 ? 1 : 0;
  const int osc_detune_phase =
      osc_phase_inc == osc_detune_phase_inc ? osc_phase : osc_phase_detuned;
  const int osc_amplitude = (osc_vol * 3) / 2;

  /*
   * Buffer Constants
   */
  const double TWO_PI = M_PI * 2;
  const int freq = (osc_phase_inc * 22050) >> 16;
  const int detune_freq = (osc_detune_phase_inc * 22050) >> 16;

  double normalize = 0;

  // 10914 = 11025 * 0.99
  for (int i = 1; (i * freq / 2) < 10914; i += 1) {
    if ((i & 1) == 0) {
      normalize += 1.0 / ((double)i / 2);
    }

    if (osc_buzz) {
      normalize += 1.0 / i / 2;
    }
  }

  double detune_normalize = 0;

  for (int i = 1; (i * detune_freq / 2) < 10914; i += 1) {
    if ((i & 1) == 0) {
      detune_normalize += 1.0 / ((double)i / 2);
    }

    if (osc_buzz) {
      detune_normalize += 1.0 / i / 2;
    }
  }

  if (osc_buzz) {
    normalize /= 0.75;
    detune_normalize /= 0.75;
  }

  /*
   * Populate buffer
   */
  int cur_phase = osc_phase;
  int cur_detune_phase = osc_detune_phase;

  for (int i = 0; i < chunk_len; i += 1) {
    const double radians = (((double)cur_phase / 0x10000) * TWO_PI);

    /*
     * Primary phasor
     */
    double amplitude = 0;

    // 10914 = 11025 * 0.99
    for (int j = 1; (j * freq) < 10914; j += 1) {
      amplitude -= (1.0 / j) * sin(j * radians);
    }

    /*
     * Detune phasor
     */
    const int detune_phase = cur_detune_phase << osc_detune_m1;
    const int detune_partial = detune_phase & 0xffff;
    const double detune_radians = (((double)detune_partial / 0x10000) * TWO_PI);

    double detune_amplitude = 0;

    for (int j = 1; (j * detune_freq) < 10914; j += 1) {
      detune_amplitude -= (1.0 / j) * sin(j * detune_radians);
    }

    /*
     * Buzz phasors
     *
     * If buzz enabled, overlay additional saw waves, pitched down one octave
     */
    if (osc_buzz) {
      // Primary phasor
      const double buzz_radians =
          ((((double)cur_phase / 2) / 0x10000) * TWO_PI);

      for (int j = 1; (j * (freq / 2)) < 10914; j += 1) {
        amplitude -= (1.0 / j / 2) * sin(j * buzz_radians);
      }

      // Detune phasor
      const double buzz_detune_radians =
          ((((double)detune_phase / 2) / 0x10000) * TWO_PI);

      for (int j = 1; (j * (freq / 2)) < 10914; j += 1) {
        detune_amplitude -= (1.0 / j / 2) * sin(j * buzz_detune_radians);
      }
    }

    /*
     * Mix sample
     */
    amplitude = (amplitude / normalize) * 0x8000;
    detune_amplitude = (detune_amplitude / detune_normalize) * 0x4000;

    // Write new sample
    const int s_pregain = amplitude + detune_amplitude;
    const int s = (s_pregain * osc_amplitude) / 3072;
    chunk_buffer[i] = (short)s;

    // Increment phase
    cur_phase = (cur_phase + osc_phase_inc) & 0xffff;
    cur_detune_phase = (cur_detune_phase + osc_detune_phase_inc) & 0x1ffff;
  }

  // Update oscillator state
  osc_state[1] = cur_phase;
  osc_state[3] = cur_detune_phase;
}
