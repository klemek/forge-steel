#include "args.h"
#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help(int status_code) {
  puts(PACKAGE " " VERSION "\n\n"
               "usage: " PACKAGE " "
               "[--help] "
               "[-v] "
               "\n\n"
               "Fusion Of Real-time Generative Effects.\n\n"
               "options:\n"
               "  --help             show this help message and exit\n"
               "  -v, --version      print version\n");
  exit(status_code);
}

void invalid_arg(char *arg) {
  fprintf(stderr, "invalid argument: '%s'\n\n", arg);
  print_help(EXIT_FAILURE);
}

bool is_arg(char *arg, char *ref) { return strcoll(arg, ref) == 0; }

char *split_arg_value(char *arg) {
  strtok(arg, "=");
  return strtok(NULL, "=");
}

parameters parse_args(int argc, char **argv) {
  parameters params;

  int i;
  char *arg;
  char *value;
  for (i = 1; i < argc; i++) {
    arg = argv[i];
    value = split_arg_value(arg);
    if (is_arg(arg, "--help")) {
      print_help(EXIT_SUCCESS);
    } else if (is_arg(arg, "-v") || is_arg(arg, "--version")) {
      puts(PACKAGE " " VERSION);
      exit(EXIT_SUCCESS);
    } else {
      invalid_arg(arg);
    }
  }

  return params;
}