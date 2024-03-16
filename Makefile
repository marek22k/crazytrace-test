.PHONY: all setup addresssanitizer leaksanitizer undefinedsanitizer clean compile install debian check cppcheck flawfinder

all: setup compile

setup:
	test -d build || meson setup . build

addresssanitizer: setup
	meson setup --reconfigure --debug -Db_sanitize=address . build

leaksanitizer: setup
	meson setup --reconfigure --debug -Db_sanitize=leak . build

undefinedsanitizer: setup
	meson setup --reconfigure --debug -Db_sanitize=leak . build

clean:
	meson compile --clean -C build

compile: setup
	meson compile -C build

install: setup compile
	meson install -C build

debian:
	sed "s/%VERSION%/$(shell date +%s)-$(shell git describe --long --always)/" -i debian/changelog
	sed "s/%DATE%/$(shell date --rfc-email)/" -i debian/changelog
	cat debian/changelog
	dpkg-buildpackage --build=full

check: cppcheck flawfinder

cppcheck: setup
	meson compile -C build cppcheck

flawfinder: setup
	meson compile -C build flawfinder
