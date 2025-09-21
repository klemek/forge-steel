#include "types.h"

#ifndef WINDOW_H
#define WINDOW_H

void window_startup(void (*error_callback)(int, const char *));

void window_terminate();

Window *window_init(char *title, unsigned char monitor_index, bool windowed,
                    Window *shared_context,
                    void (*key_callback)(Window *, int, int, int, int));

void window_update_title(Window *window, char *title);

double window_get_time();

void window_use(Window *window, Context *context);

void window_refresh(Window *window);

void window_events();

void window_close(Window *window);

bool window_should_close(Window *window);

bool window_escape_key(int key, int action);

bool window_char_key(int key, int action, const int char_code);

#endif /* WINDOW_H */