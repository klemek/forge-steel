#include <stdbool.h>

#ifndef STRINGS_H
#define STRINGS_H

char *string_concat(const char *s1, const char *s2);

unsigned int string_trim(char *str);

bool string_is_number(char *value);

#endif /* STRINGS_H */