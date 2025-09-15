#include <stdio.h>

#ifndef LOG_H
#define LOG_H

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define log_debug(format, ...)                                                 \
  fprintf(stderr, ANSI_COLOR_MAGENTA "[DEBG] " format ANSI_COLOR_RESET         \
                                     "\n" __VA_OPT__(, ) __VA_ARGS__)
#define log_success(format, ...)                                               \
  fprintf(stdout, ANSI_COLOR_GREEN "[SUCC] " format ANSI_COLOR_RESET           \
                                   "\n" __VA_OPT__(, ) __VA_ARGS__)
#define log_info(format, ...)                                                  \
  fprintf(stdout, "[INFO] " format "\n" __VA_OPT__(, ) __VA_ARGS__)
#define log_warn(format, ...)                                                  \
  fprintf(stderr, ANSI_COLOR_YELLOW "[WARN] " format ANSI_COLOR_RESET          \
                                    "\n" __VA_OPT__(, ) __VA_ARGS__)
#define log_error(format, ...)                                                 \
  fprintf(stderr, ANSI_COLOR_RED "[FAIL] " format ANSI_COLOR_RESET             \
                                 "\n" __VA_OPT__(, ) __VA_ARGS__)

#endif