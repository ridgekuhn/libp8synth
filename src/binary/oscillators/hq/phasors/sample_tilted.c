#include <math.h>

/**
 * Tilted Sawtooth Sample
 *
 * @param freq Frequency
 * @param t Oscillator phase, 0 <= t < 0x10000
 * @param duty_cycle Duty cycle len, 1 <= duty_cycle < 0xffff
 */
int sample_tilted(int freq, int t, int duty_cycle) {
  if (freq == 0) {
    return 0;
  }

  // Offset phase by 1/2 duty_cycle
  const double new_t = (t - (duty_cycle / 2.0)) / 0x10000;
  const double radians = new_t * 2 * M_PI;
  const double duty_radians = ((double)duty_cycle / 0x10000) * M_PI;

  double amplitude = 0;
  double normalize = 0;

  // 10914 = 11025 * 0.99
  for (int i = 1; i * freq < 10914; i += 1) {
    const double duty_angle = i * duty_radians;
    const double magnitude = sin(duty_angle) / (duty_angle * duty_angle);

    amplitude -= magnitude * sin(i * radians);
    normalize += magnitude;
  }

  return (amplitude / normalize) * 0x10000;
}
