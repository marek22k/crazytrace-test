
setup:
	meson setup src build

compile:
	meson compile -C build

install: setup compile
	meson install -C build

debian:
	cd deb && ./build.sh

check:
	cppcheck --project=crazytrace.cppcheck --enable=warning,style,information,missingInclude --error-exitcode=1
