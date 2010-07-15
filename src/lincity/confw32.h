/* ---------------------------------------------------------------------- *
 * confw32.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */  
/* This is a config.h file that has been hacked for VC++ 4.0.  You'll 
 * probably need to change if you are using a different compiler.
 * ---------------------------------------------------------------------- */ 
    
/* Define to empty if the keyword does not work.  */ 
/* #undef const */ 
    
/* Define if you have locale.h */ 
#define HAVE_LOCALE_H
    
/* Define if you want to enable foreign language support */ 
#undef ENABLE_NLS
/* #define ENABLE_NLS */ 
    
/* Define as __inline if that's what the C compiler calls it.  */ 
#define inline __inline
    
/* This is not yet fixed for UNIX...  */ 
/* #define snprintf _snprintf */ 
    
/* Define if you need to in order for stat and other things to work.  */ 
/* #undef _POSIX_SOURCE */ 
    
/* Define if you have the ANSI C header files.  */ 
/* #undef STDC_HEADERS */ 
    
/* Define if you can safely include both <sys/time.h> and <time.h>.  */ 
/* #undef TIME_WITH_SYS_TIME */ 
    
/* Define if the X Window System is missing or not being used.  */ 
/* #undef X_DISPLAY_MISSING */ 
    
/* Define if save files should be compressed with gzip.  */ 
/* #undef HAVE_GZIP */ 
    
/* The number of bytes in a char.  */ 
#define SIZEOF_CHAR 1
    
/* The number of bytes in a int.  */ 
#define SIZEOF_INT 4
    
/* The number of bytes in a long.  */ 
#define SIZEOF_LONG 4
    
/* The number of bytes in a short.  */ 
#define SIZEOF_SHORT 2
    
/* Define if you have the gettimeofday function.  */ 
/* #undef HAVE_GETTIMEOFDAY */ 
    
/* Define if you have the mkdir function.  */ 
/* #undef HAVE_MKDIR */ 
    
/* Define if you have the popen function.  */ 
/* #undef HAVE_POPEN */ 
    
/* Define if you have the select function.  */ 
/* #undef HAVE_SELECT */ 
    
/* Define if you have the <dirent.h> header file.  */ 
/* #undef HAVE_DIRENT_H */ 
    
/* Define if you have the <ndir.h> header file.  */ 
/* #undef HAVE_NDIR_H */ 
    
/* Define if you have the <string.h> header file.  */ 
#define HAVE_STRING_H 1
    
/* Define if you have the <strings.h> header file.  */ 
/* #undef HAVE_STRINGS_H */ 
    
/* Define if you have the <sys/dir.h> header file.  */ 
/* #undef HAVE_SYS_DIR_H */ 
    
/* Define if you have the <sys/ndir.h> header file.  */ 
/* #undef HAVE_SYS_NDIR_H */ 
    
/* Define if you have the <sys/time.h> header file.  */ 
/* #undef HAVE_SYS_TIME_H */ 
    
/* Define if you have the <unistd.h> header file.  */ 
/* #undef HAVE_UNISTD_H */ 
    
/* Define if you have the <vga.h> header file.  */ 
/* #undef HAVE_VGA_H */ 
    
/* Name of package */ 
#define PACKAGE "lincity"
    
/* Version number of package */ 
#define VERSION "1.13.2"

/** @file lincity/confw32.h */

