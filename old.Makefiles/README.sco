  ****  README.sco   for lincity  ****

  Lincity uses 'gzip' to compress and uncompress saved games.  Gzip is
  not a standard SCO OpenServer utility, but you can get it from the
  'SCO Skunkware' archive; currently

     ftp://ftp.sco.com/Skunk/bin/gzip



To compile under SCO OpenServer 5.0:

    make clean
    make sco

If the CC link fails, with the message

    undefined                       first referenced
     symbol                             in file
    gettimeofday(timeval*,timezone*)  main.o

then you need to make a small edit to the file /usr/include/sys/time.h
to make it C++-clean.  To do this, add the encapsulation

    #ifdef __cplusplus
    extern "C" {
    #endif

before the line

    #if !defined(_INKERNEL)

and

    #ifdef __cplusplus
    }
    #endif

after the line

    #endif /* !defined(_INKERNEL) */

to the file.  Here's a patch input that will do it on SCO OpenServer
5.0.0 and 5.0.2:

    57a58,61
    > #ifdef __cplusplus
    > extern "C" {
    > #endif
    >
    68a73,76
    >
    > #ifdef __cplusplus
    > }
    > #endif

(but be VERY careful applying this on other releases).  Now run

    make clean
    make sco

and everything should be OK.  Assuming that it is, install with

    make install

You may need to create /usr/local, /usr/local/lib, and/or
/usr/local/bin if they aren't there already.

Run the game with  'xlincity'
