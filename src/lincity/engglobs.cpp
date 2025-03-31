/* ---------------------------------------------------------------------- *
 * src/lincity/engglobs.cpp
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

/* ---------------------------------------------------------------------- *
 * engglobs.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "engglobs.h"

#include <stddef.h>             // for NULL
#include <map>                  // for map

#include "lin-city.h"           // for EX_DISCOUNT_TRIGGER_1, EX_DISCOUNT_TR...\

/* Vector for visiting neigbours = ( dx[k] , dy[k] )  ; ordered so that diagonal moves are the last 4 */
const int dx[8] = { -1, 0, 1, 0, 1, 1, -1, -1};
const int dy[8] = { 0, -1, 0, 1, 1, -1, 1, -1};
//anti clockwise ordering
const int dxo[8] ={ -1, -1,  0,  1,  1,  1,  0, -1};
const int dyo[8] ={  0, -1, -1, -1,  0,  1,  1,  1};

int ex_tax_dis[NUMOF_DISCOUNT_TRIGGERS] = {
    EX_DISCOUNT_TRIGGER_1,
    EX_DISCOUNT_TRIGGER_2,
    EX_DISCOUNT_TRIGGER_3,
    EX_DISCOUNT_TRIGGER_4,
    EX_DISCOUNT_TRIGGER_5,
    EX_DISCOUNT_TRIGGER_6,
};

/** @file lincity/engglobs.cpp */
