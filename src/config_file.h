#include "types.h"

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

ConfigFile config_file_read(char *path, bool free_path);

char *config_file_get_str(ConfigFile config, char *key, char *default_value);

int config_file_get_int(ConfigFile config, char *key, int default_value);

void config_file_free(ConfigFile config);

#endif /* CONFIG_FILE_H */