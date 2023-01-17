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

The game depends on some libraries to be present, before you can start building it,
but if you use `vcpkg` (check the [Building](#building) section)
this is taken care of for you.

Here's a list of them:

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

If you use packages from you distribution pay attention that you need
the header files in addition to the libraries. They are often in 
separate `*-dev` packages. Eg. for `physfs` you need `libphysfs` and `libphysfs-dev`.

### Building

To build the game:

    cmake -B out --preset vcpkg --config Debug
    cmake --build out --parallel

`out` can be replaced with any other folder you want, and config can be `Debug` or `Release`.

To install the game:

    cmake --install out --prefix <target directory> --config Debug

A package (currently zip) can be created with:

    cmake --build out --target package --config Debug

CMakePresets can also be used (build directory will be `builds/vcpkg`):

    cmake --preset vcpkg                    # To configure

    cmake --build --preset vcpkg            # To build
    cmake --build --preset vcpkg-package    # To package

### Starting the game

You can start the game by typing

    lincity-ng

### Exit the game

If you are in the main menu, you can quit the program also by
pressing 'ESCAPE' or '^c'. This might be a way to terminate the
program even if the selected video-mode is not working as expected.

## Gameplay

See the ingame help [F1] for more details.

### Objective

You can win Lincity either by building a sustainable economy or
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


 * arrow keys or WASD: more ways to scroll


 * use shift key to scroll faster


 * h  to hide high buildings. Press h again to show them.
 * v  to cycle through MiniMap-overlay modes
 * b  toggle between current tool and bulldoze mode 
 * g  toggle between right click showing building or tile info
 * F1 Help
 
 * F12 quick save
 * F9  quick load

## Contact

Bugs should be reported to:
    https://github.com/lincity-ng/lincity-ng/issues
    
You can contact us at the lincity-ng-devel mailinglist:
    http://lists.fedorahosted.org/mailman/listinfo/lincity-ng-devel
or you might be able to catch us in irc at irc.freenode.net #lincity.

Visit our homepage: https://github.com/lincity-ng/lincity-ng
