AUTOMAKE_OPTIONS = foreign subdir-objects -Wall
bin_PROGRAMS = forge
forge_SOURCES = src/main.c src/args.c src/forge.c src/file.c src/window.c src/shaders.c src/timer.c src/strings.c src/config_file.c $(top_srcdir)/include/glad/gl.h $(top_srcdir)/hashmap.c/hashmap.c
forge_CFLAGS = -Ofast -march=native -flto -funroll-loops -fprefetch-loop-arrays -fno-exceptions -fopenmp -I$(top_srcdir)/include -DGLFW_INCLUDE_NONE
forge_LDADD = -lm -lGL -lglfw
include_HEADERS = src/main.h src/args.h src/config.h src/types.h src/forge.h src/file.h src/constants.h src/window.h src/shaders.h src/logs.h src/timer.h src/strings.h src/config_file.h $(top_srcdir)/include/glad/gl.h $(top_srcdir)/include/linmath.h $(top_srcdir)/include/hashmap.h