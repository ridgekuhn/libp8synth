struct Input {
  char *args[1];
};

typedef struct Input Input;

Input arg_parser(int argc, char **argv);
