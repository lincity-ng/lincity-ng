#!/bin/sh

set -e

mkdir -p tmp/
mkdir -p ../data/images/out/

for i in *.blend; do
    blender -b "$i"  -P render.py
    blender -b out/tmp.blend -a
    cp -v out/0001 "../images/tiles/${i%%.blend}.png"
done

# EOF #
