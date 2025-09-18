#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "logs.h"
#include "types.h"

static void print_help(int status_code) {
  puts(PACKAGE
       " " VERSION "\n\n"
       "usage: " PACKAGE " "
       "[-h] "
       "[-v] "
       "[-hr] "
       "[-s=SCREEN] "
       "[-f=DIR_PATH] "
       "\n\n"
       "Fusion Of Real-time Generative Effects.\n\n"
       "options:\n"
       "  -h, --help         show this help message and exit\n"
       "  -v, --version      print version\n"
       "  -hr, --hot-reload  hot reload of shaders scripts\n"
       "  -s, --screen       output screen number (default: primary)\n"
       "  -f, --frag         fragment shaders directory (default: TODO)\n");
  exit(status_code);
}

static void invalid_arg(char *arg) {
  log_error("invalid argument: '%s'", arg);
  print_help(EXIT_FAILURE);
}

static void invalid_value(char *arg, char *value) {
  log_error("invalid value for argument '%s': '%s'", arg, value);
  print_help(EXIT_FAILURE);
}

static bool is_arg(char *arg, char *ref) { return strcoll(arg, ref) == 0; }

static char *split_arg_value(char *arg) {
  strtok(arg, "=");
  return strtok(NULL, "=");
}

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

static bool is_number(char *value) {
  unsigned long value_len;
  unsigned int i;
  if (value == NULL) {
    return false;
  }
  value_len = strlen(value);
  for (i = 0; i < value_len; i++) {
    if (!is_digit(value[i])) {
      return false;
    }
  }
  return true;
}

static unsigned char parse_uchar(char *arg, char *value) {
  if (!is_number(value)) {
    invalid_value(arg, value);
  }
  unsigned long long tmp_value = (unsigned long long)atoll(value);
  if (tmp_value >= 256) {
    invalid_value(arg, value);
  }
  return (unsigned char)tmp_value;
}

Parameters args_parse(int argc, char **argv) {
  Parameters params;
  int i;
  char *arg;
  char *value;

  params.screen = 0;
  params.frag_path = 0;
  params.hot_reload = false;

  for (i = 1; i < argc; i++) {
    arg = argv[i];
    value = split_arg_value(arg);
    if (is_arg(arg, "-h") || is_arg(arg, "--help")) {
      print_help(EXIT_SUCCESS);
    } else if (is_arg(arg, "-v") || is_arg(arg, "--version")) {
      puts(PACKAGE " " VERSION);
      exit(EXIT_SUCCESS);
    } else if (is_arg(arg, "-hr") || is_arg(arg, "--hot-reload")) {
      params.hot_reload = true;
    } else if (is_arg(arg, "-s") || is_arg(arg, "--screen")) {
      params.screen = parse_uchar(arg, value);
    } else if (is_arg(arg, "-f") || is_arg(arg, "--frag")) {
      params.frag_path = value;
    } else {
      invalid_arg(arg);
    }
  }

  if (params.frag_path == 0) {
    log_error("required argument -f/--frag");
    exit(EXIT_FAILURE);
  }

  return params;
}