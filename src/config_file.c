#include <hashmap.h>
#include <stdlib.h>

#include "config_file.h"
#include "file.h"
#include "types.h"

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

ConfigFile config_file_read(char *path, bool free_path) {
  File file;
  ConfigFile output;

  file = file_read(path);

  output.map = hashmap_new(sizeof(ConfigFileItem), 0, 0, 0, item_hash,
                           item_compare, NULL, NULL);

  // TODO

  file_free(&file, free_path);

  return output;
}

char *config_file_get_str(ConfigFile config, char *key, char *default_value) {
  ConfigFileItem c_key;
  ConfigFileItem *item;

  c_key.key = key;

  item = (ConfigFileItem *)hashmap_get(config.map, &c_key);

  if (item == NULL) {
    return default_value;
  }

  return item->value;
}

int config_file_get_int(ConfigFile config, char *key, int default_value) {
  ConfigFileItem c_key;
  ConfigFileItem *item;

  c_key.key = key;

  item = (ConfigFileItem *)hashmap_get(config.map, &c_key);

  if (item == NULL) {
    return default_value;
  }

  return atoi(item->value);
}

void config_file_free(ConfigFile config) { hashmap_free(config.map); }