aclocal
automake --add-missing --foreign Makefile
autoconf
autoheader
rm -f config.cache
./configure --prefix=$HOME/lincity

## My solaris box gets messed up dependencies for config.h because of some
## goofy nfs incompatibility.  So I have to do this after doing a make.
## (unfortunately this remakes everything!)
# make
# rm -f stamp-h config.h
# make config.h
# ls -l config.h* stamp-h*
# touch stamp-h.in
# touch config.h.in
# touch stamp-h
# touch config.h
# ls -l config.h* stamp-h*
# make
