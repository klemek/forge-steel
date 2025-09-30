#include "types.h"

#ifndef SHADERS_H
#define SHADERS_H

ShaderProgram shaders_init(FileArray fragment_shaders, ConfigFile config,
                           SharedContext *context, VideoCaptureArray inputs,
                           StateConfig state_config, ShaderProgram *previous);

void shaders_update(ShaderProgram program, FileArray fragment_shaders,
                    unsigned int i);

void shaders_compute(ShaderProgram program, SharedContext *context,
                     bool monitor, bool output_only);

void shaders_free(ShaderProgram program);

void shaders_free_window(ShaderProgram program, bool secondary);

void shaders_free_input(ShaderProgram program, VideoCapture input);

#endif /* SHADERS_H */