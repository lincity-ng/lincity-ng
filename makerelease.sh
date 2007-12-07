#!/bin/sh
cat data/gui/creditslist.xml |grep -v "@"|cut -d\> -f2|cut -d\< -f1 >CREDITS
echo "# automatically generated from data/gui/creditslist.xml. Do not edit. #">>CREDITS

jam maintainer-clean
rm -rf build
./autogen.sh
./configure
jam dist

