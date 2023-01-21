# LinCity-NG: A city simulation game

https://github.com/lincity-ng/lincity-ng

## Introduction
LinCity-NG is a city simulation game. It is a polished and improved
version of the classic LinCity game. In the game, you are required
to build and maintain a city. You can win the game either by
building a sustainable economy or by evacuating all citizens with
spaceships.

## Building and Installation

These are generic instructions, please see the wiki for more specific instructions

### Prerequisites

The game depends on some libraries to be present before you can building it.

If you use `vcpkg` (see the [Building](#building) section) this is taken care
of automatically for you.

Here is the list of required packages:

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

* PhysicsFS 2.1.0 or later (use stable, NOT development branch)
  http://www.icculus.org/physfs/

* zlib 1.0 or later
  http://www.gzip.org/zlib/

* libxml 2.6.11 or later
  http://xmlsoft.org/

If you use packages from your distribution, pay attention that you need the
header files in addition to the libraries. They are often in separate `*-dev`
packages. E.g. for `physfs` you need `libphysfs` and `libphysfs-dev`.

### Building

To configure the build:
```
$ cmake -B build
```
To use vcpkg to automatically manage dependencies, add
`-DCMAKE_TOOLCHAIN_FILE=<path to vcpkg>/scripts/buildsystems/vcpkg.cmake`
to the configure command, where `<path to vcpkg>` is the path to your vcpkg
installation.

To build the game:
```
$ cmake --build out --parallel
```

To install the game:
```
# cmake --install build
```
(You may specify a different install prefix with the `--prefix` option.)

To create a package (currently zip):
```
$ cmake --build build --target package
```

Run `cmake --help` to see all the available options.

### Running

To run the game without installing:
```
$ build/bin/lincity-ng
```

When the game is installed, you may run it with:
```
$ lincity-ng
```

From the main menu, you can quit the program by pressing ESC or ^C. This can be
useful to terminate the program even if the selected video mode is not working
properly.

## Gameplay

See the ingame help [F1] for more details.

### Objective

You can win Lincity-NG either by building a sustainable economy or
by evacuating all citizens with spaceships.

### Controls

Mouse

any:
 * Click on Minimap shows the selected Area on mainscreen.

right:
 * right click on mainscreen to show info on building under cursor.
 * select tool from menu root
 * show help on tool

middle:
 * pan map on mainscreen.

left:
 * Perform action depending on selected tool. Bulldoze, show Information, construct building.
 * open Tool-Selection-Menus
 * select tool from menus

wheel:
 * up: zoom in
 * down: zoom out

motion:
 * move cursor near the screen edge to scroll main screen

Keyboard

 * ESCAPE switch to query tool


 * KP_PLUS: zoom in
 * KP_MINUS zoom out
 * KP_ENTER zoom 100%


 * KP5 put middle of the map in the center of the main screen


 * KP9 scroll main screen north
 * KP1 scroll main screen south
 * KP7 scroll main screen west
 * KP3 scroll main screen east


 * KP2 scroll main screen SE
 * KP4 scroll main screen SW
 * KP6 scroll main screen NE
 * KP8 scroll main screen NW


 * arrow keys and WASD: more ways to scroll


 * use shift key to scroll faster


 * h  to hide high buildings. Press h again to show them.
 * v  to cycle through MiniMap-overlay modes
 * b  toggle between current tool and bulldoze mode
 * g  toggle between right click showing building or tile info
 * F1 Help

 * F12 quick save
 * F9  quick load

## Contact

Please report bugs and suggest features to:
    https://github.com/lincity-ng/lincity-ng/issues

You can contact us at the lincity-ng-devel mailinglist:
    http://lists.fedorahosted.org/mailman/listinfo/lincity-ng-devel
or you might be able to catch us in irc at irc.freenode.net #lincity.

Visit our homepage: https://github.com/lincity-ng/lincity-ng
