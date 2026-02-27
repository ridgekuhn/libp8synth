/**
 * Disable oscillator
 */
function disable_osc(osc_state) {
	c[osc_state >> 2] = 0;
	c[(osc_state + 8) >> 2] = 0;
	c[(osc_state + 0x1c) >> 2] = 0;
}
