/* ---------------------------------------------------------------------- *
 * src/lincity/groups.hpp
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

#ifndef __groups_h__
#define __groups_h__

/******* Buildings // GROUPS ************/

#define NUM_OF_GROUPS    51

#define GROUP_BARE          0
#define GROUP_POWER_LINE    1
#define GROUP_SOLAR_POWER   2
#define GROUP_SUBSTATION    3
#define GROUP_RESIDENCE_LL  4
#define GROUP_ORGANIC_FARM  5
#define GROUP_MARKET        6
#define GROUP_TRACK         7
#define GROUP_COALMINE      8
#define GROUP_RAIL          9
#define GROUP_COAL_POWER    10
#define GROUP_ROAD          11
#define GROUP_INDUSTRY_L    12
#define GROUP_UNIVERSITY    13
#define GROUP_COMMUNE       14
#define GROUP_OREMINE       15
#define GROUP_TIP           16
#define GROUP_PORT          17
#define GROUP_INDUSTRY_H    18
#define GROUP_PARKLAND      19
#define GROUP_RECYCLE       20
#define GROUP_WATER         21
#define GROUP_HEALTH        22
#define GROUP_ROCKET        23
#define GROUP_WINDMILL      24
#define GROUP_MONUMENT      25
#define GROUP_SCHOOL        26
#define GROUP_BLACKSMITH    27
#define GROUP_MILL          28
#define GROUP_POTTERY       29
#define GROUP_FIRESTATION   30
#define GROUP_CRICKET       31
#define GROUP_BURNT         32
#define GROUP_SHANTY        33
#define GROUP_FIRE          34
#define GROUP_USED          35
#define GROUP_RESIDENCE_ML  36
#define GROUP_RESIDENCE_HL  37
#define GROUP_RESIDENCE_LH  38
#define GROUP_RESIDENCE_MH  39
#define GROUP_RESIDENCE_HH  40
#define GROUP_WATERWELL     41
#define GROUP_DESERT        42
#define GROUP_TREE          43
#define GROUP_TREE2         44
#define GROUP_TREE3         45
#define GROUP_TRACK_BRIDGE  46
#define GROUP_ROAD_BRIDGE   47
#define GROUP_RAIL_BRIDGE   48
#define GROUP_WIND_POWER    49
#define GROUP_PARKPOND      50

#define GROUP_RIVER (GROUP_WATER)

#define GROUP_IS_RESIDENCE(group) \
            ((group == GROUP_RESIDENCE_LL) || \
             (group == GROUP_RESIDENCE_ML) || \
             (group == GROUP_RESIDENCE_HL) || \
             (group == GROUP_RESIDENCE_LH) || \
             (group == GROUP_RESIDENCE_MH) || \
             (group == GROUP_RESIDENCE_HH))

#define GROUP_IS_BARE(group) \
        ((group == GROUP_BARE) || \
         (group == GROUP_DESERT) || \
         (group == GROUP_TREE) || \
         (group == GROUP_TREE2) || \
         (group == GROUP_TREE3))

#endif // __groups_h__

/** @file lincity/groups.h */
