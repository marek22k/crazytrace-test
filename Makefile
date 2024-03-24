.PHONY: all setup addresssanitizer leaksanitizer undefinedsanitizer clean compile coverage install debian check cppcheck flawfinder clangtidy test clangformat

all: setup compile

setup:
	test -d build || meson setup build

sanitizer:
	meson setup --reconfigure --debug -Db_sanitize=address,undefined build

setupcoverage:
	meson setup --reconfigure -Db_coverage=true build

clean: setup
	meson compile --clean -C build

compile: setup
	meson compile -C build

coverage: setupcoverage test
	ninja -C build coverage

install: setup compile
	meson install -C build

debian:
	dpkg-buildpackage -b

check: cppcheck flawfinder clangtidy

cppcheck: setup
	meson compile -C build cppcheck

flawfinder: setup
	meson compile -C build flawfinder

clangtidy: setup
	ninja -C build clang-tidy

test: setup
	meson test -C build

clangformat: setup
	ninja -C build clang-format
