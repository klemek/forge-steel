#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "file.h"
#include "logs.h"
#include "strings.h"
#include "types.h"

static time_t get_file_time(File file) {
  struct stat attr;
  if (stat(file.path, &attr) == 0) {
    return attr.st_mtim.tv_sec;
  }
  return 0;
}

bool file_should_update(File file) {
  return file.last_write != get_file_time(file);
}

void file_update(File *file) {
  long length;
  FILE *file_pointer;

  // free remaining data
  if (file->content != 0) {
    free(file->content);
  }
  log_info("Reading '%s'...", file->path);
  // init empty file
  file->content = 0;
  file->error = false;

  // open file
  file_pointer = fopen(file->path, "rb");
  if (file_pointer == NULL) {
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
  if (file->content == NULL) {
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

File file_read(char *path) {
  File file;

  file.path = path;
  file.content = NULL;
  file.error = false;
  file.last_write = 0;

  file_update(&file);
  return file;
}

void file_prepend(File *src, File extra) {
  char *old_src_content;

  old_src_content = src->content;
  src->content = strings_concat(extra.content, src->content);
  free(old_src_content);
}

void file_free(File *file, bool free_path) {
  free(file->content);
  if (free_path) {
    free(file->path);
  }
}