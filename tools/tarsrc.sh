#!/bin/sh
rm ../source.tgz
filelist="\
    *make* *Make* Lincity.mak *.mdp \
    README.aix	    README.irix       README.sco \
    README.INSTALL	  README.freebsd    README.profiling  \
    *.man \
    tools* \
    *.h *.cxx *.ico *.rc"
tar czvf ../source.tgz $filelist
rm -r $filelist
