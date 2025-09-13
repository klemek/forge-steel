#include "types.h"

#ifndef SHADERS_H
#define SHADERS_H

ShaderProgram init_program(File fragment_shader);

void update_program(ShaderProgram program, File fragment_shader);

#endif