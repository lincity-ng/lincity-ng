## Unreleased

###### Gameplay

###### User Interface
- Fixed a bug where users could pick a negative map size in settings
- Fixed a bug where the application would not launch if the window was sized too
  small in the previous session.

###### Internal
- Added the following methods for input validation to Config::Option:
  - `set()`
  - `trySet()`
  - `setConfig()`
  - `trySetConfig()`
- Added public `std::function` `validator` and assigned it in `Config.cpp`
- Replaced `validateRange()` from `Config.cpp` by the new methods above

###### Documentation / Translation
