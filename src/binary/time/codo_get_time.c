#include <SDL.h>
#include <stdint.h>

/**
 * Get SDL ticks
 */
uint32_t codo_get_time() { return SDL_GetTicks(); }
