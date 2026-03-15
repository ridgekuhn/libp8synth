#include "./stop_osc.h"

/**
 * Stop Oscillator Playback
 */
void stop_osc(int *osc_state) {
  *osc_state = 0;
  *(osc_state + 8) = 0;
  *(osc_state + 0x1c) = 0;
}
