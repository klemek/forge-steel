pkgname=forge
pkgver=0.0.0
pkgrel=1
pkgdesc="Fusion Of Real Time Generative Effects"
arch=('i686' 'pentium4' 'x86_64' 'arm' 'armv7h' 'armv6h' 'aarch64' 'riscv64')
depends=('glfw>=1:3.0', 'v4l-utils>=1.30', 'alsa-lib>=1.2.14')
url="https://github.com/klemek/forge"
source=("${pkgname}-${pkgver}.tar.gz::https://github.com/klemek/forge/releases/download/v${pkgver}/${pkgname}-${pkgver}.tar.gz")
sha256sums=('TODO')
srcdir=build

build() {
  cd "$srcdir/$pkgname-$pkgver"
  ./configure --prefix=/usr
  make
}

package() {
  cd "$srcdir/$pkgname-$pkgver"
  sudo make DESTDIR="$pkgdir" install
}
