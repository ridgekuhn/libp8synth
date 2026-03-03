/**
 * Initialize channel state
 */
function init_ch_state(ch_state) {
  // 0x2010 = 0x2010 in p8 binary
  c[(ch_state + 0x2010) >> 2] = 0;
  // 0x201c = 0x2020 in p8 binary
  c[(ch_state + 0x201c) >> 2] = 0;
  // 0x2028 = 0x2034 in p8 binary
  c[(ch_state + 0x2028) >> 2] = 0;
  // 0x202c = 0x2038 in p8 binary
  c[(ch_state + 0x202c) >> 2] = 0;
  // 0x2020 = 0x2028 in p8 binary
  c[(ch_state + 0x2020) >> 2] = 0;
  // 0x2d1c = 0x2d28 in p8 binary
  c[(ch_state + 0x2d1c) >> 2] = 0;
}
