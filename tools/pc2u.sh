#! /bin/sh

tmp_file=".pc2u.tmp"

if test -z "$1" -o -n "$2"; then
    echo Usage: pc2u.sh filename
    exit 1
fi

if test "$1" = "--help" -o "$1" = "-h"; then
    echo Usage: pc2u.sh filename
    exit 0
fi

if test ! -f "$1"; then
    echo File \"$1\" not found
    exit 1
fi

if test -f "$tmp_file"; then
    echo Scratch file \"$tmp_file\" exists!  Please investigate.
    exit 1
fi

tr -d '\r' < $1 > $tmp_file
rm $1
mv $tmp_file $1

