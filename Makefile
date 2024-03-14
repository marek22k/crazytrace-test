
setup:
	meson setup src build

addresssanitizer:
	meson setup --reconfigure -Ddebug=true -Db_sanitize=address src build

leaksanitizer:
	meson setup --reconfigure -Ddebug=true -Db_sanitize=leak src build

undefinedsanitizer:
	meson setup --reconfigure -Ddebug=true -Db_sanitize=leak src build

clean:
	meson compile --clean -C build

compile:
	meson compile -C build

install: setup compile
	meson install -C build

debian:
	cd deb && ./build.sh

check: cppcheck flawfinder

cppcheck:
	meson compile -C build cppcheck

flawfinder:
	meson compile -C build flawfinder
