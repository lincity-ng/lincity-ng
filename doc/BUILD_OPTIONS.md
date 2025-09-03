# Build Configuration Options

This is a list of options which may be of interest when configuring the build.
These options should be passed to `cmake` at the configuration step.


## CMAKE_BUILD_TYPE

```
cmake -B build -DCMAKE_BUILD_TYPE=<build-type>
```

Selects which debug features and optimization level to use. Values are
case-insensitive. Default is `Release`.

Allowed values are:
- `Release`: This is for regular users or anyone not interested in debugging.
  This enables the highest optimization level (-O3), and disables all features
  that are only for development and/or testing. This is the default.
- `MinSizeRel`: This tries to make the binary as small as possible.
  It is the same as `Release` except the -Os optimization level is used.
- `RelWithDebInfo`: This is for debugging issues in the `Release` build type
  that may not be present with lower optimization levels. It is also useful for
  running performance analysis. It is the same as `Release` except debug symbols
  are included in the binary and compile-time warnings are enabled.
- `BetaTest`: This is for beta-testing.
  This enables the highest optimization level (-O3), includes debug symbols in
  the binary, and enables runtime assertions. Use this only if you intend to
  report issues that you find. The runtime assertions will cause the game
  to crash when they fail, so this is just unnecessary inconvenience if you
  don't report the failed assertion anyway.
- `Debug`: This is for general development.
  This enables the lowest optimization level (-O0) and all development and
  testing features. This includes debug symbols, runtime-assertions, and debug
  logging to the console. The low optimization level is used to shorten build
  times as much as possible
- `DebugOpt`: This is for development when a faster binary is useful.
  This is the same as `Debug` except the -O2 optimization level is used.
- Specifying any other build type not listed above will prevent
  per-configuration build flags from being used. This may be useful when
  supplying flags via `CMAKE_<LANG>_FLAGS`.


## CMAKE_INSTALL_PREFIX

```
cmake -B build -DCMAKE_INSTALL_PREFIX=<prefix>
```

Sets the installation prefix of the build. The default is system dependent.

The install prefix is compiled into the binary and used at runtime to help find
the app data directory. If later installed to a different prefix, then the user
must set the app data directory manually either via the `--app-data-dir` command
line option or via the [config file](LINCITY-NG_CONFIG.md#app-data-directory).

This may alternatively be specified via the `--install-prefix` command line
option.

See also: [CMAKE_INSTALL_PREFIX](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html)


#### LINCITYNG_VERSION_SUFFIX

```
cmake -B build -DLINCITYNG_VERSION_SUFFIX=<suffix>
```

The `-DLINCITYNG_VERSION_SUFFIX=<suffix>` configure time option is provided
for packagers to allow marking builds as coming from a specific source. It's
especially useful when building from a source tarball or with custom patches to
avoid an `unknown` or `dirty` suffix.


## ENABLE_NLS

```
cmake -B build -DENABLE_NLS=<YES/NO>
```

Enables or disables native language support. Disabling NLS removes the `libintl`
and `gettext` dependencies, and the build will not support translation to other
languages. Default is `YES`, i.e. NLS is enabled.


## LINGUAS

```
cmake -B build -DLINGUAS=<list of languages>
```

If defined, only the listed language translations will be built and installed.
Default is to build all supported languages.
