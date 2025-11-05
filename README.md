[![CI](https://github.com/klemek/forge-steel/actions/workflows/ci.yml/badge.svg)](https://github.com/klemek/forge-steel/actions/workflows/ci.yml)

# F.O.R.G.E. (Steel)

> **F**usion **O**f **R**eal **T**ime **G**enerative **E**ffects

*Steel version (PC executable)*

TODO cool image and youtube link

## Table of Contents

- [F.O.R.G.E. (Steel)](#forge-steel)
  - [Table of Contents](#table-of-contents)
  - [What is FORGE ?](#what-is-forge-)
  - [Install](#install)
    - [Prerequisites](#prerequisites)
    - [From release](#from-release)
    - [From repository (PKGBUILD)](#from-repository-pkgbuild)
    - [From repository (dev version)](#from-repository-dev-version)
  - [Usage](#usage)
    - [CLI arguments](#cli-arguments)
    - [Included Shaders and Config](#included-shaders-and-config)
    - [Making your own FORGE config](#making-your-own-forge-config)
    - [Working with `frag0.glsl`](#working-with-frag0glsl)
  - [Frequently Asked Questions](#frequently-asked-questions)
    - [Why "steel"?](#why-steel)
    - [How do I report a bug?](#how-do-i-report-a-bug)
    - [Help I got low FPS on my video device](#help-i-got-low-fps-on-my-video-device)

## What is FORGE ?

![](images/forge.drawio.png)

TODO

## Install

### Prerequisites

| library | apt | pacman |
| --- | --- | --- |
| libglfw | libglfw3-dev | extra/glfw |
| libGL | libgl-dev | extra/libglvnd |
| libasound | libasound2-dev | extra/alsa-lib |
| libv4l2 | libv4l-dev | extra/v4l-utils |

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

## Usage

### CLI arguments

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
  --trace-midi              print midi code and values
  --trace-fps               print fps status of subsystems
```

### Included Shaders and Config

TODO

### Making your own FORGE config

TODO

### Working with `frag0.glsl`

TODO

## Frequently Asked Questions

### Why "steel"?

FORGE is a concept which is intended to be derived onto other forms (like Raspberry PI). In order to differentiate between them each would be name after a different alloy.

FORGE (steel) describe the linux version.

### How do I report a bug?

TODO

### Help I got low FPS on my video device

TODO
