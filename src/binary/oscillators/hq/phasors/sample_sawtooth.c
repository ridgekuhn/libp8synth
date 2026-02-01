#include <math.h>

/**
 * Sawtooth Sample
 *
 * @param freq Frequency
 * @param t Oscillator phase, 0 <= t < 0x10000
 */
int sample_sawtooth(int freq, int t) {
  if (freq == 0) {
    return 0;
  }

  const double radians = ((double)t / 0x10000) * 2 * M_PI;

  double amplitude = 0;
  double normalize = 0;

  // 10914 = 11025 * 0.99
  for (int i = 1; (i * freq) < 10914; i += 1) {
    const double magnitude = 1.0 / i;

    amplitude -= magnitude * sin(i * radians);
    normalize += magnitude;
  }

  return (amplitude / normalize) * 0x10000;
}
