#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "types.h"

void update_file_time(File *file) {
  struct stat attr;
  if (stat(file->path, &attr) == 0) {
    file->last_write = attr.st_mtime;
  }
}

bool should_update_file(File *file) {
  struct stat attr;
  if (stat(file->path, &attr) == 0) {
    return file->last_write != attr.st_mtime;
  }
  return false;
}

void update_file(File *file) {
  // free remaining data
  free(file->content);
  // init empty file
  file->content = 0;
  file->error = false;
  long length;
  // open file
  FILE *file_pointer = fopen(file->path, "rb");
  if (!file_pointer) {
    file->error = true;
    return;
  }
  // read file length
  fseek(file_pointer, 0, SEEK_END);
  length = ftell(file_pointer);
  // init buffer
  fseek(file_pointer, 0, SEEK_SET);
  file->content = (char *)malloc((length + 1) * sizeof(char));
  if (!file->content) {
    file->error = true;
    fclose(file_pointer);
    return;
  }
  // read file
  fread(file->content, sizeof(char), length, file_pointer);
  // close file
  fclose(file_pointer);
  // append null byte
  file->content[length] = '\0';
  // read last update time
  update_file_time(file);
}

File read_file(char *path) {
  File file = {path, 0, 0, 0};
  update_file(&file);
  return file;
}