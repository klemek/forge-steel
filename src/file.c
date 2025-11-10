#include <bsd/string.h>
#include <log.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "types.h"

#include "file.h"
#include "string.h"

static time_t get_file_time(File *file) {
  struct stat attr;

  if (stat(file->path, &attr) == 0) {
    return attr.st_mtim.tv_sec;
  }

  return 0;
}

bool file_should_update(File *file) {
  return file->last_write != get_file_time(file);
}

bool file_update(File *file) {
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
    return false;
  }
  // read file length
  fseek(file_pointer, 0, SEEK_END);
  length = ftell(file_pointer);
  // init buffer
  fseek(file_pointer, 0, SEEK_SET);
  file->content = malloc(length + 1);
  if (file->content == NULL) {
    file->error = true;
    fclose(file_pointer);
    log_error("Cannot read file '%s'", file->path);
    return false;
  }
  // read file
  fread(file->content, sizeof(char), length, file_pointer);
  // close file
  fclose(file_pointer);
  // append null byte
  file->content[length] = 0;
  // read last update time
  file->last_write = get_file_time(file);

  return true;
}

void file_read(File *file, char *path) {
  strlcpy(file->path, path, STR_LEN);
  file->content = NULL;
  file->error = false;
  file->last_write = 0;

  file_update(file);
}

void file_dump(char *path, char *content) {
  FILE *file_pointer;

  log_info("Dumping %s...", path);

  // open file
  file_pointer = fopen(path, "w");
  if (file_pointer == NULL) {
    log_warn("Cannot open file '%s'", path);
    return;
  }

  // write file
  fprintf(file_pointer, "%s", content);

  // close file
  fclose(file_pointer);
}

void file_write(char *path, StringArray *lines) {
  FILE *file_pointer;

  log_info("Writing %s...", path);

  // open file
  file_pointer = fopen(path, "w");
  if (file_pointer == NULL) {
    log_warn("Cannot open file '%s'", path);
    return;
  }

  // write file
  for (unsigned int i = 0; i < lines->length; i++) {
    fprintf(file_pointer, "%s\n", lines->values[i]);
  }

  // close file
  fclose(file_pointer);
}

void file_free(File *file) {
  if (!file->error) {
    free(file->content);
    file->error = true;
  }
}
