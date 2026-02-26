#include <math.h>

/**
 * Square Sample
 *
 * @param freq Frequency
 * @param t Oscillator phase, 0 <= t < 0x10000
 */
int sample_square(int freq, int t) {
  if (freq == 0) {
    return 0;
  }

  const double radians = ((double)t / 0x10000) * 2 * M_PI;

  double amplitude = 0;
  double normalize = 0;

  // 9922 = 11025 * 0.90
  for (int i = 1; (i * freq) < 9922; i += 2) {
    const double magnitude = 1.0 / i;

    amplitude -= magnitude * sin(i * radians);
    normalize += magnitude;
  }

  return (amplitude / normalize) * 0x100000;
}
