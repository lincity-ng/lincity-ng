# LinCity-NG: A city simulation game

https://github.com/lincity-ng/lincity-ng

## Introduction

LinCity-NG is a city simulation game. It is a polished and improved
version of the classic LinCity game. In the game, you are required
to build and maintain a city. You can win the game either by
building a sustainable economy or by evacuating all citizens with
spaceships.

See the in-game help [F1] for how to play.

## Building and Installation

**Note:**
For best results, follow the instructions included in your version of the
source. That is, consult your local version of README.md. Following instructions
for a different version than what you are building can cause build failures.

### Run Dependencies

* SDL2 2.0.0 or later

  http://www.libsdl.org

* SDL2_mixer 2.0.0 or later (with ogg-support enabled)

  http://www.libsdl.org/projects/SDL_mixer/

* SDL2_image 2.0.0 or later (with png support)

  http://www.libsdl.org/projects/SDL_image/

* SDL2_ttf 2.0.12 or later

  http://www.libsdl.org/projects/SDL_ttf/

* SDL2_gfx 1.0.0 or later

  http://www.ferzkopp.net/~aschiffler/Software/SDL_gfx-2.0

* zlib 1.0 or later

  https://zlib.net/

* libxml 2.6.11 or later (recommended 2.13.0 or later)

  http://xmlsoft.org/

* libxml++ 5.0

  https://libxmlplusplus.github.io/libxmlplusplus/

### Build Dependencies

* A C++ compiler supporting C++17 (such as gcc)

  https://gcc.gnu.org/

* git

  https://git-scm.com/

* CMake 3.21 or later

  https://cmake.org/

* LibXslt (with xsltproc)

  https://gitlab.gnome.org/GNOME/libxslt

* Header files for all [run dependencies](#run-dependencies)

  If you use packages from your distribution, header files are often in separate
  `*-dev` packages. E.g. for zlib you may need `zlib1g` and `zlib1g-dev`.

* Include What You Use (optional)

  https://include-what-you-use.org/

* gettext (optional)

  https://www.gnu.org/software/gettext/

### Building

To clone, configure, build, and install:
```
git clone https://github.com/lincity-ng/lincity-ng.git
cd lincity-ng
cmake -B build                  # configure
cmake --build build --parallel  # build
sudo cmake --install build      # install
```

To create a package:
```
cmake -B build -DCMAKE_BUILD_TYPE=Release --install-prefix <expected install path>
cmake --build build --parallel --target package
```

#### CMAKE_BUILD_TYPE
The `-DCMAKE_BUILD_TYPE=<build-type>` option at configure time selects which
debug features and optimization level to use.

Allowed values are:
- `RELEASE`: This is for regular users or anyone not interested in debugging.
  This enables the highest optimization level (-O3), and disables all features
  that are only for development and/or testing.
- `MINSIZEREL`: This tries to make the binary as small as possible.
  It is the same as `RELEASE` except the -Os optimization level is used.
- `RELWITHDEBINFO`: This is for debugging issues in the `RELEASE` build type
  that may not be present with lower optimization levels. It is also useful for
  running performance analysis. It is the same as `RELEASE` except debug symbols
  are included in the binary and compile-time warnings are enabled.
- `BETATEST`: This is for beta-testing.
  This enables the highest optimization level (-O3), includes debug symbols in
  the binary, and enables runtime assertions. Use this only if you intend to
  report issues that you find. The runtime assertions will cause the game
  to crash when they fail, so this is just unnecessary inconvenience if you
  don't report the failed assertion anyway.
- `DEBUG`: This is for general development.
  This enables the lowest optimization level (-O0) and all development and testing
  features. This includes debug symbols, runtime-assertions, and debug logging
  to the console. The low optimization level is used to shorten build times as
  much as possible
- `DEBUGOPT`: This is for development when a faster binary is useful.
  This is the same as `DEBUG` except the -O2 optimization level is used.

### Running

To run the game from the source directory without installing:
```
./build/bin/lincity-ng --app-data build/share/lincity-ng
```

When the game is installed, you may run it with:
```
lincity-ng
```

## Loading games from versions prior to 2.13.0

LinCity-NG 2.13.0 comes with a completely new load/save system that is,
unfortunately, not backward compatible. (Some old code just needed to go.) The
good news is all your hard work toward your super-city-utopia is not lost.

You can use the script located at `contrib/ldsv-format-convert/1328-to-2130.sh`
in the source code (or `/usr/share/lincity-ng/1328-to-2130.sh` on GNU/Linux
systems) to convert your game to the new format. The script converts games saved
with Lincity-NG 2.12.x. And the script is likely to also work with games created
with earlier versions if you had "binary saving" and "seed saving" options
disabled.

Before using the script, first make a backup of the games you want to convert:
```
mv ~/.local/share/lincity-ng/<my-lincity-ng-game>.gz{,.orig}
```

Then use use the script by passing the old game as standard input, and saving
the new game to standard output:
```
./contrib/ldsv-format-convert/1328-to-2130.sh \
  < ~/.local/share/lincity-ng/<my-lincity-ng-game>.gz.orig \
  > ~/.local/share/lincity-ng/<my-lincity-ng-game>.gz
```

If your game is _very_ old, you may find it in the `~/.lincity-ng` directory
instead.

If the script gives an error, then most likely the game was saved in a format
that is incompatible with the script. To fix this, fire up a compatible version
of LinCity-NG (2.12.x recommended, but prior versions may work if
"binary saving" and "seed saving" are disabled), and load then save your game.
Once you do this, the recently-saved game should work with the script.

After using the script to convert your game, upon loading it with version
2.13.x, you should not see any warnings in the console -- particularly warnings
about unexpected elements. If you see any warnings, then please [report the
issue](https://github.com/lincity-ng/lincity-ng/issues/new) and include the file
you were trying to load. Such warnings could be indicative of critical errors in
the conversion script.

## Contact

View the project on GitHub: https://github.com/lincity-ng/lincity-ng.

Report bugs and suggest features:
  https://github.com/lincity-ng/lincity-ng/issues.
