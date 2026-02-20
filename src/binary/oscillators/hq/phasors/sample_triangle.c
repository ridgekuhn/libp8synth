#include <math.h>

/**
 * Triangle Sample
 *
 * @param freq Frequency
 * @param t Oscillator phase, 0 <= t < 0x10000
 */
int sample_triangle(int freq, int t) {
  if (freq == 0) {
    return 0;
  }

  // Offset phase by 1/4 cycle
  const double radians = ((double)(t + 0x4000) / 0x10000) * 2 * M_PI;

  double amplitude = 0;
  double normalize = 0;

  // 10914 = 11025 * 0.99
  for (int i = 0; (i * freq) < 10914; i += 1) {
    const int n = 2 * i + 1;
    const double magnitude = (i & 1 ? -1.0 : 1.0) / (n * n);

    amplitude -= magnitude * sin(n * radians);
    normalize += magnitude;
  }

  return (amplitude / normalize) * 0x100000;
}
