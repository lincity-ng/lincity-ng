#! /bin/sh

## Unfortunately, indent doesn't do what I want, which is to respect spacing
## after commas in function declarations.


tmp_file="indent.tmp"

if test -z "$1" -o -n "$2"; then
    echo Usage: indent.sh filename
    exit 1
fi

if test "$1" = "--help" -o "$1" = "-h"; then
    echo Usage: indent.sh filename
    exit 0
fi

if test ! -f "$1"; then
    echo File \"$1\" not found
    exit 1
fi

if test -f "$tmp_file"; then
    echo Scratch file \"$tmp_file\" exists!  Please investigate.
#    exit 1
fi

options="
	    -nbad -bap -bbo -nbc -br -brs 
	    -c33 -cd33 -ncdb -ce -ci8 -cli0
            -cp33 -cs -d0 -di1 -nfc1 -nfca -hnl -i4 -ip0 -l78 -lp 
	    -lc78 -npcs -nprs -npsl -nsc -nsob -nss -ts8 -v"


indent -st $options $1 > $tmp_file
