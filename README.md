[![CI](https://github.com/klemek/forge-steel/actions/workflows/ci.yml/badge.svg)](https://github.com/klemek/forge-steel/actions/workflows/ci.yml)

# F.O.R.G.E. (Steel)

> **F**usion **O**f **R**eal **T**ime **G**enerative **E**ffects

*Steel version (linux executable)*

TODO

## Install

### Prerequisites

* libglfw
* libGL
* libasound2
* libv4l

### From release

See [Releases](https://github.com/klemek/forge-steel/releases)

```shell
tar xvzf forge-steel-x.y.z.tar.gz
cd forge-steel-x.y.z
./configure
make
make install
```

### From repository (PKGBUILD)

```shell
git clone --recursive https://github.com/klemek/forge-steel
cd forge
makepkg -si
```


### From repository (dev version)

```shell
git clone --recursive https://github.com/klemek/forge-steel
cd forge
aclocal
autoconf
automake --add-missing
./configure
make
make install
```

## CLI arguments

```txt
usage: forge [-h] [-v] [-hr] [-s=SCREEN] [-m=SCREEN] [-mo] [-f=DIR_PATH] [-c=CFG_PATH] [-sf=STATE_PATH] [-ls / -nls] [-ss / -nss] [-is=SIZE] [-v=FILE] [-vs=SIZE] [-t=TEMPO] [--demo] [-w] 

Fusion Of Real-time Generative Effects.

options:
  -h, --help                show this help message and exit
  -v, --version             print version
  -hr, --hot-reload         hot reload of shaders scripts
  -s, --screen              output screen number (default: primary)
  -m, --monitor             monitor screen number (default: none)
  -mo, --monitor-only       no output screen
  -f, --frag                fragment shaders directory (default: /usr/share/forge/shaders)
  -c, --config              fragment shaders config file (default: /usr/share/forge/default.cfg)
  -sf, --state-file         saved state file (default: forge_saved_state.txt)
  -ls, --load-state         load saved state (default)
  -nls, --no-load-state     do not load saved state
  -ss, --save-state         save state (default)
  -nss, --no-save-state     do not save state
  -is, --internal-size      internal texture height (default: 720)
  -v, --video-in            path to video capture device (multiple allowed)
  -vs, --video-size         video capture desired height (default: internal texture height)
  -t, --tempo               base tempo (default: 60)
  --demo                    demonstration mode (assume --no-save-state and --no-load-state)
  -w, --windowed            not fullscreen
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
- [ ] Midi
  - [x] Read Midi events
  - [x] Read midi mapping config file
  - [x] Write Midi events
  - [x] Send midi data to shaders
  - [x] Save midi state
  - [x] Load midi state from last save
  - [x] State machine with A/B switch
  - [x] Tap-tempo feature
  - [ ] Clean code and fix things
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
- [ ] Other
  - [ ] Update readme with usage documentation
  - [ ] Documentation in default config file
  - [x] Clone "shaders" and config in system path at setup
  - [ ] Find and fix opengl errors 0500 ?
  - [ ] Investigate video device fps loss (bad unregister ?)
- [ ] Bonus
  - [ ] Record show as text files
  - [ ] Play from record text file
  - [ ] Try to write NanoKontrol config