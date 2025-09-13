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
usage: forge [-h] [-v] [-hr] [-s=SCREEN] [-f=FRAG_PATH] 

Fusion Of Real-time Generative Effects.

options:
  -h, --help         show this help message and exit
  -v, --version      print version
  -hr, --hot-reload  hot reload of shaders scripts
  -s, --screen       output screen number (default: primary)
  -f, --frag         fragment shader path (default: TODO)
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
  - [x] Clean code
- [ ] Multi-stage shaders
  - [ ] Test 2 stages with render to texture
  - [ ] 2 in 2 fx 1 mix 1 fx layout
  - [ ] Include fragments with special directive
  - [ ] 16 input + 16 fx definition and selection (with param)
  - [ ] Feedback texture
  - [ ] Clean code
- [ ] Midi
  - [ ] Read Midi events
  - [ ] Read midi mapping config file
  - [ ] Write Midi events
  - [ ] Save midi state
  - [ ] Tap-tempo feature
  - [ ] Clean code
- [ ] Video input
  - [ ] Fixed camera video
  - [ ] Video mapping config file
  - [ ] Clean code
- [ ] Monitor screen
  - [ ] 2nd window
  - [ ] Use buffers as panels (INA A FXA / DEBUG A+B FXA+B / INB B FXB)
  - [ ] Clean code
- [ ] Packaging & install
  - [ ] Clone "shaders" and config in system path at setup