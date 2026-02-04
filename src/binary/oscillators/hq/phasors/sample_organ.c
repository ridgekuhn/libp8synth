#include <math.h>

/**
 * Organ Sample
 *
 * @todo this is not exactly accurate
 *
 * @param freq Frequency
 * @param t Oscillator phase, 0 <= t < 0x10000
 */
int sample_organ(int freq, int t) {
  if (freq == 0) {
    return 0;
  }

  const double radians = ((double)t / 0x10000) * 2 * M_PI;
  const double radians2 = ((double)t * 2 / 0x10000) * 2 * M_PI;

  double amplitude = 0;
  double normalize = 0;

  // 10914 = 11025 * 0.99
  for (int i = 0; (i * freq * 2) < 10914; i += 1) {
    const int n = 2 * i + 1;
    const double magnitude = pow(-1.0, i) / (n * n);

    amplitude -= magnitude * sin(n * radians) / 2;
    amplitude -= magnitude * sin(n * radians2);
    normalize += magnitude * 1.5;
  }

  return (amplitude / normalize) * 0x10000;
}
