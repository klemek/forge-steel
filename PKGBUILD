pkgname=forge
pkgver=0.0.0
pkgrel=1
pkgdesc="Fusion Of Real Time Generative Effects"
arch=('i686' 'pentium4' 'x86_64' 'arm' 'armv7h' 'armv6h' 'aarch64' 'riscv64')
depends=('glfw>=1:3.0', 'v4l-utils>=1.30', 'alsa-lib>=1.2.14')
url="https://github.com/klemek/forge-steel"
source=("${pkgname}-steel-${pkgver}.tar.gz::https://github.com/klemek/forge-steel/releases/download/steel-v${pkgver}/${pkgname}-steel-${pkgver}.tar.gz")
sha256sums=('71e2db98e1346b101d175535c30875f28eb21432f4147ecc2b40c9da3973f5fb')
srcdir=build
backup=("usr/share/${pkgname}")

build() {
  cd "$srcdir/$pkgname-$pkgver"
  ./configure --prefix=/usr
  make
}

package() {
  cd "$srcdir/$pkgname-$pkgver"
  sudo make DESTDIR="$pkgdir" install
}
