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

* PhysicsFS 2.1.0 or later (use stable, not development branch)

  http://www.icculus.org/physfs/

* zlib 1.0 or later

  http://www.gzip.org/zlib/

* libxml 2.6.11 or later

  http://xmlsoft.org/

On Ubuntu, install all these by running
```
apt-get install libsdl2-2.0-0 libsdl2-gfx-1.0-0 libsdl2-image-2.0-0 libsdl2-mixer-2.0-0 libsdl2-ttf-2.0-0 libphysfs1 zlib1g libxml++2.6-2v5
```

### Build Dependencies

* A C++ compiler supporting C++11 (such as gcc)

  https://gcc.gnu.org/

* git

  https://git-scm.com/

* CMake 3.21 or later

  https://cmake.org/

* LibXslt (with xsltproc)

  https://gitlab.gnome.org/GNOME/libxslt

* gettext

  https://www.gnu.org/software/gettext/

* Header files for all [run dependencies](#run-dependencies)

  If you use packages from your distribution, header files are often in separate
  `*-dev` packages. E.g. for zlib you may need `zlib1g` and `zlib1g-dev`.

On Ubuntu, install all build dependencies by running
```
apt-get install build-essential cmake libsdl2-dev libsdl2-gfx-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libphysfs-dev zlib1g-dev libxml++2.6-dev libxslt1.1 libxslt1-dev xsltproc gettext git
```

### Building

To clone, configure, build, and install:
```
git clone https://github.com/lincity-ng/lincity-ng.git
cd lincity-ng
cmake -B build
cmake --build build --parallel
sudo cmake --install build  # optional
```

To create a package (currently tar.gz and zip):
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target package
```

Run `cmake --help` to see all the available options.

### Running

To run the game from the source directory without installing:
```
build/bin/lincity-ng
```

When the game is installed, you may run it with:
```
lincity-ng
```


## Contact

View the project on GitHub: https://github.com/lincity-ng/lincity-ng.

Report bugs and suggest features:
  https://github.com/lincity-ng/lincity-ng/issues.
