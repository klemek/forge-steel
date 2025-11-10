#include <bsd/string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"

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

char *string_replace_at(char *src, unsigned int from, unsigned int to,
                        char *rpl) {
  unsigned long src_len, rpl_len;
  char *dst;

  src_len = strlen(src);
  rpl_len = strlen(rpl);

  dst = malloc(src_len - (to - from) + rpl_len + 1);

  strncpy(dst, src, from);
  strncpy(dst + from, rpl, rpl_len);
  strlcpy(dst + from + rpl_len, src + to, src_len - to + 1);

  return dst;
}