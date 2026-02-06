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

	const double new_t = t + 0xa000;
  const double radians = (new_t / 0x10000) * 2 * M_PI;
  const double radians2 = (new_t * 2 / 0x10000) * 2 * M_PI;

  double amplitude = 0;
  double normalize = 0;

  // 10914 = 11025 * 0.99
  for (int i = 0; (i * freq * 2) < 10914; i += 1) {
    const int n = 2 * i + 1;
    const double magnitude = (i & 1 ? -1.0 : 1.0) / (n * n);

    amplitude -= magnitude * sin(n * radians) / 2;
    amplitude -= magnitude * sin(n * radians2);
    normalize += magnitude * 1.5;
  }

  return (amplitude / normalize) * 0x10000;
}
