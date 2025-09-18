#include <stdlib.h>
#include <string.h>

#include "strings.h"

char *strings_concat(const char *s1, const char *s2) {
  char *result;

  result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator

  strcpy(result, s1);
  strcat(result, s2);

  return result;
}