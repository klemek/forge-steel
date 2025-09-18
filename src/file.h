#include "types.h"

#ifndef FILE_H
#define FILE_H

File file_read(char *path);

bool file_should_update(File file);

void file_update(File *file);

void file_prepend(File *src, File extra);

void file_free(File *file);

#endif /* FILE_H */