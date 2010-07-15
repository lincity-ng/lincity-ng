/* ---------------------------------------------------------------------- *
 * loadsave.h
 * This file is part of lincity-NG
 * ---------------------------------------------------------------------- */

/* This is the loading/saving facility */

#ifndef __loadsave_h__
#define __loadsave_h__

#if defined (WIN32)
#   define PATH_SLASH '\\'
#   define PATH_SLASH_STRING "\\"
#else
#   define PATH_SLASH '/'
#   define PATH_SLASH_STRING "/"
#endif

# define LC_SAVE_DIR ".lincity-ng"
# define LINCITYRC_FILENAME ".lincity-NGrc"

#define RESULTS_FILENAME "results.txt"


/* New load/save format */
#define WATERWELL_V2 1322


//void load_saved_city(char *s);
void save_city(char *);
void save_city_2(char *);
void load_city_2(char *);

extern void load_city_old(char *);

#endif /* __loadsave_h__ */

/** @file lincity/loadsave.h */

