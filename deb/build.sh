#!/bin/sh

die () {
    >&2 echo "$1"
    exit 1
}


(
    rm -rf package || die "Failed to remove old packages"
    rm -rf build || die "Failed to remove build directory"
)

(
    meson setup --prefix=/usr ../src/ build || die "Failed to configure project"
    meson compile -C build || die "Failed to build project"
    meson install -C build --destdir "../package/" || die "Failed to install project to package directory"
)

mkdir -p package/DEBIAN/ || die "Failed to make create data directory"
VERSION="$(date +%s)-$(git describe --long --always)"
[ "$VERSION" ] || die "Failed to get version string"
ARCH="$(dpkg --print-architecture)"
[ "$ARCH" ] || die "Failed to get architecture string"

cp control.template package/DEBIAN/control || die "Failed to copy control template"
sed "s/%VERSION%/$VERSION/" -i package/DEBIAN/control || die "Failed to replace version in control template"
sed "s/%ARCHITECTURE%/$ARCH/" -i package/DEBIAN/control || die "Failed to replace architecture in control template"

mkdir -p package/usr/share/doc/crazytrace/ || die "Failed to create package meta data directory"
cp copyright package/usr/share/doc/crazytrace/copyright || die "Failed to copy copyright file"

echo "Building package..."
dpkg-deb --root-owner-group --build package crazytrace.deb || die "Failed to build package"

echo "Package $(dpkg-deb --show crazytrace.deb | sed "s/\t/ /") successful builded!"
