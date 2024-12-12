## Unreleased

###### Gameplay
- Fixed rocket pads resetting after save-load cycle.

###### User Interface
- Fixed the camera position when starting a game.
- Removed construction ID numbers.

###### Internal
- [SECURITY] Fixed a heap overflow in game loading.
- Added libxml++ dependency for load/save parsing.
- Changed the load/save format. (Games saved with previous versions of
  LinCity-NG are incompatible with the current version and vise-versa.)
- Added a script to convert games from the load/save format of version 2.12.x.
- Changed starting scenarios to current load/save format.
- Fixed an issue where games may not load correctly on a different platform than
  the one used for saving.
- Changed the C++ standard from C++11 to C++17.
- Removed ~/.lincity-ng from the search path.
- Added BetaTest and DebugOpt build configurations.

###### Documentation / Translation
