#include "./phasor_hq_organ.h"
#include "./phasor_hq_triangle.h"

/**
 * Organ Sample
 *
 * @param freq Frequency
 * @param t Oscillator phase, 0 <= t < 0x10000
 */
double phasor_hq_organ(int t, int freq) {
  if (freq == 0) {
    return 0;
  }

  const double tri1 = phasor_hq_triangle(t * 2, freq * 2);
  const double tri2 = phasor_hq_triangle((t + 0xc000) & 0xffff, freq) * 0.45;

  return (tri1 - tri2) * 0.82;
}
