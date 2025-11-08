#include "types.h"

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

void config_file_read(ConfigFile *config, char *path);

char *config_file_get_str(ConfigFile *config, char *key, char *default_value);

unsigned int config_file_get_int(ConfigFile *config, char *key,
                                 unsigned int default_value);

void config_file_free(ConfigFile *config);

#endif /* CONFIG_FILE_H */