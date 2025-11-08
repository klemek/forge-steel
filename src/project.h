#include "types.h"

#ifndef PROJECT_H
#define PROJECT_H

void project_init(Project *project, char *project_path, char *config_file);

void project_reload(Project *project, void (*reload_callback)(unsigned int));

void project_free(Project *project);

#endif /* PROJECT_H */