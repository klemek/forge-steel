[![CI](https://github.com/klemek/forge/actions/workflows/ci.yml/badge.svg)](https://github.com/klemek/forge/actions/workflows/ci.yml)

# F.O.R.G.E.

> **F**usion **O**f **R**eal **T**ime **G**enerative **E**ffects

TODO

## Install

### Prerequisites

* libglfw
* libGL

### From release

See [Releases](https://github.com/klemek/forge/releases)

```shell
tar xvzf forge-x.y.z.tar.gz
cd forge-x.y.z
./configure
make
make install
```

### From repository (PKGBUILD)

```shell
git clone --recursive https://github.com/klemek/forge
cd forge
makepkg -si
```


### From repository (dev version)

```shell
git clone --recursive https://github.com/klemek/forge
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
usage: forge [-h] [-v] [-hr] [-s=SCREEN] [-m=SCREEN] [-mo] [-f=DIR_PATH] [-fc=CFG_PATH] [-is=SIZE] [-mf=FACTOR] [-t=TEMPO] [--demo] [-w] 

Fusion Of Real-time Generative Effects.

options:
  -h, --help                show this help message and exit
  -v, --version             print version
  -hr, --hot-reload         hot reload of shaders scripts
  -s, --screen              output screen number (default: primary)
  -m, --monitor             monitor screen number (default: none)
  -mo, --monitor-only       no output screen
  -f, --frag                fragment shaders directory (default: TODO)
  -fc, --frag-config        fragment shaders config file (default: TODO)
  -is, --internal-size      internal texture height (default: 720)
  -mf, --monitor-factor     monitor internal texture downscale factor (default: 3)
  -t, --tempo               base tempo (default: 60)
  --demo                    demonstration mode
  -w, --windowed            not fullscreen
```

## Release guide

```shell
# get latest version
git pull origin master
# update configure.ac with new version
$EDITOR configure.ac
# make full build
make -f Makefile.dev release
# update PKGBUILD with new version and sha256 sum
sha256sum build/forge-x.y.z.tar.gzx
$EDITOR PKGBUILD
# push to repo
git commit -am "forge vX.Y.Z"
git tag vX.Y.Z
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
  - [ ] Read Midi events
  - [ ] Read midi mapping config file
  - [ ] Write Midi events
  - [ ] Save midi state
  - [ ] State machine with A/B switch
  - [ ] Tap-tempo feature
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
  - [ ] Clone "shaders" and config in system path at setup
  - [ ] Find and fix opengl errors 0500 ?