#include "args.h"
#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help(int status_code) {
  puts(PACKAGE
       " " VERSION "\n\n"
       "usage: " PACKAGE " "
       "[--help] "
       "[-v] "
       "[-s=SCREEN] "
       "\n\n"
       "Fusion Of Real-time Generative Effects.\n\n"
       "options:\n"
       "  --help             show this help message and exit\n"
       "  -v, --version      print version\n"
       "  -s, --screen       output screen number (default: primary)\n");
  exit(status_code);
}

void invalid_arg(char *arg) {
  fprintf(stderr, "invalid argument: '%s'\n\n", arg);
  print_help(1);
}

void invalid_value(char *arg, char *value) {
  fprintf(stderr, "invalid value for argument '%s': '%s'\n\n", arg, value);
  print_help(1);
}

bool is_arg(char *arg, char *ref) { return strcoll(arg, ref) == 0; }

char *split_arg_value(char *arg) {
  strtok(arg, "=");
  return strtok(NULL, "=");
}

bool is_digit(char c) { return c >= '0' && c <= '9'; }

bool is_number(char *value) {
  if (value == NULL) {
    return false;
  }
  unsigned long value_len = strlen(value);
  unsigned int i;
  for (i = 0; i < value_len; i++) {
    if (!is_digit(value[i])) {
      return false;
    }
  }
  return true;
}

unsigned char parse_uchar(char *arg, char *value) {
  if (!is_number(value)) {
    invalid_value(arg, value);
  }
  unsigned long long tmp_value = (unsigned long long)atoll(value);
  if (tmp_value >= 256) {
    invalid_value(arg, value);
  }
  return (unsigned char)tmp_value;
}

Parameters parse_args(int argc, char **argv) {
  Parameters params;

  params.screen = 0;

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
    } else if (is_arg(arg, "-s") || is_arg(arg, "--screen")) {
      params.screen = parse_uchar(arg, value);
    } else {
      invalid_arg(arg);
    }
  }

  return params;
}