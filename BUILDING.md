# Building

crazytrace uses meson as a build system. However, there is also a Makefile that contains frequently used routines.

## Dependencies

The libraries used are listed in the README. To install them on Debian, the following command can be used:
```
$ sudo apt-get --yes install build-essential git meson ninja-build libboost1.81-all-dev libyaml-cpp-dev libtins-dev
```

The following packages are required to analyze, lint and format the code:
```
$ sudo apt-get --yes install clang-tidy clang-format cppcheck flawfinder
```
To get the latest release of LLVM or Clang, llvm has its [own Debian repostitory](https://apt.llvm.org/).

The following packages are required to build the Debian package:
```
$ sudo apt-get --yes install debhelper debhelper-compat config-package-dev clang-tidy cppcheck flawfinder
```

Furthermore, libtuntap is required, which unfortunately does not have a development package in Debian.

### Building and installing libtuntap

The following packages are required to build libtuntap:
```
$ sudo apt-get --yes install build-essential git cmake
```

After that, libtuntap can be built like a normal cmake project:
```
$ git clone https://github.com/LaKabane/libtuntap.git
$ cmake -DCMAKE_BUILD_TYPE=Release -B build_libtuntap -S libtuntap
$ cmake --build build_libtuntap
$ cmake --install build_libtuntap --prefix /usr
```

## Download crazytrace

To build crazytrace, the latest version can be obtained via Git:
```
$ git clone https://codeberg.org/mark22k/crazytrace.git
$ cd crazytrace
```

## Building with the Makefile

If you want to build crazytrace without further settings you can execute `make` normally:
```
$ make
```

The following command can be used to clean up object files and the like:
```
$ make clean
```

### Tests

```
$ make test
```

### Building with Sanitizer

To build the project with a sanitizer, you can select this in `make` and compile the project again:
```
$ make sanitizer
$ make compile
```

This activates the address and the undefined sanitizer.

### Install

To install the project in `/usr/local/bin`, the following can be executed:
```
$ make install
```

### Building the Debian package

To build the Debian package, you can do the following:
```
$ make debian
```

### Linting

The following can be carried out to execute all available linters:
```
$ make check
```

Otherwise, individual linters can also be executed:
```
$ make cppcheck
$ make flawfinder
$ make clangtidy
```
(To execute clangtidy, `ninja-build` is required.)

### Format

To format the source code, the following command can be executed:
```
$ make clangformat
```
(To execute clangformat, `ninja-build` is required.)

### Getting crazytrace

The executable file is located at `./build/crazytrace`.

## Building with meson and ninja

It is also possible to build crazytrace without the Makefile wrapper.

To do this, the Meson project can be set up first:
```
$ meson setup build
```

Then crazytrace can be built:
```
$ meson compile -C build
```

Alternatively, crazytrace can be compiled using the ninja build system:
```
$ ninja -C build
```

### Tests

The tests can be executed with the following command:
```
$ meson test -C build
```

Or using the Ninja Build System:
```
$ ninja -C build test
```

### Building with Sanitizer

To build crazytrace with Sanitizer, you can set up the project with the following command:
```
$ meson setup --reconfigure --debug -Db_sanitize=address,undefined build
```

All sanitizers supported by the compiler or Meson are available. The exact ones can be found in the documentation of meson and the compiler used.

### Install

crazytrace can be installed with the following command:
```
$ meson install -C build
```

Or using the Ninja Build System:
```
$ ninja -C build install
```

### Linting

cppcheck:
```
$ meson compile -C build cppcheck
```

or via the ninja build system:
```
$ ninja -C build cppcheck
```

flawfinder:
```
$ meson compile -C build flawfinder
```

or via the ninja build system:
```
$ ninja -C build flawfinder
```

clang-tidy:
```
$ ninja -C build clang-tidy
```
I am currently not aware of any way to run clang-tidy via meson without ninja. A [demand](https://github.com/mesonbuild/meson/issues/2383#issuecomment-2002148039) has been created.

### Format

To format the source code, the following command can be executed:
```
$ ninja -C build clang-format
```

I am currently not aware of any way to run clang-format via meson without ninja.

### Getting crazytrace

The executable file is available under the name `crazytrace` in the build directory.

## Building the Debian package without Makefile-wrapper

To build the binary crazytrace Debian package, the following command can be executed:
```
$ dpkg-buildpackage -b
```
