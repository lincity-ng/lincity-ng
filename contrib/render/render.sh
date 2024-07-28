#!/bin/sh

set -e -x

if [ "$(dirname $0)" != "." ]; then
	echo error: `basename $0` must be run from its own directory with ./`basename $0`
	exit
fi

mkdir -p tmp
cd ../../data/images
mkdir -p tiles.render
cd -

for i in ../../data/models/*.blend; do
    blender -b "$i" -P render.py
    blender -b tmp/tmp.blend -a
    b=`basename $i`
    cp -v tmp/0001 "../../data/images/tiles.render/${b%%.blend}.png"
done

# EOF #
