#include "types.h"

#ifndef WINDOW_H
#define WINDOW_H

Window *window_init(char *title, unsigned char monitor_index, bool windowed,
                    void (*error_callback)(int, const char *),
                    void (*key_callback)(Window *, int, int, int, int));

void window_update_title(Window *window, char *title);

void window_refresh(Window *window);

void window_get_context(Window *window, Context *context);

void window_close(Window *window, bool hard);

bool window_should_close(Window *window);

bool window_escape_key(int key, int action);

bool window_char_key(int key, int action, const int char_code);

#endif /* WINDOW_H */