/* ---------------------------------------------------------------------- *
 * transport.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lintypes.h"

#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

#endif


//int collect_transport_info(int x, int y, Construction::Commodities stuff_ID, int center_ratio);
/*
    returns the relative load TRANSPORT_QUANTA = 100% if the construction would
tentatively participate want traffic or -1 if there is no such stuff known at mapTile[x][y]. center_ratio == -1 ignores tentative traffic
*/

//int equilibrate_transport_stuff(int x, int y, int *rem_lvl, int rem_cap, int ratio, Construction::Commodities stuff_ID);
/*
    Handles the flow of stuff between the central and the remote tile. takes care of  paying taxes.
*/

void connect_transport(int originx, int originy, int w, int h);


#define GROUP_TRACK_COLOUR 32   /* 32 is a brown */
#define GROUP_TRACK_COST   1
#define GROUP_TRACK_COST_MUL 25
#define GROUP_TRACK_BUL_COST   10
#define GROUP_TRACK_TECH   0
#define GROUP_TRACK_FIREC  4

#define GROUP_ROAD_COLOUR  (white(18))
#define GROUP_ROAD_COST    100
#define GROUP_ROAD_COST_MUL 25
#define GROUP_ROAD_BUL_COST    50
#define GROUP_ROAD_TECH    50
#define GROUP_ROAD_FIREC   4

#define GROUP_RAIL_COLOUR  (magenta(18))
#define GROUP_RAIL_COST    500
#define GROUP_RAIL_COST_MUL 10
#define GROUP_RAIL_BUL_COST    1000
#define GROUP_RAIL_TECH    180
#define GROUP_RAIL_FIREC   6

#define GROUP_TRANSPORT_RANGE 0
#define GROUP_TRANSPORT_SIZE 1

#define GROUP_POWER_LINE_COLOUR (yellow(26))
#define GROUP_POWER_LINE_COST 100
#define GROUP_POWER_LINE_COST_MUL 2
#define GROUP_POWER_LINE_BUL_COST 100
#define GROUP_POWER_LINE_TECH 200
#define GROUP_POWER_LINE_FIREC 0
#define GROUP_POWER_LINE_RANGE 0
#define GROUP_POWER_LINE_SIZE 1
#define POWER_LINE_LOSS 1       /* one MWH */
#define POWER_MODULUS 18        /* Controls how often we see a packet in anim */


#define TRANSPORT_RATE 8 //slows down the transport
#define TRANSPORT_QUANTA 1000 //granularity for transport flow

#define MAX_FOOD_ON_TRACK 2048
#define MAX_FOOD_ON_ROAD (MAX_FOOD_ON_TRACK*4)
#define MAX_FOOD_ON_RAIL (MAX_FOOD_ON_TRACK*16)
#define MAX_FOOD_IN_MARKET (MAX_FOOD_ON_RAIL*8)

#define MAX_JOBS_ON_TRACK 1024
#define MAX_JOBS_ON_ROAD (MAX_JOBS_ON_TRACK*5)
#define MAX_JOBS_ON_RAIL (MAX_JOBS_ON_TRACK*25)
#define MAX_JOBS_IN_MARKET (MAX_JOBS_ON_RAIL*3)

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

#define MAX_KWH_ON_TRACK 2048
#define MAX_KWH_ON_ROAD (MAX_KWH_ON_TRACK*4)
#define MAX_KWH_ON_RAIL (MAX_KWH_ON_TRACK*16)
#define KWH_LOSS_ON_TRANSPORT 3

#define MAX_MWH_ON_POWERLINE (MAX_KWH_ON_RAIL*8)

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



/** @file lincity/transport.h */

