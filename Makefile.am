AUTOMAKE_OPTIONS = foreign subdir-objects -Wall
bin_PROGRAMS = forge
forge_SOURCES = src/main.c src/args.c src/forge.c src/file.c src/window.c src/shaders.c src/timer.c src/string.c src/config_file.c src/rand.c src/video.c src/shared.c src/midi.c src/state.c src/arr.c src/tempo.c $(top_srcdir)/include/glad/gl.h $(top_srcdir)/include/glad/egl.h $(top_srcdir)/hashmap.c/hashmap.c $(top_srcdir)/log.c/src/log.c
forge_CFLAGS = -Ofast -march=native -flto -funroll-loops -fprefetch-loop-arrays -fno-exceptions -fopenmp -I$(top_srcdir)/include -DGLFW_INCLUDE_NONE -DGLFW_EXPOSE_NATIVE_EGL -DGLFW_NATIVE_INCLUDE_NONE -DLOG_USE_COLOR -DDATADIR=\"$(datadir)/$(PACKAGE)\"
forge_LDADD = -lm -lGL -lglfw -lasound
include_HEADERS = src/main.h src/args.h src/config.h src/types.h src/forge.h src/file.h src/constants.h src/window.h src/shaders.h src/timer.h src/string.h src/config_file.h src/rand.h src/video.h src/shared.h src/midi.h src/state.h src/arr.h src/tempo.h $(top_srcdir)/include/glad/gl.h $(top_srcdir)/include/glad/egl.h $(top_srcdir)/include/linmath.h $(top_srcdir)/include/hashmap.h $(top_srcdir)/include/log.h

EXTRA_DIST = default/forge_project.cfg default/frag0.glsl default/frag1.glsl default/frag2.glsl default/frag3.glsl default/frag4.glsl default/frag5.glsl default/frag6.glsl default/frag7.glsl default/frag8.glsl default/frag9.glsl default/frag10.glsl
confdir = $(prefix)/share/$(PACKAGE)
conf_DATA = default/forge_project.cfg default/frag0.glsl default/frag1.glsl default/frag2.glsl default/frag3.glsl default/frag4.glsl default/frag5.glsl default/frag6.glsl default/frag7.glsl default/frag8.glsl default/frag9.glsl default/frag10.glsl