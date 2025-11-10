#include <stdbool.h>

#ifndef STRINGS_H
#define STRINGS_H

unsigned int string_trim(char *str);

bool string_is_number(const char *value);

char *string_replace_at(const char *src, unsigned int from, unsigned int to,
                        const char *rpl);

#endif /* STRINGS_H */