#include "./cli/main_init.h"
#include "./globals.h"
#include <stdlib.h>

/**
 * Main program
 */
int main(int argc, char **argv) {
  /*
   * Init
   */
  main_init(argc, argv);

  /*
   * De-init
   */
  free(g_cdata);
}
