#!/bin/sh
# This file is part of lincity
# Create a new module fileset from skeleton files

cp ../skel.h $1.h
cp ../module_skel.c $1.c

perl -pi -e "s/skel/$1/g" $1.[ch]
