#!/bin/sh

for i in ../data/images/tiles.render/*; do
    ./autocrop "$i" "$i"
done

# EOF #
