## How to Translate

 1. Install gettext
 2. fork the lincity-ng repo and do the usual `cmake -B build`, configure.
    Make sure configure can find the xgettext application.
    (ie. "checking for xgettext... xgettext")
 3. Run `cmake --build build -t messages.pot`. This should create
    build/share/lincity-ng/locale/messages.pot and
    build/share/lincity-ng/locale/gui/messages.pot files.
 4. Go into the directory of each messages.pot file and do:
    - In case you want to create a new translation do
      ```
      LANG=<2-character language handle e.g. 'es'>
      msginit -i build/share/lincity-ng/locale/messages.pot -o data/locale/$LANG.po
      msginit -i build/share/lincity-ng/locale/gui/messages.pot -o data/locale/gui/$LANG.po
      ```
    - In case you want to update an existing translation do
      ```
      LANG=<2-character language handle e.g. 'es'>
      msgmerge -U data/locale/$LANG.po build/share/lincity-ng/locale/messages.pot
      msgmerge -U data/locale/gui/$LANG.po build/share/lincity-ng/locale/gui/messages.pot
      ```
 5. Edit the .po files with [a PO editor](
    https://www.gnu.org/software/trans-coord/manual/web-trans/html_node/PO-Editors.html)
    or with your favorite text editor.
 6. Commit/push the changes to your fork and open a pull request against master.
 7. If you want to edit the help texts, then create a directory with your
    language shortcut in the data/help directory (e.g. data/help/de) and write
    new xml files in your language (similar to the ones in the data/help/en
    directory). You don't need to copy over the english texts, the game will
    automatically use the english version of a text if no translated version
    exists.
 8. You should also edit the lincity-ng.desktop file and add a comment entry for
    your language
