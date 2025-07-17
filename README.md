# forge

> **F**usion **O**f **R**eal **T**ime **G**enerative **E**ffects

TODO

## Install

### From release

See [Releases](https://github.com/klemek/forge/releases)

```sh
tar xvzf forge-x.y.z.tar.gz
cd forge-x.y.z
./configure
make
make install
```

### From repository (PKGBUILD)

```sh
git clone https://github.com/klemek/forge
cd forge
forge -si
```


### From repository (dev version)

```sh
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

```bash
# update configure.ac with new version
$EDITOR configure.ac
# make full build
make -f Makefile.dev release
# push to repo
git commit -am "forge vX.Y.Z"
git tag vX.Y.Z
git push origin master --tags
# create release from tag on github
# update PKGBUILD with new sha256 sum
sha256sum forge-x.y.z.tar.gz
make -f Makefile.dev release-arch
git commit -am "update arch sha256"
# add .pkg.tar.zst on the release
```

## Roadmap

TODO