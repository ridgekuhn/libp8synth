#include "./verify_cart_header.h"
#include "../globals.h"
#include <stdio.h>
#include <string.h>

int verify_cart_header(FILE *file_ptr) {
  /*
   * Pico-8 cart identifier
   */
  char header_str[80] = "";

  fgets(header_str, 80, file_ptr);

  if (strstr(header_str, "pico-8 cartridge") == 0) {
    fprintf(stderr, "ERROR: Invalid cart format\n");
    return 1;
  }

  /*
   * Version header
   */
  char version_str[11] = "";

  fgets(version_str, 11, file_ptr);

  if (strstr(version_str, "version")) {
    int version = 0;

    sscanf(version_str, "version %d", &version);

    if (version > g_version) {
      fprintf(stderr,
              "WARNING: Cart is version %d. Max supported version is %d.\n",
              version, g_version);
    }
  }

  return 0;
}
