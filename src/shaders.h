#include "types.h"

#ifndef SHADERS_H
#define SHADERS_H

ShaderProgram shaders_init(File *fragment_shaders, ConfigFile shader_config,
                           Context context, ShaderProgram *shared_program);

void shaders_update(ShaderProgram program, File *fragment_shaders,
                    unsigned int i);

void shaders_compute(ShaderProgram program, Context context, bool monitor,
                     bool output_only);

void shaders_free(ShaderProgram program);

void shaders_free_window(ShaderProgram program, bool secondary);

#endif /* SHADERS_H */