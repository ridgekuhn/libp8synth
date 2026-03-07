// NOTICE: This file is not accurate and only partially implemented

#include "./codo_load_pico8_cart_from_file.h"
#include "../globals.h"
#include "./cdata.h"
#include "./codo_create_pico8_cart.h"
#include "./load_music.h"
#include "./load_sfx.h"
#include "./verify_cart_header.h"
#include <regex.h>
#include <stdio.h>
#include <string.h>

/**
 * Load Pico-8 Cart from File
 */
CData *codo_load_pico8_cart_from_file(FILE *file_ptr, int unknown) {
  /*
   * Verify file
   */
  if (verify_cart_header(file_ptr) != 0) {
    return 0;
  }

  /*
   * Populate CData
   */
  // Line buffer
  char str[0x30001] = "";
  // Regex for section tags
  regex_t regex;
  regcomp(&regex, "^__[a-z]*__\0?$", 0);
  // Cart data struct
  CData *cdata = codo_create_pico8_cart();

  while (fgets(str, 0x30001, file_ptr)) {
    // If current line is section tag
    if (regexec(&regex, str, 0, NULL, 0) == 0) {
      if (strstr(str, "__sfx__")) {
        load_sfx(file_ptr, cdata);
      }

      if (strstr(str, "__music__")) {
        load_music(file_ptr, cdata);
      }
    }
  }

  return cdata;
}
