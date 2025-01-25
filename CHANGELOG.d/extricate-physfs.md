## Unreleased

###### Gameplay

###### User Interface
- Added `--config`, `--app-data`, and `--user-data` command-line options for
  specifying custom paths.

###### Internal
- Removed PhysFS library dependency in favor of C++17 Filesystem library.
- Changed detection of app data location to use `CMAKE_INSTALL_PREFIX`. Note:
  `--install-prefix` may need to be specified at configure time with the
  expected install location; otherwise, the default app data directory will be
  incorrect.
- Added cfgpath library for detecting config and user data locations.
- Changed format and location of config file.
- Added `appDataDir` and `userDataDir` config options for storing non-standard
  app data and user data locations.

###### Documentation / Translation
