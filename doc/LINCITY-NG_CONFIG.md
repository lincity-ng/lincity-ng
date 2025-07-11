# LinCity-NG Runtime Config

This is a comprehensive list of configuration options for LinCity-NG.

Most options may be specified on the command line, in a config file, or in the
in-game options menu. Changes made in the in-game options menu take effect
immediately, and write the change to the config file to make it persistent.
Options given on the command line take precedence over the config file.

The default config file location is system dependent. Typical locations are:
- Linux: `/home/<user>/.config/lincity-ng.conf`
- Windows: `C:\Users\<user>\AppData\Roaming\lincity-ng.ini`
- Mac: `/Users/<user>/Library/Application Support/appname.conf`


## Config File Location

**Command Line Options:** `-c <file>` or `--config <file>`
<br/>
**Config File Elements:** This option may not be specified in the config file.
<br/>
**Allowed Values:** file path
<br/>
**Default:** _system-dependent_

Specifies the configuration file location. This option may only be given once.
If not specified, then a system-dependent default location is used. If the file
does not exist, then it is created.


## Video Mode

**Command Line Options:** `-g`, `--gl`, `-s`, or `--sdl`
<br/>
**Config File Elements:** `<useOpenGL>`
<br/>
**Allowed Values:** `yes`/`no`/`default`
<br/>
**Default:** `no`, i.e. SDL mode

Switches between GL or SDL video mode. If specified more than once on the
command line, the last option takes precedence. Not all modes are supported in
all versions/builds.


## Fullscreen/Windowed

**Command Line Options:** `-f`, `--fullscreen`, `-w`, or `--window`
<br/>
**Config File Elements:** `<fullscreen>`
<br/>
**Allowed Values:** `yes`/`no`/`default`
<br/>
**Default:** `yes`, i.e. fullscreen

Toggles fullscreen/windowed display mode. If specified more than once on the
command line, the last option takes precedence.


## Window Size

**Command Line Options:** `-S <width>x<height>` or `--size <width>x<height>`
<br/>
**Config File Elements:** `<x>` and `<y>`
<br/>
**Allowed Values:** positive decimal integers
<br/>
**Default:** 1024x768

Sets the window dimensions for windowed mode. This option has no effect in
fullscreen mode.


## Mute Sound

**Command Line Options:** `-m` or `--mute`
<br/>
**Config File Elements:** `<soundEnabled>` and `<musicEnabled>`
<br/>
**Allowed Values:** `yes`/`no`/`default`
<br/>
**Default:** `yes`, i.e. not muted

Mutes sound effects and/or music.


## Sound Volume

**Command Line Options:** _none_
<br/>
**Config File Elements:** `<soundVolume>` and `<musicVolume>`
<br/>
**Allowed Values:** integers 0 to 100
<br/>
**Default:** `100` for sound FX, `50` for music

Sets the sound/music volume percent.


## Music Theme

**Command Line Options:** _none_
<br/>
**Config File Elements:** `<musicTheme>`
<br/>
**Allowed Values:** string
<br/>
**Default:** `default`

Sets the music theme. Typically, the only available theme is `default`. However,
other music themes may be added.


## Language

**Command Line Options:** _none_
<br/>
**Config File Elements:** `<language>`
<br/>
**Allowed Values:** 2-character language code with an optional region ID
<br/>
**Default:** `autodetect`

Sets the user's preferred language. If the special value `autodetect` is
specified, LinCity-NG will detect the preferred language from the environment.
This option has no effect if native language support is disabled for the build.


## Map Size

**Command Line Options:** _none_
<br/>
**Config File Elements:** `<WorldSideLen>`
<br/>
**Allowed Values:** positive integers
<br/>
**Default:** `100`

Sets the size of newly generated maps. This does not affect the size of
pre-built scenarios.


## Car Animations

**Command Line Options:** _none_
<br/>
**Config File Elements:** `<carsEnabled>`
<br/>
**Allowed Values:** `yes`/`no`/`default`
<br/>
**Default:** `yes`, i.e. car animations enabled

Enables/disables in-game vehicle animations. These animations are purely
cosmetic and do not affect gameplay.


## App Data Directory

**Command Line Options:** `--app-data-dir <directory>`
<br/>
**Config File Elements:** `<appDataDir>`
<br/>
**Allowed Values:** directory path
<br/>
**Default:** _build-dependent_

Sets the directory location to search for game assets. The default location is
`<prefix>/share/lincity-ng` where `<prefix>` is the installation prefix
specified at the _configure_ build step.

Typical install prefixes are:
- Unix: `/usr`, `/usr/local`, `/home/<user>/.local`
- Windows: `C:\Program Files\lincity-ng`

## User Data Directory

**Command Line Options:** `--user-data-dir <directory>`
<br/>
**Config File Elements:** `<userDataDir>`
<br/>
**Allowed Values:** directory path
<br/>
**Default:** _system-dependent_

Sets the directory location to search for game assets. The default location is
system-dependent.

Typical defaults are:
- Linux: `/home/<user>/.local/share/lincity-ng/`
- Windows: `C:\Users\<user>\AppData\Roaming\lincity-ng-data\`
- Mac: `/Users/<user>/Library/Application Support/lincity-ng-data/`
