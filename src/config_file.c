#include <bsd/string.h>
#include <hashmap.h>
#include <log.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#include "config.h"
#include "config_file.h"
#include "file.h"
#include "string.h"

static int item_compare(const void *a, const void *b,
                        __attribute__((unused)) void *udata) {
  const ConfigFileItem *c_item_a;
  const ConfigFileItem *c_item_b;

  c_item_a = a;
  c_item_b = b;

  return strcmp(c_item_a->key, c_item_b->key);
}

static uint64_t item_hash(const void *item, uint64_t seed0, uint64_t seed1) {
  const ConfigFileItem *c_item;

  c_item = item;

  return hashmap_sip(c_item->key, strnlen(c_item->key, STR_LEN), seed0, seed1);
}

static void parse_config_file_line(const ConfigFile *config, char *line) {
  unsigned int size;
  const char *equal_pos;
  unsigned int key_size;
  unsigned int value_size;
  ConfigFileItem item;

  size = string_trim(line);

  if (size == 0 || line[0] == '#') {
    return;
  }

  equal_pos = strchr(line, '=');

  if (equal_pos == NULL) {
    log_warn("Invalid config line '%s'", line);
    return;
  }

  key_size = equal_pos - line;
  value_size = size - key_size - 1;

  strlcpy(item.key, line, key_size + 1);
  item.key[key_size] = '\0';

  if (value_size > 0) {
    strlcpy(item.value, line + key_size + 1, value_size + 1);
    item.value[value_size] = '\0';
  }

  hashmap_set(config->map, &item);
}

void config_file_read(ConfigFile *config, const char *path) {
  File file;
  char *line;
  char *rest;

  config->map = hashmap_new(sizeof(ConfigFileItem), 0, 0, 0, item_hash,
                            item_compare, NULL, NULL);

  file_read(&file, path);

  if (file.error) {
    return;
  }

  line = strtok_r(file.content, "\n", &rest);

  while (line != NULL) {
    parse_config_file_line(config, line);
    line = strtok_r(rest, "\n", &rest);
  }

  file_free(&file);
}

const char *config_file_get_str(const ConfigFile *config, const char *key,
                                const char *default_value) {
  ConfigFileItem c_key;
  const ConfigFileItem *item;

  strlcpy(c_key.key, key, STR_LEN);

  item = (const ConfigFileItem *)hashmap_get(config->map, &c_key);

  if (item == NULL || strnlen(item->value, STR_LEN) == 0) {
    return default_value;
  }

  return item->value;
}

unsigned int config_file_get_int(const ConfigFile *config, const char *key,
                                 unsigned int default_value) {
  ConfigFileItem c_key;
  const ConfigFileItem *item;

  strlcpy(c_key.key, key, STR_LEN);

  item = (const ConfigFileItem *)hashmap_get(config->map, &c_key);

  if (item == NULL || strnlen(item->value, STR_LEN) == 0) {
    return default_value;
  }

  if (!string_is_number((char *)item->value)) {
    log_warn("Invalid number for %s: '%s'", item->key, item->value);
    return default_value;
  }

  return (unsigned int)atoi(item->value);
}

void config_file_free(const ConfigFile *config) { hashmap_free(config->map); }