AUTOMAKE_OPTIONS = foreign subdir-objects -Wall
bin_PROGRAMS = forge
forge_SOURCES = src/main.c src/args.c src/forge.c src/file.c
forge_CFLAGS = -Ofast -march=native -flto -funroll-loops -fprefetch-loop-arrays -fno-exceptions -fopenmp
include_HEADERS = src/main.h src/args.h src/config.h src/types.h src/forge.h src/file.h include/linmath.h include/glad/gl.h