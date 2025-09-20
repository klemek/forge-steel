#include "types.h"

#ifndef SHADERS_H
#define SHADERS_H

ShaderProgram shaders_init(File *fragment_shaders, ConfigFile shader_config,
                           Context context);

void shaders_update(ShaderProgram program, File *fragment_shaders,
                    unsigned int i);

void shaders_compute(ShaderProgram program, Context context, bool monitor);

#endif /* SHADERS_H */