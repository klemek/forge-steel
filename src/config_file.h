#include "types.h"

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

void config_file_read(ConfigFile *config, const char *path);

bool config_file_has(const ConfigFile *config, const char *key);

const char *config_file_get_str(const ConfigFile *config, const char *key,
                                const char *default_value);

unsigned int config_file_get_int(const ConfigFile *config, const char *key,
                                 unsigned int default_value);

void config_file_free(const ConfigFile *config);

#endif /* CONFIG_FILE_H */