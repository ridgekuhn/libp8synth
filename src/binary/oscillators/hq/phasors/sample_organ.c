// #include "./sample_triangle.c"

/**
 * Organ Sample
 *
 * @param freq Frequency
 * @param t Oscillator phase, 0 <= t < 0x10000
 */
int sample_organ(int freq, int t) {
  if (freq == 0) {
    return 0;
  }

  const int tri1 = sample_triangle(freq * 2, t * 2);
  const int tri2 = sample_triangle(freq, (t + 0xc000) & 0xffff) * 0.45;
  return (tri1 - tri2) * 0.82;
}
