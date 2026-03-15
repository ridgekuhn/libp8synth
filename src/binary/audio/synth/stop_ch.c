/**
 * Stop Channel Playback
 */
void init_ch_state(int *ch_state) {
  *(ch_state + 0x2010) = 0;
  *(ch_state + 0x2020) = 0;
  *(ch_state + 0x2028) = 0;
  *(ch_state + 0x2034) = 0;
  *(ch_state + 0x2038) = 0;
  *(ch_state + 0x2d28) = 0;
}
