/* ---------------------------------------------------------------------- *
 * ldsvguts.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

/* this is the saving facility */

#ifndef __ldsvguts_h__
#define __ldsvguts_h__

/* Don't load if < MIN_LOAD_VERSION */
#define MIN_LOAD_VERSION 97

/* VERSION_INT is used in the load/save code.  All other code uses 
   the symbol VERSION in config.h */
#define VERSION_INT 113

/* Disable waterwell if version < MIN_WATERWELL_VERSION */
#define MIN_WATERWELL_VERSION 1180

/* New load/save format */
#define WATERWELL_V2 1316


#if defined (WIN32)
#   define PATH_SLASH '\\'
#   define PATH_SLASH_STRING "\\"
#else
#   define PATH_SLASH '/'
#   define PATH_SLASH_STRING "/"
#endif

#define OLD_LC_SAVE_DIR "Lin-city"
#if defined (WIN32)
#    define LC_SAVE_DIR "SAVED_GAMES"
#    define LINCITYRC_FILENAME "lincity-ng.ini"
#else
#    define LC_SAVE_DIR ".lincity-ng"
#    define LINCITYRC_FILENAME ".lincity-ng_rc"
#endif
#define RESULTS_FILENAME "results"




//void load_saved_city(char *s);
void sanity_check(void);
void save_city(char *);
void save_city_2(char *);
void load_city(char *);
void load_city_2(char *);

#endif /* __ldsvguts_h__ */
