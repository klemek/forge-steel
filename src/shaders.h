#include "types.h"

#ifndef SHADERS_H
#define SHADERS_H

ShaderProgram shaders_init(Project project, SharedContext *context,
                           VideoCaptureArray inputs, ShaderProgram *previous);

void shaders_update(ShaderProgram program, FileArray fragment_shaders,
                    unsigned int i);

void shaders_compute(ShaderProgram program, SharedContext *context,
                     bool monitor, bool output_only);

void shaders_free(ShaderProgram program);

void shaders_free_window(ShaderProgram program, bool secondary);

void shaders_free_input(ShaderProgram program, VideoCapture input);

#endif /* SHADERS_H */