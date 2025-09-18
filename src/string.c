#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"

char *string_concat(const char *s1, const char *s2) {
  char *result;

  result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator

  strcpy(result, s1);
  strcat(result, s2);

  return result;
}

unsigned int string_trim(char *str) {
  // https://www.delftstack.com/howto/c/trim-string-in-c/
  unsigned int start;
  unsigned int end;

  start = 0;
  end = strlen(str) - 1;

  if (end == 0) {
    return 0;
  }

  // Remove leading whitespace
  while (str[start] == ' ') {
    start++;
  }

  // Remove trailing whitespace
  while (end > start && str[end] == ' ') {
    end--;
  }

  // If the string was trimmed, adjust the null terminator
  if (start > 0 || end < (strlen(str) - 1)) {
    memmove(str, str + start, end - start + 1);
    str[end - start + 1] = '\0';
  }

  return end - start + 1;
}

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

bool string_is_number(char *value) {
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