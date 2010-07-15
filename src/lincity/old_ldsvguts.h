/* ---------------------------------------------------------------------- *
 * old_ldsvguts.h
 * This file is part of lincity-ng
 * ---------------------------------------------------------------------- */

/* This is the OLD facility, before lincity-NG 1.91 */
/* Used for reading old games and convert them to new format + data structure */

#ifndef __old_ldsvguts_h__
#define __old_ldsvguts_h__


/* Load corrections if version <= MM_MS_C_VER (max markets/substations) */
#define MM_MS_C_VER 97

/* Load corrections if version <= MG_C_VER (max monthgraph size) */
#define MG_C_VER 111

/* Don't load if < MIN_LOAD_VERSION */
#define MIN_LOAD_VERSION 97

/* VERSION_INT is used in the load/save code.  All other code uses 
   the symbol VERSION in config.h */
#define VERSION_INT 113

void load_city_old(char *);

#endif /* __old_ldsvguts_h__ */

/** @file lincity/old_ldsvguts.h */

