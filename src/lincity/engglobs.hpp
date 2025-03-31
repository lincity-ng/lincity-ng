/* ---------------------------------------------------------------------- *
 * src/lincity/engglobs.h
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2025 David Bears
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

#ifndef __engglobs_h__
#define __engglobs_h__

#include "lin-city.hpp"  // for NUMOF_DISCOUNT_TRIGGERS

/* Vector for visiting neigbours = ( dx[k] , dy[k] )  ; ordered so that diagonal moves are the last 4 */
extern const int dx[8];
extern const int dy[8];
//anti clockwise ordering
extern const int dxo[8];
extern const int dyo[8];

extern int ex_tax_dis[NUMOF_DISCOUNT_TRIGGERS];

#endif /* __engglobs_h__ */

/** @file lincity/engglobs.h */
