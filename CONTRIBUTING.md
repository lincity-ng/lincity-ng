# Contributing to LinCity-NG

In developing LinCity-NG, my hope is it may be a pleasure and a good experience
for others. If You enjoyed playing LinCity-NG, I encourage you to give back.
There are many ways you can give back to Open Source even if you do not know how
to program, and no way is lesser or greater than another.

- You can [contribute code](#contributing-code) if you know how to program C++.
  You can add features, fix bugs, or just generally improve the code base.
- You can test the game. I actually do not play LinCity-NG very much -- mostly I
  enjoy coding it -- so I can miss even obvious issues. If you report issues,
  and suggest improvements, that is very helpful. It is especially helpful to
  test the latest `master` branch or even unmerged pull requests, so issues can
  be caught early, before they are rolled out to other users.
- You can make the game portable by developing, testing, or packaging it for
  other platforms. I use Arch Linux to develop LinCity-NG, and I do not have
  access to other platforms such as Windows or Mac, so without help, I am unable
  to detect or address issues specific to other platforms.
- You can contribute to documentation such as the readme or the in-game help. I
  believe this is an oft-forgotten way of contributing. But a user can't have
  fun with the game if they don't know how to play it, and as the game improves,
  it is easy for documentation to become outdated.
- You can [translate the game](#contributing-translation) to your own language.
  I know many languages, but all except one are programming languages, so I am
  ill-equipped to contribute in this area, and I appreciate those who do.
- You can contribute art to the game such as graphics, music, or UX design. I'm
  not particularly gifted in these areas, so I appreciate contributions from
  others.
- You can [contribute to other free and open-source projects](
  monetary-contribution). I use numerous free and open-source programs on a
  daily basis, so you can support me by supporting the software I use.


## General Contributing

### Changelog

The format of the LinCity-NG changelog is inspired by [keepachangelog.com](
https://keepachangelog.com). Whenever you contribute meaningfully to the
project, write a changelog entry with a short summary of what you changed with
respect to the most recent release.

To avoid git merge conflicts from everyone modifying the same `CHANGELOG.md`
file. Instead of directly adding to the changelog, you will write your entry in
a new file in the `CHANGELOG.d` directory. See [template.md](
CHANGELOG.d/template.md) for an example changelog entry file. After your PR is
merged, as part of routine maintenance, I will compile all the entries from the
`CHANGELOG.d` directory and add them to the main changelog.

Changelog entries should be categorized into one of four sections:
- **Gameplay** for changes affecting the game rules
- **User Interface** for changes affecting look-and-feel or controls
- **Internal** for implementation changes are more or less invisible to the user
  such as performance improvements.
- **Documentation / Translation** for updates to documentation or translation

Each entry should be written in complete sentences in the past tense and
preferably start with one of the following five keywords:
- **Fixed** for changes that fix previously broken behavior
- **Added** for new features
- **Removed** for now-removed features
- **Changed** for modifications to existing functionality
- **Deprecated** for features now planned to be removed

Additionally, add a **[SECURITY]** tag to the beginning of your entry if it
  addresses a security vulnerability.

### Credits list

If this is your first time contributing to LinCity-NG, please add your name to
the [credits list](data/gui/creditslist.xml). Add your name, the current year,
and (optionally) your email, following the same format of other entries. Note,
the entries are ordered by last name. If you already have an entry in the
credits list, take a moment to update the year and/or email address of your
entry.

### Directory Structure

- `build` - Build artifacts.
- `CHANGELOG.d` - Unreleased changelog entries.
- `contrib` - Miscellaneous unused files including old art and scripts.
- `data` - Game assets.
- `external` - External libraries.
- `mk` - Files for configure/build.
- `src` - All source code.
  - `gui` - Custom GUI library for LinCity-NG.
  - `lincity` - LinCity-NG backend library for game mechanics.
  - `lincity-ng` - LinCity-NG frontend.


## Contributing Code

You may contribute features and/or bug fixes by
[forking the project on GitHub](https://github.com/lincity-ng/lincity-ng/fork)
and [opening a pull request](https://github.com/lincity-ng/lincity-ng/compare).

### Singletons

[Singletons are evil.](https://kentonshouse.com/singletons) I have spent
considerable effort to remove singletons, so do me a favor and avoid adding more
if possible.

### Formatting

Forgive me, I can be quite picky about code formatting. These aren't necessarily
all hard and fast rules, but I might not approve your PR if I think the code is
ugly.

- Indent 2 spaces
- Open braces go on the same line with a preceding space
- Binary operators deserve a space on either side (except `.` and `->`)
- Open parenthesis is not proceeded by whitespace (except for a preceding binary
  operator)
- Line continuations get an indent
- No trailing whitespace (just a newline character)
- Unix line endings
- End files with a single newline
- No more than 80 characters to a line (not including the newline character)
- Casing:
  - Class names in PascalCase
  - Macros in MACRO_CASE
  - Enum members in MACRO_CASE
  - Everything else in camelCase
  - If you really_want_to_use_snake_case, I can excuse it for private members
    and local variables.
  - In PascalCase and camelCase, only the first letter of an acronym may be
    caps.

Much of the old code does not follow this format, so I am slowly changing it.

### Include What You Use

This project uses Include What You Use to manage `#include`s. The build system
automatically runs include-what-you-use and reports warnings. When you submit a
PR, make sure that your code does not introduce any new IWYU warnings.

To find IWYU warnings for all files, run a clean build and save the standard
error stream to a file:
```
cmake --build build --parallel --clean-first 2> >(tee build/iwyu.log)
```

To fix the warnings automatically, use `iwyu-fix-includes`:
```
iwyu-fix-includes --comments --update_comments --reorder --nosafe_headers < build/iwyu.log
```

It is also possible for IWYU warnings to exist only for certain build
configurations or platforms. For example, if you use a certain symbol only
within a `#ifdef DEBUG` context, then IWYU may report extraneous `#include`s for
release builds but not for debug builds. To prevent this, you are encouraged to
look for IWYU warnings in all relevant build configurations. If you find a
`#include` that is needed only for certain build configurations, wrap it in a
`#ifdef` and place it below other `#include`s.

### Git

In general:
 - **Rebase** when you want to incorporate local changes into remote.
 - **Merge** when you want to incorporate one branch into another.

#### Rebasing
When pulling changes from remote, local changes should be rebased onto remote
changes. This avoids having lots of small merge commits from `git pull`. This
means that when you are working on a branch locally and find that someone
recently pushed changes, then you should rebase your work on top of theirs
before pushing.

To have `git pull` do a rebase instead of a merge, add the `--rebase` option, or
you can set the `pull.rebase` configuration variable to `true`. Alternatively,
to prevent `git pull` from doing any automatic merge or rebase, use the
`--ff-only` option or set the `pull.ff` configuration variable to `only`.

```
git config pull.rebase true
git config pull.ff only
```

In general, do not rebase commits that are already pushed -- use merge instead.
This avoids the need to force-push and rewrite history which can add work for,
or worse, confuse, other developers.

#### Merging
When merging one branch into another, do not fast-forward and do not squash.
This keeps the history more clear by making it obvious that there was a merge,
and it keeps the history of the side branch clear. To avoid fast-forward, use
the `--no-ff` option.

If by chance you want/need to incorporate local changes into remote with a
_merge_ instead of a rebase, then make sure you are merging local changes into
remote changes instead of the other way around. This preserves the first-parent
history so that the history and diff is relative to the trunk instead of
relative to your changes. (FYI, `git pull` by default does this the wrong way by
merging remote into local instead of local into remote.) To properly merge your
changes on branch A into origin/A, run the following:
```
git fetch
git checkout origin/A
git merge A
# fix conflicts
git checkout -B A
git push origin A
```

## Contribute Play-Testing

You can contribute to LinCity-NG by testing it and reporting issues you find.
Here are some pointers for testing effectively:

- Test the latest master branch or open PRs (not draft). It is generally
  less useful to test older versions (e.g. the latest release) except to confirm
  or deny whether an issue on master exists in the latest release.
- Configure the build with the BetaTest build type. You select this by using
  `-DCMAKE_BUILD_TYPE=BetaTest` at configure time. This will enable runtime
  assertions so faults will be more obvious.
- Run the game in gdb. This can help with reporting crashes.
- When playing the game, make an effort to test a broad range of functionality.
  For example, test multiple kinds of starting scenarios, change settings in the
  options menu, build all types of constructions, etc. You can also test extreme
  edge cases; for example, build 10,000 rivers, save and load the game many
  times, try opening the help window when it's already open, use a small window
  size or strange aspect ratio, etc. Try to break the game.
- Report all kinds of issues that you find. This might be crashes, "buggy"
  behavior, regressions, visual issues, poor performance, confusing
  user-interface, incomplete documentation, or even feature ideas.
- When you find an issue on master, report it to
  https://github.com/lincity-ng/lincity-ng/issues. If you find an issue in a
  pull request that does not exist on master, then comment in the PR discussion
  thread. When opening the issue, include your LinCity-NG version and your
  platform/operating system. To avoid duplicate issues, search the issues list
  (including closed issues) to find whether your issue has already been
  reported; if so, then you may be able to provide additional details to the
  existing issue.

#### Checklist for reporting issues:

- Search the
  [issues list](https://github.com/lincity-ng/lincity-ng/issues?q=is%3Aissue%20)
  (including closed issues), to see if someone has already reported it.
  - If it is already reported, you may be able to provide additional details,
    confirm the issue affects multiple people, and/or confirm the issue still
    exists.
  - If it is not already reported, open a
    [new issue](https://github.com/lincity-ng/lincity-ng/issues/new).
- In the report, please include your LinCity-NG version, your platform/OS, and
  an explanation of the issue.
- For crashes, include what you did (or tried to do) that seemed to cause the
  crash, any console output related to the crash (especially warnings/errors),
  and a backtrace if available. (You can get a backtrace by running the game in
  gdb; when the game crashes, run the `bt` gdb command.)
- Include screenshots if/where applicable.


## Contributing Translation

Here is the general workflow for translating LinCity-NG:
1. Fork and clone the lincity-ng repo
2. Install [gettext](https://www.gnu.org/software/gettext/). It is important you
   do this before configuring for the first time.
3. Configure the build:
   ```
   cmake -B build
   ```
4. Get translatable strings and generate the `messages.pot` files:
   ```
   cmake --build build -t messages.pot
   ```
5. Create/Update the translation with the new strings:
   - If you are creating a new translation:
     ```
     LANG=<2-character language handle e.g. 'en'>
     msginit -i build/share/lincity-ng/locale/messages.pot -o data/locale/$LANG.po
     msginit -i build/share/lincity-ng/locale/gui/messages.pot -o data/locale/gui/$LANG.po
     ```
   - If you are updating an existing translation:
     ```
     LANG=<2-character language handle e.g. 'en'>
     msgmerge -U data/locale/$LANG.po build/share/lincity-ng/locale/messages.pot
     msgmerge -U data/locale/gui/$LANG.po build/share/lincity-ng/locale/gui/messages.pot
     ```
6. Edit the .po files with [a PO editor](
   https://www.gnu.org/software/trans-coord/manual/web-trans/html_node/PO-Editors.html)
   or with your favorite text editor.
7. If you want to edit the help texts, then create a directory with your
   language shortcut in the data/help directory (e.g. data/help/de) and write
   new xml files in your language (similar to the ones in the data/help/en
   directory). You don't need to copy over the english texts, the game will
   automatically use the english version of a text if no translated version
   exists.
8. You should also edit the lincity-ng.desktop file and add a comment entry for
   your language
9. Commit/push the changes to your fork and open a pull request against master.


## Monetary Contribution

I do not have any payment processing set up to receive donations for LinCity-NG.
But if you want to contribute monetarily, you can donate to one of these other
free and open-source projects. These projects create programs that I use on a
daily basis, so you can support me by supporting them:

- [Arch Linux](https://archlinux.org/donate/)
- [Pulsar editor](https://github.com/sponsors/pulsar-edit)
- [Free Software Foundation](https://my.fsf.org/donate)
- [KDE](https://kde.org/donate/)
- [VideoLAN (VLC)](https://www.videolan.org/contribute.html)
- [Ammonite](https://www.patreon.com/lihaoyi)
- Mozilla ([Firefox](https://foundation.mozilla.org/?form=moco-donate-footer),
  [Thunderbird](https://www.thunderbird.net/en-US/donate/))
- [LibreOffice](https://www.libreoffice.org/donate/)
