/* ---------------------------------------------------------------------- *
 * src/lincity/all_buildings.h
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

#ifndef __all_buildings_h__
#define __all_buildings_h__

#include "lin-city.hpp"

/************* Buildings stuff *********************/

#define DAYS_PER_POLLUTION      14
#define RAIL_POLLUTION          1
#define DAYS_PER_RAIL_POLLUTION 30
#define ROAD_POLLUTION          1
#define DAYS_PER_ROAD_POLLUTION 20
#define UNNAT_DEATHS_COST       500

#define INCOME_TAX_RATE 8
#define COAL_TAX_RATE 15
#define GOODS_TAX_RATE 1
#define DOLE_RATE 15
#define TRANSPORT_COST_RATE 14
#define IM_PORT_COST_RATE 1

#define DAYS_BETWEEN_COVER (NUMOF_DAYS_IN_MONTH*3)
#define COVER_TOLERANCE_DAYS 5

#define ORE_RESERVE       1000
#define COAL_RESERVE_SIZE 10000
#define COAL_PER_RESERVE  1000

#define LABOR_LOAD_COAL  18
#define LABOR_LOAD_ORE    9
#define LABOR_LOAD_STEEL 15

//used on markets, transport, shanties when burning waste
#define WASTE_BURN_TIME 350
#define WASTE_BURN_UNCONTROLLED .001

/* interest rate *10  ie 10 is 1% */
#define INTEREST_RATE 15

/******* Buildings // GROUPS ************/
#define red(x) (32 + x)
#define green(x) (64 + x)
#define yellow(x) (96 + x)
#define blue(x) (128 + x)
#define magenta(x) (160 + x)
#define cyan(x) (192 + x)
#define white(x) (224 + x)

#define GROUP_NAME_LEN   20

#define GROUP_BARE_COLOUR  (green(12))
#define GROUP_BARE_COST    0
#define GROUP_BARE_COST_MUL 1
#define GROUP_BARE_BUL_COST 1
#define GROUP_BARE_TECH    (0 * (MAX_TECH_LEVEL / 1000))
#define GROUP_BARE_FIREC   0

#define GROUP_WATER_COLOUR    (blue(31))
#define GROUP_WATER_COST      1000000
#define GROUP_WATER_COST_MUL 2
#define GROUP_WATER_BUL_COST      1000000
#define GROUP_WATER_TECH      (0 * (MAX_TECH_LEVEL / 1000))
#define GROUP_WATER_FIREC 0

#define GROUP_BURNT_COLOUR    (red(30))
#define GROUP_BURNT_COST   0    /* Unused */
#define GROUP_BURNT_COST_MUL   1        /* Unused */
#define GROUP_BURNT_BUL_COST  1000
#define GROUP_BURNT_TECH   0    /* Unused */
#define GROUP_BURNT_FIREC  0

#define GROUP_USED_COLOUR (green(12))   /* Unused */
#define GROUP_USED_COST   0     /* Unused */
#define GROUP_USED_COST_MUL  1  /* Unused */
#define GROUP_USED_BUL_COST  0  /* Unused */
#define GROUP_USED_TECH   0     /* Unused */
#define GROUP_USED_FIREC  0     /* Unused */

#define GROUP_DESERT_COLOUR  (yellow(18))
#define GROUP_DESERT_COST    0
#define GROUP_DESERT_COST_MUL 1
#define GROUP_DESERT_BUL_COST 1
#define GROUP_DESERT_TECH    (0 * (MAX_TECH_LEVEL / 1000))
#define GROUP_DESERT_FIREC   0

#define GROUP_TREE_COLOUR  (green(12))
#define GROUP_TREE_COST    0
#define GROUP_TREE_COST_MUL 1
#define GROUP_TREE_BUL_COST 1
#define GROUP_TREE_TECH    (0 * (MAX_TECH_LEVEL / 1000))
#define GROUP_TREE_FIREC   0

#define GROUP_TREE2_COLOUR  (green(12))
#define GROUP_TREE2_COST    0
#define GROUP_TREE2_COST_MUL 1
#define GROUP_TREE2_BUL_COST 1
#define GROUP_TREE2_TECH    0
#define GROUP_TREE2_FIREC   0

#define GROUP_TREE3_COLOUR  (green(12))
#define GROUP_TREE3_COST    0
#define GROUP_TREE3_COST_MUL 1
#define GROUP_TREE3_BUL_COST 1
#define GROUP_TREE3_TECH    0
#define GROUP_TREE3_FIREC   0

#endif // __all_buildings_h__

/** @file lincity/all_buildings.h */
