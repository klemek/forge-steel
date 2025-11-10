#include "types.h"

#ifndef SHADERS_H
#define SHADERS_H

void shaders_init(ShaderProgram *program, const Project *project,
                  const SharedContext *context, VideoCaptureArray *inputs,
                  bool rebind);

void shaders_update(ShaderProgram *program, const File *fragment_shader,
                    unsigned int i, const Project *project);

void shaders_compute(ShaderProgram *program, const SharedContext *context,
                     bool monitor, bool output_only);

void shaders_free(const ShaderProgram *program);

void shaders_free_window(const ShaderProgram *program, bool secondary);

void shaders_free_input(const ShaderProgram *program,
                        const VideoCapture *input);

#endif /* SHADERS_H */