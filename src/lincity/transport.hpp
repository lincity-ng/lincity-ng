/* ---------------------------------------------------------------------- *
 * src/lincity/transport.hpp
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

#include "all_buildings.hpp"  // for magenta, white, yellow
#include "lin-city.hpp"       // for MAX_TECH_LEVEL

#ifndef __LINCITYNG_LINCITY_TRANSPORT_HPP__
#define __LINCITYNG_LINCITY_TRANSPORT_HPP__

#define GROUP_TRACK_COLOUR 32   /* 32 is a brown */
#define GROUP_TRACK_COST   1
#define GROUP_TRACK_COST_MUL 25
#define GROUP_TRACK_BUL_COST   10
#define GROUP_TRACK_TECH   (0 * (MAX_TECH_LEVEL / 1000))
#define GROUP_TRACK_FIREC  4

#define GROUP_ROAD_COLOUR  (white(18))
#define GROUP_ROAD_COST    100
#define GROUP_ROAD_COST_MUL 25
#define GROUP_ROAD_BUL_COST    50
#define GROUP_ROAD_TECH    (50 * (MAX_TECH_LEVEL / 1000))
#define GROUP_ROAD_FIREC   4

#define GROUP_RAIL_COLOUR  (magenta(18))
#define GROUP_RAIL_COST    500
#define GROUP_RAIL_COST_MUL 10
#define GROUP_RAIL_BUL_COST    1000
#define GROUP_RAIL_TECH    (180 * (MAX_TECH_LEVEL / 1000))
#define GROUP_RAIL_FIREC   6

#define GROUP_TRANSPORT_RANGE 0
#define GROUP_TRANSPORT_SIZE 1

#define GROUP_POWER_LINE_COLOUR (yellow(26))
#define GROUP_POWER_LINE_COST 100
#define GROUP_POWER_LINE_COST_MUL 2
#define GROUP_POWER_LINE_BUL_COST 100
#define GROUP_POWER_LINE_TECH (200 * (MAX_TECH_LEVEL / 1000))
#define GROUP_POWER_LINE_FIREC 0
#define GROUP_POWER_LINE_RANGE 0
#define GROUP_POWER_LINE_SIZE 1
#define POWER_LINE_LOSS 1       /* one hi-volt */
#define POWER_MODULUS 18        /* Controls how often we see a packet in anim */


#define TRANSPORT_RATE 8 //slows down the transport
#define TRANSPORT_QUANTA 1000 //granularity for transport flow

#define MAX_FOOD_ON_TRACK 2048
#define MAX_FOOD_ON_ROAD (MAX_FOOD_ON_TRACK*4)
#define MAX_FOOD_ON_RAIL (MAX_FOOD_ON_TRACK*16)
#define MAX_FOOD_IN_MARKET (MAX_FOOD_ON_RAIL*8)

#define MAX_LABOR_ON_TRACK 1024
#define MAX_LABOR_ON_ROAD (MAX_LABOR_ON_TRACK*5)
#define MAX_LABOR_ON_RAIL (MAX_LABOR_ON_TRACK*25)
#define MAX_LABOR_IN_MARKET (MAX_LABOR_ON_RAIL*3)

#define MAX_COAL_ON_TRACK 64
#define MAX_COAL_ON_ROAD (MAX_COAL_ON_TRACK*8)
#define MAX_COAL_ON_RAIL (MAX_COAL_ON_TRACK*64)
#define MAX_COAL_IN_MARKET (MAX_COAL_ON_RAIL*2)

#define MAX_GOODS_ON_TRACK 2048
#define MAX_GOODS_ON_ROAD (MAX_GOODS_ON_TRACK*5)
#define ROAD_GOODS_USED_MASK 0x1f
#define MAX_GOODS_ON_RAIL (MAX_GOODS_ON_TRACK*25)
#define RAIL_GOODS_USED_MASK 0xf
#define MAX_GOODS_IN_MARKET (MAX_GOODS_ON_RAIL*4)

