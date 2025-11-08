#include "types.h"

#ifndef FILE_H
#define FILE_H

void file_read(File *file, char *path);

bool file_should_update(File *file);

bool file_update(File *file);

void file_write(char *path, StringArray *lines);

void file_dump(char *path, char *content);

void file_free(File *file);

#endif /* FILE_H */