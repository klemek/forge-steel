AUTOMAKE_OPTIONS = foreign subdir-objects -Wall
bin_PROGRAMS = forge
forge_SOURCES = src/main.c src/args.c src/forge.c src/file.c src/window.c src/shaders.c src/timer.c src/string.c src/config_file.c src/rand.c src/video.c src/shared.c src/midi.c src/state.c src/arr.c src/tempo.c $(top_srcdir)/include/glad/gl.h $(top_srcdir)/include/glad/egl.h $(top_srcdir)/hashmap.c/hashmap.c $(top_srcdir)/log.c/src/log.c
forge_CFLAGS = -Ofast -march=native -flto -funroll-loops -fprefetch-loop-arrays -fno-exceptions -fopenmp -I$(top_srcdir)/include -DGLFW_INCLUDE_NONE -DGLFW_EXPOSE_NATIVE_EGL -DGLFW_NATIVE_INCLUDE_NONE -DLOG_USE_COLOR -DDATADIR=\"$(datadir)/$(PACKAGE)\"
forge_LDADD = -lm -lGL -lglfw -lasound
include_HEADERS = src/main.h src/args.h src/config.h src/types.h src/forge.h src/file.h src/constants.h src/window.h src/shaders.h src/timer.h src/string.h src/config_file.h src/rand.h src/video.h src/shared.h src/midi.h src/state.h src/arr.h src/tempo.h $(top_srcdir)/include/glad/gl.h $(top_srcdir)/include/glad/egl.h $(top_srcdir)/include/linmath.h $(top_srcdir)/include/hashmap.h $(top_srcdir)/include/log.h

EXTRA_DIST = default.cfg shaders/frag0.glsl shaders/frag1.glsl shaders/frag2.glsl shaders/frag3.glsl shaders/frag4.glsl shaders/frag5.glsl shaders/frag6.glsl shaders/frag7.glsl shaders/frag8.glsl shaders/frag9.glsl shaders/frag10.glsl
confdir = $(prefix)/share/$(PACKAGE)
conf_DATA = default.cfg shaders/frag0.glsl shaders/frag1.glsl shaders/frag2.glsl shaders/frag3.glsl shaders/frag4.glsl shaders/frag5.glsl shaders/frag6.glsl shaders/frag7.glsl shaders/frag8.glsl shaders/frag9.glsl shaders/frag10.glsl