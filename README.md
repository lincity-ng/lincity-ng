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

For best results, follow the instructions included in your version of the
source. That is, consult your local version of README.md. Following instructions
for a different version than what you are building can cause build failures.
Notably, the build system was changed between versions 2.9.0 and 2.10.0, so
following instructions for the wrong build system will definitely cause issues.

### Dependencies

See [DEPENDENCIES.md](doc/DEPENDENCIES.md) for the list of dependencies.

### Building

To clone, configure, build, and install:
```
git clone https://github.com/lincity-ng/lincity-ng.git
cd lincity-ng
cmake -B build -DCMAKE_BUILD_TYPE=Release  # configure
cmake --build build --parallel             # build
sudo cmake --install build                 # install
```

To create a package:
```
cmake -B build --install-prefix <expected install path>
cmake --build build --parallel --target package
```

See [CONFIGURE_OPTIONS.md](doc/CONFIGURE_OPTIONS.md) for a list of build
configuration options.

### Running

To run the game from the source directory without installing:
```
./build/bin/lincity-ng --app-data build/share/lincity-ng
```

When the game is installed, you may run it with:
```
lincity-ng
```

## Alternative Binary Releases

As an alternative to building from source, there are several places to get
pre-built binary packages:

- Linux distribution repositories. This is likely the best way to install
  LinCity-NG pre-built binaries for Linux users. However, LTS and stable
  distributions, by their very nature, may not offer the latest version of
  LinCity-NG.
- [Flathub](https://flathub.org/apps/io.github.lincity_ng.lincity-ng). As of
  version 2.14.1, LinCity-NG is available on Flathub.
- [GitHub](https://github.com/lincity-ng/lincity-ng/releases). Binary packages
  for both Linux and Windows are available on GitHub. However, these binaries
  may not work for everyone due to strict library version requirements.


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
