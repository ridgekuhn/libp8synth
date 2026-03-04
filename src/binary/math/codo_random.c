#include "./codo_random.h"
#include "../globals.h"

long codo_random(int n) {
  long result = 0;

  if (n != 0) {
    m_high = (m_high << 0x10 | m_high >> 0x10) + m_low;
    m_low = m_low + m_high;
    result = (long)m_high % (long)n;
  }

  return result;
}
