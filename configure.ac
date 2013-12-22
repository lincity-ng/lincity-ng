#----------------------------------------------------------------------------
# Autoconf input script. Start the ./autgen.sh script to create
# configure script.
#----------------------------------------------------------------------------
AC_PREREQ([2.54])

AC_INIT([lincity-ng], [2.9.beta], [matze@braunis.de])
AC_CONFIG_SRCDIR([mk/jam/build.jam])
AC_CONFIG_AUX_DIR([mk/autoconf])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

#----------------------------------------------------------------------------
# Configuration header
#----------------------------------------------------------------------------
AC_CONFIG_HEADERS(config.h)

# stupid autoconf is adding default -g -O2 flags when we don't want this :-/
test ".$CXXFLAGS" = "." && CXXFLAGS=" "
test ".$CFLAGS" = "." && CFLAGS=" "

#----------------------------------------------------------------------------
# Check for build variant (debug, profile, optimize)
#----------------------------------------------------------------------------
VARIANT=optimize
AC_ARG_ENABLE([optimize], [AC_HELP_STRING([--enable-optimize],
    [build with optimizations enabled (default YES)])],
    [test "$enableval" = "yes" && VARIANT=optimize])
                                                                                
AC_ARG_ENABLE([debug], [AC_HELP_STRING([--enable-debug],
    [build with debugging information (default NO)])],
    [test "$enableval" = "yes" && VARIANT=debug])
                                                                                
AC_ARG_ENABLE([profile], [AC_HELP_STRING([--enable-profile],
    [build with profiling information (default NO)])],
    [test "$enableval" = "yes" && VARIANT=profile])
AC_SUBST([VARIANT])

#----------------------------------------------------------------------------
# find applications
#----------------------------------------------------------------------------
AC_PROG_CXX
AC_PROG_INSTALL
AC_CHECK_PROGS([XGETTEXT], xgettext)
AC_CHECK_PROGS([XSLTPROC], xsltproc)

# check if our c++ compiler is gcc3.2 or newer
AC_MSG_CHECKING([whether g++ is new enough])
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([], [
#ifndef __GNUC__
#error no gcc compiler found (__GNUC__ not defined)
#endif
#if __GNUC__ < 3 || (__GNUC__ == 3 && __GNUC__ < 2)
#error gcc compiler too old. Need at least version 3.2.0
#endif
], [AC_MSG_RESULT([yes])], [
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([
Error: Your gcc version is too old. Lincity-ng needs at least gcc 3.2.0.
])])
AC_LANG_RESTORE

#----------------------------------------------------------------------------
# check headers
#----------------------------------------------------------------------------
AC_HEADER_DIRENT
AC_HEADER_TIME
AC_HEADER_STDC

# compiler characteristics
AC_C_CONST
AC_C_INLINE
AC_CHECK_FUNCS(gettimeofday mkdir popen)

AM_ICONV
AC_SUBST([ICONV_LIBS], [$LIBICONV])

#----------------------------------------------------------------------------
# find libraries
#----------------------------------------------------------------------------
AM_BINRELOC

NP_FINDLIB([ZLIB], [zlib], [zlib],
	NP_LANG_PROGRAM([[#include <zlib.h>]],
            [[gzFile* file = gzopen("", "");]]),
	[], [-lz],
	[],
	[AC_MSG_ERROR([Please install zlib])])

PKG_CHECK_MODULES([LIBXML], [libxml-2.0 >= 2.6.11])

AX_CHECK_GL
if test "$no_gl" = "yes"; then
    AC_MSG_ERROR([Couldn't find opengl libraries or headers])
fi

AM_PATH_SDL([1.2.14], , AC_MSG_ERROR([Please install libsdl >= 1.2.14]))

NP_FINDLIB([SDLMIXER], [SDL_mixer], [SDL_mixer >= 1.2],
	NP_LANG_PROGRAM([#include <SDL_mixer.h>
#if MIX_MAJOR_VERSION < 1
# error SDLMix too old
#else
# if MIX_MAJOR_VERSION == 1
#  if MIX_MINOR_VERSION < 2
#   error SDLMix too old
#  endif
# endif
#endif], [Mix_OpenAudio(0, 0, 0, 0);]),
	[], [-lSDL_mixer],
	[],
	[AC_MSG_ERROR([Please install SDLMixer >=1.2.2])],
	[$SDL_CFLAGS], [$SDL_LIBS])

NP_FINDLIB([SDLIMAGE], [SDL_image], [SDL_image >= 1.2],
	NP_LANG_PROGRAM([#include <SDL_image.h>], [IMG_Load("");]),
	[], [-lSDL_image],
	[],
	[AC_MSG_ERROR([Please install SDLImage >= 1.2.1])],
	[$SDL_CFLAGS], [$SDL_LIBS])

NP_FINDLIB([SDLTTF], [SDL_ttf], [SDL_ttf >= 2.0.8],
NP_LANG_PROGRAM([#include <SDL_ttf.h>], [if(!TTF_WasInit()) TTF_Init();]),
	[], [-lSDL_ttf],
	[],
	[AC_MSG_ERROR([Please install SDL_ttf >= 2.0.8])],
	[$SDL_CFLAGS], [$SDL_LIBS])

NP_FINDLIB([SDLGFX], [SDL_gfx], [SDL_gfx >= 2.0.13],
		NP_LANG_PROGRAM([#include <SDL_gfxPrimitives.h> 
#if SDL_GFXPRIMITIVES_MAJOR < 2
# error SDL_GFX too old
#endif
], [hlineColor(0, 0, 0, 0, 0);]),
	[], [-lSDL_gfx],
	[],
	[AC_MSG_ERROR([Please install SDL_gfx >= 2.0.13])],
	[$SDL_CFLAGS], [$SDL_LIBS])

NP_FINDLIB([PHYSFS], [physfs], [physfs >= 1.0.0],
	NP_LANG_PROGRAM([
#include <stdio.h>
#include <physfs.h>
#if PHYSFS_VER_MAJOR < 1
# error PHYSFS is too old
#endif]),
	[], [-lphysfs],
	[],
	[AC_MSG_ERROR([Please install physfs >= 1.0])])
    
AC_INIT_JAM
AC_CONFIG_FILES([Jamconfig])

# We have a config.h file
CPPFLAGS="$CPPFLAGS -DHAVE_CONFIG_H"

AC_OUTPUT

AC_MSG_NOTICE([

Notice: This project uses jam (and not make) as build tool.
])
