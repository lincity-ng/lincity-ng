## Unreleased

###### Gameplay

###### User Interface
- Added partial language switching without restarting the game. (GUI components
  that were already translated are not yet re-translated into the new language.
  A restart is still required for that.) Notably, the pop-up dialog suggesting
  to the user to restart the game now uses the new language.

###### Internal
- Added gettext/libintl optional library dependency.
- Removed the `tinygettext` custom gettext implementation.
- Changed the installation of locale files to binary (.mo) format.
- Combined the source and GUI translation PO files into one.
- Fixed Ninja build generation.

###### Documentation / Translation
- Added documentation on build configuration options.
- Added documentation on runtime config options.
- Moved dependencies list from README.md to a separate file.
