pkgname=forge-steel
pkgver=1.0.0
pkgrel=1
pkgdesc="Fusion Of Real Time Generative Effects"
arch=('i686' 'pentium4' 'x86_64' 'arm' 'armv7h' 'armv6h' 'aarch64' 'riscv64')
depends=('glfw>=1:3', 'v4l-utils>=1.32', 'alsa-lib>=1.2', 'libglvnd>=1.7')
url="https://github.com/klemek/forge-steel"
source=("${pkgname}-steel-${pkgver}.tar.gz::https://github.com/klemek/forge-steel/releases/download/v${pkgver}/${pkgname}-${pkgver}.tar.gz")
sha256sums=('da61207bc8af1b71544f5e35e99e7ea8dbbb72b77c0c7dc21f05ba694c28cdfa')
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
