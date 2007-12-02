/* ---------------------------------------------------------------------- *
 * ldsvguts.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

/* this is the saving facility */

#ifndef __ldsvguts_h__
#define __ldsvguts_h__

/* VERSION_INT is used in the load/save code.  All other code uses 
   the symbol VERSION in config.h */
#define VERSION_INT 113

/* Disable waterwell if version < MIN_WATERWELL_VERSION */
#define MIN_WATERWELL_VERSION 1180

/* Additionnal data structure */
#define WATERWELL_V2 1306

/* Don't load if < MIN_LOAD_VERSION */
#define MIN_LOAD_VERSION 97


void load_saved_city(char *s);
void sanity_check(void);
void save_city(char *);
void load_city(char *);

#endif /* __ldsvguts_h__ */
