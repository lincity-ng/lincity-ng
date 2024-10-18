/* ---------------------------------------------------------------------- *
 * src/lincity/old_ldsvguts.h
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

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
