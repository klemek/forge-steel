#include "types.h"

#ifndef PROJECT_H
#define PROJECT_H

void project_init(Project *project, const char *project_path,
                  const char *config_file);

void project_reload(Project *project, void (*reload_callback)(unsigned int));

void project_free(const Project *project);

#endif /* PROJECT_H */