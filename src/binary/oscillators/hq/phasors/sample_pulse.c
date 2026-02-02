#include <math.h>

/**
 * Pulse Sample
 *
 * @param freq Frequency
 * @param t Oscillator phase, 0 <= t < 0x10000
 * @param duty_cycle Duty cycle len, 1 <= duty_cycle < 0xffff
 */
int sample_pulse(int freq, int t, int duty_cycle) {
  if (freq == 0) {
    return 0;
  }

  const double radians = ((double)t / 0x10000) * 2 * M_PI;
  const double duty_radians = ((double)(t - duty_cycle) / 0x10000) * 2 * M_PI;

  double amplitude = 0;
  double normalize = 0;

  // 10914 = 11025 * 0.99
  for (int i = 1; i * freq < 10914; i += 1) {
    const double magnitude = 1.0 / i;
    const double saw1 = magnitude * sin(i * radians);
    const double saw2 = magnitude * sin(i * duty_radians);

    amplitude -= saw1 - saw2;
    normalize += magnitude;
  }

  return (amplitude /normalize) * 0x10000;
}
