#!/bin/sh

set -e

mkdir -p out/
mkdir -p ../images/tiles/

for i in *.blend; do
    blender -b "$i"  -P render.py
    blender -b out/tmp.blend -a
    cp -v out/0001 "../images/tiles/${i%%.blend}.png"
done

# EOF #
