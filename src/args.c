#include <limits.h>
#include <log.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "config.h"
#include "string.h"
#include "types.h"

static void print_help(int status_code) {
  puts(PACKAGE
       " " VERSION "\n\n"
       "usage: " PACKAGE " "
       "[-h] "
       "[-v] "
       "[-hr] "
       "[-s=SCREEN] "
       "[-m=SCREEN] "
       "[-mo] "
       "[-p=PROJECT_PATH] "
       "[-c=CFG_FILE] "
       "[-sf=STATE_PATH] "
       "[-ls / -nls] "
       "[-ss / -nss] "
       "[-is=SIZE] "
       "[-v=FILE] "
       "[-vs=SIZE] "
       "[-t=TEMPO] "
       "[--demo] "
       "[-w] "
       "\n\n"
       "Fusion Of Real-time Generative Effects.\n\n"
       "options:\n"
       "  -h, --help                show this help message and exit\n"
       "  -v, --version             print version\n"
       "  -hr, --hot-reload         hot reload of shaders scripts\n"
       "  -s, --screen              output screen number (default: primary)\n"
       "  -m, --monitor             monitor screen number (default: none)\n"
       "  -mo, --monitor-only       no output screen\n"
       "  -p, --project             forge project directory "
       "(default: " DATADIR "/default)\n"
       "  -c, --config              config file name "
       "(default: forge_project.cfg)\n"
       "  -sf, --state-file         saved state file (default: "
       "forge_saved_state.txt)\n"
       "  -ls, --load-state         load saved state (default)\n"
       "  -nls, --no-load-state     do not load saved state\n"
       "  -ss, --save-state         save state (default)\n"
       "  -nss, --no-save-state     do not save state\n"
       "  -is, --internal-size      internal texture height (default: 720)\n"
       "  -v, --video-in            path to video capture device (multiple "
       "allowed)\n"
       "  -vs, --video-size         video capture desired height (default: "
       "internal texture height)\n"
       "  -t, --tempo               base tempo (default: 60)\n"
       "  --demo                    demonstration mode (assume --no-save-state "
       "and --no-load-state)\n"
       "  -w, --windowed            not fullscreen\n"
       "  --trace-midi              print midi code and values\n"
       "  --trace-fps               print fps status of subsystems\n");
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

static bool is_arg(char *arg, const char *ref) {
  return strcoll(arg, ref) == 0;
}

static char *split_arg_value(char *arg) {
  strtok(arg, "=");
  return strtok(NULL, "=");
}

static unsigned int parse_uint(char *arg, char *value) {
  if (!string_is_number(value)) {
    invalid_value(arg, value);
  }
  unsigned long long tmp_value = (unsigned long long)atoll(value);
  if (tmp_value >= UINT_MAX) {
    invalid_value(arg, value);
  }
  return (unsigned int)tmp_value;
}

Parameters args_parse(int argc, char **argv) {
  Parameters params;
  int i;
  char *arg;
  char *value;

  params.hot_reload = false;
  params.output = true;
  params.output_screen = 0;
  params.monitor = false;
  params.monitor_screen = 0;
  strncpy(params.project_path, DATADIR "/default", STR_LEN);
  strncpy(params.config_file, "forge_project.cfg", STR_LEN);
  strncpy(params.state_file, "forge_saved_state.txt", STR_LEN);
  params.load_state = true;
  params.save_state = true;
  params.internal_size = 720;
  params.video_size = 0;
  params.base_tempo = 60.0f;
  params.demo = false;
  params.windowed = false;
  params.video_in.length = 0;
  params.trace_midi = false;
  params.trace_fps = false;

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
      params.output_screen = parse_uint(arg, value);
    } else if (is_arg(arg, "-p") || is_arg(arg, "--project")) {
      strncpy(params.project_path, value, STR_LEN);
    } else if (is_arg(arg, "-c") || is_arg(arg, "--config")) {
      strncpy(params.config_file, value, STR_LEN);
    } else if (is_arg(arg, "-sf") || is_arg(arg, "--state-file")) {
      strncpy(params.state_file, value, STR_LEN);
    } else if (is_arg(arg, "-ls") || is_arg(arg, "--load-state")) {
      params.load_state = true;
    } else if (is_arg(arg, "-nls") || is_arg(arg, "--no-load-state")) {
      params.load_state = false;
    } else if (is_arg(arg, "-ss") || is_arg(arg, "--save-state")) {
      params.save_state = true;
    } else if (is_arg(arg, "-nss") || is_arg(arg, "--no-save-state")) {
      params.save_state = false;
    } else if (is_arg(arg, "-is") || is_arg(arg, "--internal-size")) {
      params.internal_size = parse_uint(arg, value);
      if (params.internal_size == 0) {
        invalid_value(arg, value);
      }
    } else if (is_arg(arg, "-v") || is_arg(arg, "--video-in")) {
      if (params.video_in.length == MAX_VIDEO) {
        log_error("maximum video input reached");
        exit(EXIT_FAILURE);
      }
      strncpy(params.video_in.values[params.video_in.length++], value, STR_LEN);
    } else if (is_arg(arg, "-vs") || is_arg(arg, "--video-size")) {
      params.video_size = parse_uint(arg, value);
      if (params.video_size == 0) {
        invalid_value(arg, value);
      }
    } else if (is_arg(arg, "-t") || is_arg(arg, "--tempo")) {
      params.base_tempo = (float)parse_uint(arg, value);
    } else if (is_arg(arg, "-m") || is_arg(arg, "--monitor")) {
      params.monitor = true;
      params.monitor_screen = parse_uint(arg, value);
    } else if (is_arg(arg, "-mo") || is_arg(arg, "--monitor-only")) {
      params.output = false;
      params.monitor = true;
    } else if (is_arg(arg, "--demo")) {
      params.demo = true;
      params.load_state = false;
      params.save_state = false;
    } else if (is_arg(arg, "-w") || is_arg(arg, "--windowed")) {
      params.windowed = true;
    } else if (is_arg(arg, "--trace-midi")) {
      params.trace_midi = true;
    } else if (is_arg(arg, "--trace-fps")) {
      params.trace_fps = true;
    } else {
      invalid_arg(arg);
    }
  }

  if (params.monitor && params.output &&
      params.monitor_screen == params.output_screen && !params.windowed) {
    log_error("monitor screen cannot be the same as output screen");
    exit(EXIT_FAILURE);
  }

  if (params.video_size == 0) {
    params.video_size = params.internal_size;
  }

  return params;
}