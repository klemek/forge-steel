#include "types.h"

#ifndef FILE_H
#define FILE_H

void file_read(File *file, const char *path);

bool file_should_update(const File *file);

bool file_update(File *file);

void file_write(const char *path, const StringArray *lines);

void file_dump(const char *path, const char *content);

void file_free(const File *file);

#endif /* FILE_H */