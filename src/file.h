#include "types.h"

#ifndef FILE_H
#define FILE_H

File file_read(char *path);

bool file_should_update(File *file);

bool file_update(File *file);

void file_write(char *path, StringArray *lines);

void file_free(File *file);

#endif /* FILE_H */