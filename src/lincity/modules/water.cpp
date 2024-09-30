/* ---------------------------------------------------------------------- *
 * src/lincity/modules/water.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2024 David Bears <dbear4q@gmail.com>
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

#include "water.h"

#include <string>                // for basic_string

#include "modules.h"             // for N_, MapTile, TileConstructionGroup
#include "modules_interfaces.h"  // for mps_water

void mps_water(int x, int y)
{
    int i;
    const char *p;

    i = 0;
    mps_store_sdd(i++, waterConstructionGroup.name, x, y);
    i++;

    if ( world(x,y)->flags & FLAG_IS_LAKE )
    {   p = N_("Lake");}
    else if ( world(x,y)->flags & FLAG_IS_RIVER )
    {   p = N_("River");}
    else
    {   p = N_("Pond");}
    mps_store_title(i++, p);
/*
#ifdef DEBUG
    mps_store_sd(10, "x", x);
    mps_store_sd(11, "y", y);
    mps_store_sd(12, "altitude", world(x,y)->ground.altitude);

    fprintf(stderr, "water x %i, y %i, Alt %i\n", x, y, world(x,y)->ground.altitude);
#endif
*/
}

/** @file lincity/modules/water.cpp */
