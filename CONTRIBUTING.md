# Contributing to LinCity-NG

In developing LinCity-NG, my hope is it may be a pleasure and a good experience
for others. If You enjoyed playing LinCity-NG, I encourage you to give back.
There are many ways you can contribute even if you do not know how to code C++.

- [How to Contribute](#how-to-contribute)
  - [Opening a Pull Request](#opening-a-pull-request)
  - [Directory Map](#directory-map)
  - [Changelog](#changelog)
  - [Attribution, Copyright, and Licensing](#attribution-copyright-and-licensing)
- [Ways to Contribute](#ways-to-contribute)
  - [Contribute Code](#contribute-code)
    - [Formatting](#formatting)
    - [Singletons](#singletons)
    - [Include What You Use](#include-what-you-use)
  - [Contribute Play-Testing](#contribute-play-testing)
    - [Checklist for reporting issues](#checklist-for-reporting-issues)
  - [Contribute Translation](#contribute-translation)
  - [Other Ways to Contribute](#other-ways-to-contribute)
- [Additional Guidelines](#additional-guidelines)
  - [Git Merge and Rebase](#git-merge-and-rebase)
  - [Use of Generative AI](#use-of-generative-ai)

## How to Contribute

### Opening a Pull Request

You may contribute code, art, documentation, translation, etc. by
[forking the project on GitHub](https://github.com/lincity-ng/lincity-ng/fork)
and [opening a pull request](https://github.com/lincity-ng/lincity-ng/compare).

Pull requests should contain only a single contribution. If you have multiple
contributions, then please open a separate PR for each. This streamlines the
review process and makes your contribution more likely to be accepted.

If your PR fixes an issue or implements a feature, then please
[link your PR](https://docs.github.com/en/issues/tracking-your-work-with-issues/using-issues/linking-a-pull-request-to-an-issue)
to relevant bug reports or feature requests.

When maintainers review your contribution, it is likely that we may ask
questions about it or even challenge its methods or goals. If we believe your
contribution has merit but could be improved, then we may ask you to make some
modifications before we accept your changes. Please to not take this personally.
We just want to ensure that the project maintains the highest quality possible
-- as do you as well, I assume.

### Directory Map

- `build` - Build artifacts
- `CHANGELOG.d` - Unreleased changelog entries
- `contrib` - Miscellaneous unused files including old art and scripts
- `data` - Game assets
- `lib` - External libraries
- `mk` - Files for configure/build
- `src` - All source code
  - `gui` - Custom GUI library for LinCity-NG
  - `lincity` - LinCity-NG backend library for game mechanics
  - `lincity-ng` - LinCity-NG frontend

### Changelog

The format of the LinCity-NG changelog is inspired by [keepachangelog.com](
https://keepachangelog.com). Whenever you contribute meaningfully to the
project, please take a moment to write a brief changelog entry to sumarize your
changes with respect to the most recent release.

To avoid git merge conflicts from everyone modifying the same `CHANGELOG.md`
file. Instead of directly adding to the changelog, you will write your entry in
a new file in the `CHANGELOG.d` directory. See [template.md](
CHANGELOG.d/template.md) for an example changelog entry file. After your PR is
merged, as part of routine maintenance, I will compile all the entries from the
`CHANGELOG.d` directory and add them to the main changelog.

Changelog entries should be categorized into one of four sections:
- **Gameplay** for changes affecting the game rules
- **User Interface** for changes affecting look-and-feel or controls
- **Internal** for implementation changes that are not directly visible to the
  user such as performance improvements
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

Please only make changelog entries for changes that are with respect to the most
recent release. E.g. fixing a bug that appeared since the most recent release
should not have a changelog entry.

### Attribution, Copyright, and Licensing

LinCity-NG has a credits screen that is accessible from the main menu. When you
contribute to the game, please add yourself to the credits list by adding an
entry to [data/gui/creditslist.xml](data/gui/creditslist.xml). If you already
have an entry in the list, just make sure the email and year are up-to-date.
The entry should include your name, your email, and the calander year that you
made your contribution. Entries are categorized into several sections for
different types of contributions and ordered by last name.

Every file should have a uniform header that includes the relative file
location, a statement that the file is part of LinCity-NG, copyright
information, and the GPLv2+ license header. Please add the header to new files,
and update the copyright information as appropriate in modified files.

```
/* ---------------------------------------------------------------------- *
 * CONTRIBUTING.md
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2024-2026 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */
```

By contributing to LinCity-NG, you are licensing your contribution under the
GPLv2+. Additionally, most non-code contributions are licensed under
CC-by-sa-v2. By contributing, you acknowledge that you either own copyright for
your contribution or otherwise have the right to license your contribution under
the applicable licenses.

## Ways to Contribute

### Contribute Code

#### Formatting

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

#### Singletons

[Singletons are evil.](https://kentonshouse.com/singletons) I have spent
considerable effort to remove singletons, so do me a favor and avoid adding more
if possible.

#### Include What You Use

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

### Contribute Play-Testing

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
  user-interface, incomplete documentation, or even ideas for the feature.
- When you find an issue on master, report it to
  https://github.com/lincity-ng/lincity-ng/issues. If you find an issue in a
  pull request that does not exist on master, then comment in the PR discussion
  thread. When opening the issue, include your LinCity-NG version and your
  platform/operating system. To avoid duplicate issues, search the issues list
  (including closed issues) to find whether your issue has already been
  reported; if it is already reported, then you may be able to provide
  additional details.

#### Checklist for reporting issues:

1. Search the
   [issues list](https://github.com/lincity-ng/lincity-ng/issues?q=is%3Aissue%20)
   (including closed issues), to see if someone has already reported it.
   - If it is already reported, you may be able to provide additional details,
     confirm the issue affects multiple people, and/or confirm the issue still
     exists.
   - If it is not already reported, open a
     [new issue](https://github.com/lincity-ng/lincity-ng/issues/new).
2. In the report, please include your LinCity-NG version, your platform/OS, and
   an explanation of the issue.
3. For crashes, include what you did (or tried to do) that seemed to cause the
   crash, any console output related to the crash (especially warnings/errors),
   and a backtrace if available. (You can get a backtrace by running the game in
   gdb; when the game crashes, run the `bt` gdb command.)
4. Include screenshots if/where applicable.


### Contribute Translation

Here is the general workflow for translating LinCity-NG:
1. Fork and clone the lincity-ng repo
2. Install [gettext](https://www.gnu.org/software/gettext/).
3. Configure the build:
   ```
   cmake -B build
   ```
4. Generate an up-to-date PO template file:
   ```
   cmake --build build -t generate_pot
   ```
5. Create/Update the translation with the new strings:
   - If you are creating a new translation:
     ```
     LANG=<2-character language handle e.g. 'en'>
     msginit -i build/messages.pot -o data/locale/$LANG.po
     ```
   - If you are updating an existing translation:
     ```
     LANG=<2-character language handle e.g. 'en'>
     msgmerge -U data/locale/$LANG.po build/messages.pot
     ```
6. Update and review translations in the .po file with [a PO editor](
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

### Other Ways to Contribute

- You can help make the game portable by developing, testing, or packaging it
  for other platforms. I use Arch Linux to develop LinCity-NG, and I do not have
  access to other platforms such as Windows or Mac, so without help, I am unable
  to detect or address issues specific to other platforms.
- You can contribute to documentation such as the readme or the in-game help. As
  the game improves, it is easy for documentation to become outdated, so it can
  be nice to have some help keeping it up to date.
- You can contribute art to the game such as graphics, music, or UX design. I'm
  not particularly gifted in these areas, so I appreciate contributions from
  others.

## Additional Guidelines

### Git Merge and Rebase

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

In general, do not rebase commits that are already on remote -- use merge
instead. This avoids the need to force-push and rewrite history which can add
work for, or worse, confuse, other developers.

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

### Use of Generative AI

AI-generated contributions are not outright banned, but you are still expected
to meet a high standard of quality. Low-effort contributions will not be
tolerated.
To discourage low-effort contributions, if the content of your contribution is
created with the help of generative AI, then your PR description should be fully
human-written.

The quality of AI-generated contributions must be at least that of human-created
content, and you are expected to review your contribution manually before
submitting it for review by project maintainers. If any AI-generated content
"could be better" or "isn't how you would have done it", then you should
probably make improvements by hand before submitting it. Particularly,
contributions that include strange quirks or artifacts from AI are unacceptable.

Generative AI has a little habit of being wordy. If you use generative AI to
write code comments, documentation, PR descriptions, commit messages, etc., then
ensure that the language is succinct and to-the-point without omitting important
details. And bullet points are not a get-out-of-jail-free card; please do not
overuse bullet points. Comments in code particularly should not be lengthy
(preferably they should fit on one line) and they should not simply re-iterate
what the code itself already says. If comments are too lengthy or frequent, then
you should probably rewrite the code so it is easier to read.

To promote transparency, use of generative AI must be disclosed on the last line
of PR descriptions and commit messages. Such disclosures should state that
generative AI was used, to what extent it was used, and what AI models were
used.

In most cases, AI-generated content likely falls into public domain (depending
on jurisdiction), so do not add copyright statements to code, art, or other
works for which you do not own copyright. For entire files that are
AI-generated, add a note with the year and a statement that the file was
generated with AI. If an entire file is public domain (e.g. because it is
AI-generated), put a statement that it is public domain in place of the license
header. The public-domain statement should be removed and replaced with the
GPLv2+ license header if copyrighted material is later added to the file.

```
/* ---------------------------------------------------------------------- *
 * path/to/some/file.md
 * This file is part of Lincity-NG.
 *
 * 2026 This file was generated by AI
 *
 * This file is public domain.
** ---------------------------------------------------------------------- */
```

It is possible that other contributors may be the copyright holder for
AI-generated code if the generated code resembles that of code elsewhere in the
project. In such case, copyright statements should be added where appropriate.

Obviously, you must be able to defend your contribution and answer questions
about it. Use of AI is not a replacement for using your brain. And of course
AI-generated contributions must follow the contributing guidelines just like any
other contribution.
