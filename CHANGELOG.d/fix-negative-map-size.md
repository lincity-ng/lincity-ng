## Unreleased

###### Gameplay

###### User Interface
- Fixed a bug where users could pick a negative map size in settings

###### Internal
- Added the following methods for input validation to Config::Option:
  - `setValue()`
  - `trySetValue()`
  - `setConfigValue()`
  - `trySetConfigValue()`
- Added public `std::function` `valueRange` and assigned it in `Config.cpp`
- Replaced `validateRange()` from `Config.cpp` by the new methods above

###### Documentation / Translation
