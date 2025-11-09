# Working on this project

## Makefile.dev targets

```text
clean         remove build folder
build         build project into build/forge
run           run project with test args
demo          run project with demo mode
valgrind      valgrind analysis
clean-release remove autoconf/automake files
test-release  try to build release
release-%     make full release of version %
release-arch  make arch-linux release package
```

## Release guide

```shell
# make full build
make -f Makefile.dev release-1.0.0
# push release
git push origin master --tags
# create release from tag on github
# attach .tar.gz to the github release
make -f Makefile.dev release-arch
# attach .pkg.tar.zst to the github release
```

## Roadmap

- [x] Basics
  - [x] Create GLSL Window
  - [x] Load static fragment shader into GLSL
  - [x] Add default uniforms
  - [x] Read fragment shader from file
  - [x] Handle compilation errors
  - [x] Minimal working fragment sample
  - [x] Hot-reload fragment shader (with arg)
  - [x] Specify fragment shader path
  - [x] Force fullscreen
  - [x] Select screen as argument / config
  - [x] fps in window title
  - [x] Clean code
- [x] Multi-stage shaders
  - [x] Test 2 stages with render to texture
  - [x] 2 in 2 fx 1 mix 1 fx layout
  - [x] Include common code
  - [x] 16 input + 16 fx definition and selection (with const param)
  - [x] Feedback texture
  - [x] Shaders config file
    - [x] uniform config
    - [x] fragment config
    - [x] subroutines config
  - [x] demo mode
  - [x] random seed injected into shaders
  - [x] internal texture size for speed
  - [x] pass state as uniform
  - [x] debug shader (and in monitor)
  - [x] random mode / demo mode with R/D key
  - [x] Clean code and fix things
- [x] Midi
  - [x] Read Midi events
  - [x] Read midi mapping config file
  - [x] Write Midi events
  - [x] Send midi data to shaders
  - [x] Save midi state
  - [x] Load midi state from last save
  - [x] State machine with A/B switch
  - [x] Tap-tempo feature
  - [x] Clean code and fix things
- [x] Video input
  - [x] Fixed camera video
  - [x] Pass video info to shaders
  - [x] Sub process video reading
  - [x] Shader based format mapping
  - [x] Video mapping config file
  - [x] Get first video size matching internal size
  - [x] Other internal size for video
  - [x] Pass input fps into shaders for debug
  - [x] Clean code and fix things
- [x] Monitor screen
  - [x] 2nd window
  - [x] Use buffers as panels (INA A FXA / DEBUG A+B FXA+B / INB B FXB)
  - [x] Clean code and fix things
  - [x] Share openGL state between monitor and screen
- [ ] Default project
  - [x] split with includes
  - [x] src  9 : sentences repeat h
  - [x] src 10 : isometric grid
  - [x] src 12 : pattern in my kitchen
  - [ ] src 13 : cursor with select
  - [ ] src 14 : ?
  - [ ] src 15 : ?
  - [x] fx 11 : spill
  - [x] fx 12 : game of life
  - [x] fx 13 : sobel
  - [ ] fx 14 : colorize range (pretty color ranges)
  - [ ] fx 15 : ?
  - [x] frag 10 : move debug screen here
  - [ ] debug : add src/fx/A/B indicators
- [ ] Other
  - [x] `forge_project.cfg`
  - [x] Define frag prefix in config
  - [x] Use custom `#include xxx.glsl` preprocessor
  - [x] Use snprintf isntead of sprintf (and strlcpy instand of strncpy)
  - [x] Pass "heavy" struct as pointer to avoid stack overload
  - [x] Clean and sort args
  - [x] `--auto-random` / `--no-auto-random`
  - [x] Dump failed compilation into tmp file
  - [ ] Update readme with usage documentation
  - [x] Documentation in default config file
  - [x] Clone "shaders" and config in system path at setup
  - [ ] Minimal sample shaders
  - [ ] Find and fix opengl errors 0500 ?
  - [ ] Investigate video device fps loss (bad unregister ?)
    -  explore libv4l directly [github](https://github.com/philips/libv4l) (with `-lv4l2`)
- [ ] Bonus
  - [ ] Record show as text files
  - [ ] Play from record text file
  - [ ] Try to write NanoKontrol config