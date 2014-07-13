#!/bin/bash

INSTALL_PREFIX="$PWD/lincity-ng"
APP_NAME="LinCity-NG.app"

cd ..
./configure --with-apple-opengl-framework --prefix=$INSTALL_PREFIX x86_64-apple-darwin10.6.0 
jam
jam install

cd ./make_mac

echo "* Removing any existing installation"
    rm -rf ./$APP_NAME
    rm *.dmg
    rm *.xz

echo "* Creating skeleton"
mkdir -p ./$APP_NAME/Contents/Resources
mkdir ./$APP_NAME/Contents/MacOS
cp info.plist ./$APP_NAME/Contents/info.plist
cp lincity-ng.icns ./$APP_NAME/Contents/Resources/lincity-ng.icns

echo "* Copying executable"
cp $INSTALL_PREFIX/bin/lincity-ng ./$APP_NAME/Contents/MacOS/lincity-ng

echo "* Copying data files"
cp -r $INSTALL_PREFIX/share ./$APP_NAME/Contents/Resources/

echo "* Bunling dependencies"

dylibbundler -od -b -x ./$APP_NAME/Contents/MacOS/lincity-ng -d ./$APP_NAME/Contents/libs/

echo "* Creating DMG"
    hdiutil create -size 100M -fs HFS+J -volname LinCity-NG LinCity-NG-tmp.dmg
	hdiutil attach LinCity-NG-tmp.dmg -readwrite -mount required
	cp -R LinCity-NG.app /Volumes/LinCity-NG/
	cp ../README /Volumes/LinCity-NG
	cp ../COPYING /Volumes/LinCity-NG
	cp ../COPYING-data.txt /Volumes/LinCity-NG
	cp ../COPYING-fonts.txt /Volumes/LinCity-NG
	hdiutil detach /Volumes/LinCity-NG -force
	./licenseDMG.py LinCity-NG-tmp.dmg ../COPYING
	hdiutil convert LinCity-NG-tmp.dmg -format UDRO -o LinCity-NG-`awk -F " " '{print $3}' ../RELNOTES | awk 'NR == 1'`.dmg
    xz -z LinCity-NG-`awk -F " " '{print $3}' ../RELNOTES | awk 'NR == 1'`.dmg
	rm LinCity-NG-tmp.dmg