#define MAX_ORE_ON_TRACK 4096
#define MAX_ORE_ON_ROAD (MAX_ORE_ON_TRACK*4)
#define MAX_ORE_ON_RAIL (MAX_ORE_ON_TRACK*16)
#define MAX_ORE_IN_MARKET (MAX_ORE_ON_RAIL*2)

#define MAX_STEEL_ON_TRACK 128
#define MAX_STEEL_ON_ROAD (MAX_STEEL_ON_TRACK*4)
#define MAX_STEEL_ON_RAIL (MAX_STEEL_ON_TRACK*16)
#define RAIL_STEEL_USED_MASK 0x7f
#define MAX_STEEL_IN_MARKET (MAX_STEEL_ON_RAIL*2)

#define MAX_WASTE_ON_TRACK 1024
#define MAX_WASTE_ON_ROAD (MAX_WASTE_ON_TRACK*5)
#define MAX_WASTE_ON_RAIL (MAX_WASTE_ON_TRACK*25)
#define MAX_WASTE_IN_MARKET (MAX_WASTE_ON_RAIL*3)
#define WASTE_BURN_ON_TRANSPORT 100
#define WASTE_BURN_ON_TRANSPORT_POLLUTE WASTE_BURN_ON_TRANSPORT/2

#define MAX_LOVOLT_ON_TRACK 2048
#define MAX_LOVOLT_ON_ROAD (MAX_LOVOLT_ON_TRACK*4)
#define MAX_LOVOLT_ON_RAIL (MAX_LOVOLT_ON_TRACK*16)
#define LOVOLT_LOSS_ON_TRANSPORT 3

#define MAX_HIVOLT_ON_POWERLINE (MAX_LOVOLT_ON_RAIL*8)

#define MAX_WATER_ON_TRACK (2 * MAX_FOOD_ON_TRACK)
#define MAX_WATER_ON_ROAD (MAX_WATER_ON_TRACK*4)
#define MAX_WATER_ON_RAIL (MAX_WATER_ON_TRACK*16)
#define MAX_WATER_IN_MARKET (MAX_WATER_ON_RAIL*8)

#define BRIDGE_FACTOR 500 /* a bridge is that much more expensive than the land transport */

#define GROUP_TRACK_BRIDGE_COLOUR 32   /* 32 is a brown */
#define GROUP_TRACK_BRIDGE_COST   GROUP_TRACK_COST * BRIDGE_FACTOR
#define GROUP_TRACK_BRIDGE_COST_MUL 2
#define GROUP_TRACK_BRIDGE_BUL_COST GROUP_TRACK_BUL_COST * BRIDGE_FACTOR
#define GROUP_TRACK_BRIDGE_TECH   GROUP_TRACK_TECH
#define GROUP_TRACK_BRIDGE_FIREC  0    /* unused */

#define GROUP_ROAD_BRIDGE_COLOUR  (white(18))
#define GROUP_ROAD_BRIDGE_COST    GROUP_ROAD_COST * BRIDGE_FACTOR
#define GROUP_ROAD_BRIDGE_COST_MUL 2
#define GROUP_ROAD_BRIDGE_BUL_COST GROUP_ROAD_BUL_COST * BRIDGE_FACTOR
#define GROUP_ROAD_BRIDGE_TECH    GROUP_ROAD_TECH
#define GROUP_ROAD_BRIDGE_FIREC   0

#define GROUP_RAIL_BRIDGE_COLOUR  (magenta(18))
#define GROUP_RAIL_BRIDGE_COST    GROUP_RAIL_COST * BRIDGE_FACTOR
#define GROUP_RAIL_BRIDGE_COST_MUL 2
#define GROUP_RAIL_BRIDGE_BUL_COST GROUP_RAIL_BUL_COST * BRIDGE_FACTOR
#define GROUP_RAIL_BRIDGE_TECH    GROUP_RAIL_TECH
#define GROUP_RAIL_BRIDGE_FIREC   0

#endif // __LINCITYNG_LINCITY_TRANSPORT_HPP__
