/* ---------------------------------------------------------------------- *
 * src/lincity/loadsave.h
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

/* This is the loading/saving facility */

#include <string>  // for string

#ifndef __loadsave_h__
#define __loadsave_h__

#if defined (WIN32)
#   define PATH_SLASH '\\'
#   define PATH_SLASH_STRING "\\"
#else
#   define PATH_SLASH '/'
#   define PATH_SLASH_STRING "/"
#endif

# define LC_ORG "lincity-ng"
# define LC_APP "lincity-ng"
# define LINCITYRC_FILENAME ".lincity-NGrc"

#define RESULTS_FILENAME "results.txt"


/* New load/save format */
#define WATERWELL_V2 1322

extern char given_scene[1024];

//void load_saved_city(char *s);
void save_city(char *);
void save_city_2(std::string);
void load_city_2(char *);

extern void load_city_old(char *);

#endif /* __loadsave_h__ */

/** @file lincity/loadsave.h */
