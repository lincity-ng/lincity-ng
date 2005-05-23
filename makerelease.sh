#!/bin/sh

jam maintainer-clean
rm -rf build
./autogen.sh
./configure
jam dist

