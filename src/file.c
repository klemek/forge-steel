#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "logs.h"
#include "types.h"

time_t get_file_time(File file) {
  struct stat attr;
  if (stat(file.path, &attr) == 0) {
    return attr.st_mtim.tv_sec;
  }
  return 0;
}

bool should_update_file(File file) {
  return file.last_write != get_file_time(file);
}

void update_file(File *file) {
  // free remaining data
  if (file->content != 0) {
    free(file->content);
  }
  log_info("Reading '%s'...", file->path);
  // init empty file
  file->content = 0;
  file->error = false;
  long length;
  // open file
  FILE *file_pointer = fopen(file->path, "rb");
  if (!file_pointer) {
    file->error = true;
    log_error("Cannot open file '%s'", file->path);
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
    log_error("Cannot read file '%s'", file->path);
    return;
  }
  // read file
  fread(file->content, sizeof(char), length, file_pointer);
  // close file
  fclose(file_pointer);
  // append null byte
  file->content[length] = '\0';
  // read last update time
  file->last_write = get_file_time(*file);
}

File read_file(char *path) {
  File file = {path, 0, false, 0};
  update_file(&file);
  return file;
}

void free_file(File *file) { free(file->content); }