pkgname=forge-steel
pkgver=1.0.1
pkgrel=1
pkgdesc="Fusion Of Real Time Generative Effects"
arch=('i686' 'pentium4' 'x86_64' 'arm' 'armv7h' 'armv6h' 'aarch64' 'riscv64')
depends=('glfw>=1:3', 'v4l-utils>=1.32', 'alsa-lib>=1.2', 'libglvnd>=1.7')
url="https://github.com/klemek/forge-steel"
source=("${pkgname}-steel-${pkgver}.tar.gz::https://github.com/klemek/forge-steel/releases/download/v${pkgver}/${pkgname}-${pkgver}.tar.gz")
sha256sums=('b2345c1a87f6b5b13dcce6a237a3f9a9ddb7fa9c3e982f5e164e7d7fef5725f3')
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
