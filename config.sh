
## Here is how i do it in ver 1.13
aclocal -I m4
libtoolize --force
automake --add-missing
autoconf
autoheader
rm -f config.cache
./configure --prefix=$HOME/lincity

exit

## This is how i did it in ver 1.12
aclocal
automake --add-missing --foreign Makefile
autoconf
autoheader
rm -f config.cache
./configure --prefix=$HOME/lincity
