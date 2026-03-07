#include "./codo_load_pico8_cart.h"
#include "./cdata.h"
#include "./codo_load_pico8_cart_from_file.h"
#include <stdio.h>

/**
 * Load Pico-8 Cart
 *
 * @returns Pointer to CData
 */
CData *codo_load_pico8_cart(char *file_name) {
  CData *cdata = 0;

  printf("Loading %s ...\n", file_name);

  FILE *file_ptr = fopen(file_name, "r");

  if (file_ptr != 0) {
    cdata = codo_load_pico8_cart_from_file(file_ptr, 0);
  }

  fclose(file_ptr);

  return cdata;
}
