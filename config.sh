aclocal
automake --add-missing --foreign Makefile
autoconf
autoheader
rm -f config.cache
./configure --prefix=$HOME/lincity
