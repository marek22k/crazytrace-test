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

### Building with Sanitizer

To build the project with a sanitizer, you can select this in `make` and compile the project again:
```
$ make addresssanitizer
$ make compile
```

The following sanitizers are available:
- `addresssanitizer`
- `leaksanitizer`
- `undefinedsanitizer`

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
