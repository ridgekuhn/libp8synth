#include "../globals.h"
#include "./arg_parser.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void main_init(int argc, char **argv) {
  Input input = arg_parser(argc, argv);

  float sample_multiplier = codo_state.prog_sample_rate / 22050.0;

  if (codo_state.prog_sample_rate < 22050 || fmod(sample_multiplier, 1) != 0) {
    printf("ERROR: Sample rate must be a multiple of 22050");
    exit(1);
  }

  SAMPLES_PER_TICK = 183 * sample_multiplier;
}
