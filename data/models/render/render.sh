#!/bin/sh

set -e -x

mkdir -p tmp/
mkdir -p ../data/images/tiles.render/

for i in ../data/models/*.blend; do
    blender -b "$i"  -P render.py
    blender -b tmp/tmp.blend -a
    b=`basename $i`
    cp -v tmp/0001 "../data/images/tiles.render/${b%%.blend}.png"
done

# EOF #
