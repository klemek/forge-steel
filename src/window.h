#include "types.h"

#ifndef WINDOW_H
#define WINDOW_H

Window *init_window(Parameters params,
                    void (*error_callback)(int, const char *),
                    void (*key_callback)(Window *, int, int, int, int));

#endif