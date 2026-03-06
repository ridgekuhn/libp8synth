#include "./arg_parser.h"
#include "../globals.h"
#include <argp.h>
#include <inttypes.h>

static char doc[] = "libz8synth";
static char args_doc[] = "input_file";

static struct argp_option options[] = {
    {0},
};

static error_t parser(int key, char *arg, struct argp_state *state) {
  Input *input = state->input;

  switch (key) {
  case ARGP_KEY_ARG:
    if (state->arg_num >= 1) {
      argp_usage(state);
    }

    input->args[state->arg_num] = arg;

    break;
  case ARGP_KEY_END:
    if (state->arg_num < 1) {
      argp_usage(state);
    }

    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

static struct argp argp = {
    .options = options, .parser = parser, .args_doc = args_doc, .doc = doc};

/**
 * CLI Argument Parser
 *
 * @returns argp Input struct
 */
Input arg_parser(int argc, char **argv) {
  Input input = {
      // default values
  };

  argp_parse(&argp, argc, argv, 0, 0, &input);

  return input;
}
