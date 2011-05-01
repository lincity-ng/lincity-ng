#!/bin/sh
# UGLY WORKAROUND for fixing tiles going up and down when animated during game.
#
# this is due to image management, probably a wrong use of the center of the tile
#	which has variable height !
# So put all the images of a same family to the same size works around this bug
#
# with ImageMagick
# convert start.png -background none -gravity southwest -extent 512x512 result.png

# works from data/images/tiles  directory

TMP=tmp.png
for  family in `cat list`
do
	echo "Processing image family : $family"
	MaxW=`file $family*.png | sort -n -k4,4|tail -1|awk '{ print $4 }'`
	MaxH=`file $family*.png | sort -n -k6,6|tail -1|awk '{ print $6 }'|cut -d"," -f1`
	for f in `ls $family*.png`
	do
		echo "  $f"
		cp $f $TMP
		convert $TMP -background none -gravity southwest -extent ${MaxW}x${MaxH} $f
	done
done

