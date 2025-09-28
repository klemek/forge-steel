#include "types.h"

#ifndef SHADERS_H
#define SHADERS_H

ShaderProgram shaders_init(File *fragment_shaders, ConfigFile config,
                           SharedContext *context, VideoCapture *inputs,
                           unsigned int input_count, ShaderProgram *previous);

void shaders_update(ShaderProgram program, File *fragment_shaders,
                    unsigned int i);

void shaders_compute(ShaderProgram program, SharedContext *context,
                     bool monitor, bool output_only);

void shaders_free(ShaderProgram program);

void shaders_free_window(ShaderProgram program, bool secondary);

void shaders_free_input(ShaderProgram program, VideoCapture input);

#endif /* SHADERS_H */