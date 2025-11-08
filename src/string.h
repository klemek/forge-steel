#include <stdbool.h>

#ifndef STRINGS_H
#define STRINGS_H

unsigned int string_trim(char *str);

bool string_is_number(char *value);

char *string_replace_at(char *src, unsigned int from, unsigned int to,
                        char *rpl);

#endif /* STRINGS_H */