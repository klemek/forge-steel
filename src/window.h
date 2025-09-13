#include "types.h"

#ifndef WINDOW_H
#define WINDOW_H

Window *init_window(Parameters params,
                    void (*error_callback)(int, const char *),
                    void (*key_callback)(Window *, int, int, int, int));

void update_window(Window *window);

Context get_window_context(Window *window);

void close_window(Window *window, bool hard);

bool window_should_close(Window *window);

bool escape_key(int key, int action);

#endif