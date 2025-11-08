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

  return hashmap_sip(c_item->key, strlen(c_item->key), seed0, seed1);
}

static void parse_config_file_line(ConfigFile config, char *line) {
  unsigned int size;
  char *equal_pos;
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

  strlcpy(item.key, line, key_size);
  item.key[key_size] = '\0';

  if (value_size > 0) {
    strlcpy(item.value, line + key_size + 1, value_size);
    item.value[value_size] = '\0';
  }

  hashmap_set(config.map, &item);
}

ConfigFile config_file_read(char *path) {
  File file;
  ConfigFile config;
  char *line;

  config.map = hashmap_new(sizeof(ConfigFileItem), 0, 0, 0, item_hash,
                           item_compare, NULL, NULL);

  file = file_read(path);

  if (file.error) {
    return config;
  }

  line = strtok(file.content, "\n");

  while (line != NULL) {
    parse_config_file_line(config, line);
    line = strtok(NULL, "\n");
  }

  file_free(&file);

  return config;
}

char *config_file_get_str(ConfigFile config, char *key, char *default_value) {
  ConfigFileItem c_key;
  ConfigFileItem *item;

  strlcpy(c_key.key, key, STR_LEN);

  item = (ConfigFileItem *)hashmap_get(config.map, &c_key);

  if (item == NULL || strlen(item->value) == 0) {
    return default_value;
  }

  return item->value;
}

unsigned int config_file_get_int(ConfigFile config, char *key,
                                 unsigned int default_value) {
  ConfigFileItem c_key;
  ConfigFileItem *item;

  strlcpy(c_key.key, key, STR_LEN);

  item = (ConfigFileItem *)hashmap_get(config.map, &c_key);

  if (item == NULL || strlen(item->value) == 0) {
    return default_value;
  }

  if (!string_is_number(item->value)) {
    log_warn("Invalid number for %s: '%s'", item->key, item->value);
    return default_value;
  }

  return (unsigned int)atoi(item->value);
}

void config_file_free(ConfigFile config) { hashmap_free(config.map); }