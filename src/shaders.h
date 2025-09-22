#include "types.h"

#ifndef SHADERS_H
#define SHADERS_H

ShaderProgram shaders_init(File *fragment_shaders, ConfigFile shader_config,
                           Context context, VideoCapture *video_captures,
                           unsigned int count, ShaderProgram *previous);

void shaders_update(ShaderProgram program, File *fragment_shaders,
                    unsigned int i);

void shaders_compute(ShaderProgram program, Context context, bool monitor,
                     bool output_only);

void shaders_free(ShaderProgram program);

void shaders_free_window(ShaderProgram program, bool secondary);

void shaders_free_video(ShaderProgram program, VideoCapture video_capture);

#endif /* SHADERS_H */