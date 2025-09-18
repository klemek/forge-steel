#include "types.h"

#ifndef WINDOW_H
#define WINDOW_H

Window *window_init(char *title, unsigned char monitor_index,
                    void (*error_callback)(int, const char *),
                    void (*key_callback)(Window *, int, int, int, int));

void window_update_title(Window *window, char *title);

void window_refresh(Window *window);

Context window_get_context(Window *window);

void window_close(Window *window, bool hard);

bool window_should_close(Window *window);

bool window_escape_key(int key, int action);

#endif /* WINDOW_H */