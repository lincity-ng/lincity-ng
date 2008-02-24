#!/bin/bash
mkdir -p orig
cp -af *.po *.pot orig
for f in `/bin/ls *.po`
do
    msgmerge --no-fuzzy-matching --sort-by-file orig/$f messages.pot>$f

done
