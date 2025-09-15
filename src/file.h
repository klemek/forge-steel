#include "types.h"

#ifndef FILE_H
#define FILE_H

File read_file(char *path);

bool should_update_file(File file);

void update_file(File *file);

void free_file(File *file);

void prepend_file(File *src, File extra);

#endif