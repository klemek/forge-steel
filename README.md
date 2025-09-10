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
git clone https://github.com/klemek/forge
cd forge
forge -si
```


### From repository (dev version)

```shell
git clone https://github.com/klemek/forge
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
usage: forge [--help] [-v]

Fusion Of Real-time Generative Effects.

options:
  --help             show this help message and exit
  -v, --version      print version
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
sha256sum forge-x.y.z.tar.gz
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

- [ ] Basics
  - [x] Create GLSL Window
  - [x] Load static fragment shader into GLSL
  - [x] Add default uniforms
  - [ ] Read fragment shader from file
  - [ ] Minimal working fragment sample
  - [ ] Hot-reload fragment shader
  - [ ] Force fullscreen
  - [ ] Select screen as argument
- [ ] Midi
  - [ ] Read Midi events
  - [ ] Read midi mapping config file
  - [ ] Write Midi events
- [ ] MVP
  - [ ] Add required libraries in target build
  - [ ] Include fragments with special directive
  - [ ] Source mixing
  - [ ] Save midi state
- [ ] Advanced
  - [ ] Video input
  - [ ] Feedback input
  - [ ] Tap-tempo feature