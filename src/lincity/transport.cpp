/* ---------------------------------------------------------------------- *
 * transport.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lin-city.h"
#include "lctypes.h"
#include "transport.h"
#include "all_buildings.h"
#include "engglobs.h"
#include "engine.h"
#include <iostream>


void connect_transport(int originx, int originy, int w, int h)
{
    // sets the correct TYPE depending on neighbours, => gives the correct tile to display
    static const short power_table[16] = {
        CST_POWERL_H_D, CST_POWERL_V_D, CST_POWERL_H_D, CST_POWERL_RD_D,
        CST_POWERL_H_D, CST_POWERL_LD_D, CST_POWERL_H_D, CST_POWERL_LDR_D,
        CST_POWERL_V_D, CST_POWERL_V_D, CST_POWERL_RU_D, CST_POWERL_UDR_D,
        CST_POWERL_LU_D, CST_POWERL_LDU_D, CST_POWERL_LUR_D, CST_POWERL_LUDR_D
    };
    static const short track_table[16] = {
        CST_TRACK_LR, CST_TRACK_LR, CST_TRACK_UD, CST_TRACK_LU,
        CST_TRACK_LR, CST_TRACK_LR, CST_TRACK_UR, CST_TRACK_LUR,
        CST_TRACK_UD, CST_TRACK_LD, CST_TRACK_UD, CST_TRACK_LUD,
        CST_TRACK_DR, CST_TRACK_LDR, CST_TRACK_UDR, CST_TRACK_LUDR
    };
    static const short road_table[16] = {
        CST_ROAD_LR, CST_ROAD_LR, CST_ROAD_UD, CST_ROAD_LU,
        CST_ROAD_LR, CST_ROAD_LR, CST_ROAD_UR, CST_ROAD_LUR,
        CST_ROAD_UD, CST_ROAD_LD, CST_ROAD_UD, CST_ROAD_LUD,
        CST_ROAD_DR, CST_ROAD_LDR, CST_ROAD_UDR, CST_ROAD_LUDR
    };
    static const short rail_table[16] = {
        CST_RAIL_LR, CST_RAIL_LR, CST_RAIL_UD, CST_RAIL_LU,
        CST_RAIL_LR, CST_RAIL_LR, CST_RAIL_UR, CST_RAIL_LUR,
        CST_RAIL_UD, CST_RAIL_LD, CST_RAIL_UD, CST_RAIL_LUD,
        CST_RAIL_DR, CST_RAIL_LDR, CST_RAIL_UDR, CST_RAIL_LUDR
    };
    static const short water_table[16] = {
        CST_WATER, CST_WATER_D, CST_WATER_R, CST_WATER_RD,
        CST_WATER_L, CST_WATER_LD, CST_WATER_LR, CST_WATER_LRD,
        CST_WATER_U, CST_WATER_UD, CST_WATER_UR, CST_WATER_URD,
        CST_WATER_LU, CST_WATER_LUD, CST_WATER_LUR, CST_WATER_LURD
    };

#if FLAG_LEFT != 1 || FLAG_UP != 2 || FLAG_RIGHT != 4 || FLAG_DOWN != 8
#error  connect_transport(): you loose
#error  the algorithm depends on proper flag settings -- (ThMO)
#endif

    /* Adjust originx,originy,w,h to proper range */
    if (originx <= 0)
    {
        originx = 1;
        w -= 1 - originx;
    }
    if (originy <= 0)
    {
        originy = 1;
        h -= 1 - originy;
    }
    if (originx + w >= world.len())
    {   w = world.len() - originx;}
    if (originy + h >= world.len())
    {   h = world.len() - originy;}
    int mask0 = ~(FLAG_LEFT |  FLAG_UP | FLAG_RIGHT | FLAG_DOWN );
    for (int x = originx; x < originx + w; ++x)
    {
        for (int y = originy; y < originy + h; ++y)
        {
            // First, set up a mask according to directions
            int mask = 0;
            int mwh = -1;
            switch (world(x, y)->getGroup())
            {
            case GROUP_POWER_LINE:
            {
                bool far = false;
                world(x,y)->reportingConstruction->deneighborize();
                /* power may be transferred */
                /* up -- (ThMO) */
                mwh = world(x, y-1)->reportingConstruction?
                world(x, y-1)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -1):-1;
                /* see if dug under track, rail or road */
                if ((far = ((y > 1) && (world(x, y-1)->is_water() || world(x, y-1)->is_transport()))))
                {
                    mwh = world(x, y-2)->reportingConstruction?
                    world(x, y-2)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -1):-1;
                }
                if(mwh != -1)
                {
                    mask |=8;
                    int y2 = far?(y-2):(y-1);
                    world(x,y)->reportingConstruction->link_to(world(x,y2)->reportingConstruction);
                }

                /* left -- (ThMO) */
                mwh = world(x-1, y)->reportingConstruction?
                world(x-1, y)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -1):-1;
                if((far = ((x > 1) && (world(x-1, y)->is_water() || world(x-1, y)->is_transport()))))
                {
                    mwh = world(x-2, y)->reportingConstruction?
                    world(x-2, y)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -1):-1;
                }
                if(mwh != -1)
                {
                    mask |=4;
                    int x2 = far?(x-2):(x-1);
                    world(x,y)->reportingConstruction->link_to(world(x2,y)->reportingConstruction);
                }

                /* right -- (ThMO) */
                mwh = world(x+1, y)->reportingConstruction?
                world(x+1, y)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -1):-1;
                if ((far = ((x < world.len() - 2) && (world(x+1, y)->is_water() || world(x+1, y)->is_transport()))))
                {
                    mwh = world(x+2, y)->reportingConstruction?
                    world(x+2, y)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -1):-1;
                }
                if(mwh != -1)
                {
                    mask |=2;
                    int x2 = far?(x+2):(x+1);
                    world(x,y)->reportingConstruction->link_to(world(x2,y)->reportingConstruction);
                }

                /* down -- (ThMO) */
                mwh = world(x, y+1)->reportingConstruction?
                world(x, y+1)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -1):-1;
                if ((far = (y < world.len() - 2) && (world(x, y+1)->is_water() || world(x, y+1)->is_transport())))
                {
                    mwh = world(x, y+2)->reportingConstruction?
                    world(x, y+2)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -1):-1;}
                if(mwh != -1)
                {
                    mask |=1;
                    int y2 = far?(y+2):(y+1);
                    world(x,y)->reportingConstruction->link_to(world(x,y2)->reportingConstruction);
                }
                /* Next, set the connectivity into MP_TYPE */
                world(x, y)->construction->type = power_table[mask];
                world(x, y)->construction->flags &= mask0; // clear connection flags
                world(x, y)->construction->flags |= mask; // set connection flags
                break;
            }
            case GROUP_TRACK:
                if (check_group(x, y - 1) == GROUP_TRACK
                ||  check_group(x, y - 1) == GROUP_ROAD)
                {   mask |= FLAG_UP;}
                if (check_group(x - 1, y) == GROUP_TRACK
                ||  check_group(x - 1, y) == GROUP_ROAD)
                {   mask |= FLAG_LEFT;}

                switch (check_topgroup(x + 1, y))
                {
                    case GROUP_ROAD:
                    case GROUP_TRACK:
                    case GROUP_TRACK_BRIDGE:
                    case GROUP_COMMUNE:
                    case GROUP_COALMINE:
                    case GROUP_OREMINE:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                    case GROUP_RECYCLE:
                    case GROUP_TIP:
                    case GROUP_PORT:
                    case GROUP_COAL_POWER:
                        mask |= FLAG_RIGHT;
                        break;
                }

                switch (check_topgroup(x, y + 1))
                {
                    case GROUP_ROAD:
                    case GROUP_TRACK:
                    case GROUP_TRACK_BRIDGE:
                    case GROUP_COMMUNE:
                    case GROUP_COALMINE:
                    case GROUP_OREMINE:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                    case GROUP_RECYCLE:
                    case GROUP_TIP:
                    case GROUP_PORT:
                    case GROUP_COAL_POWER:
                        mask |= FLAG_DOWN;
                        break;
                }
                world(x, y)->construction->flags &= mask0;
                world(x, y)->construction->flags |= mask;

                // A track section between 2 bridge sections
                // in this special case we use a pillar bridge section with green
                if ((check_group(x, y-1) == GROUP_TRACK_BRIDGE && (
                        check_group(x, y+1) == GROUP_TRACK_BRIDGE || check_group(x, y+2) == GROUP_TRACK_BRIDGE))
                        || (check_group(x, y+1) == GROUP_TRACK_BRIDGE && (
                        check_group(x, y-1) == GROUP_TRACK_BRIDGE || check_group(x, y-2) == GROUP_TRACK_BRIDGE)))
                {   world(x, y)->construction->type = CST_TRACK_BRIDGE_UDP;}
                else if ((check_group(x-1, y) == GROUP_TRACK_BRIDGE && (
                        check_group(x+1, y) == GROUP_TRACK_BRIDGE || check_group(x+2, y) == GROUP_TRACK_BRIDGE))
                        || (check_group(x+1, y) == GROUP_TRACK_BRIDGE && (
                        check_group(x-1, y) == GROUP_TRACK_BRIDGE || check_group(x-2, y) == GROUP_TRACK_BRIDGE)))
                {   world(x, y)->construction->type = CST_TRACK_BRIDGE_LRP;}
                // Set according bridge entrance if any
                else if (check_group(x, y-1) == GROUP_TRACK_BRIDGE)
                {   world(x, y)->construction->type = CST_TRACK_BRIDGE_OUD;}
                else if (check_group(x-1, y) == GROUP_TRACK_BRIDGE)
                {   world(x, y)->construction->type = CST_TRACK_BRIDGE_OLR;}
                else if (check_group(x, y+1) == GROUP_TRACK_BRIDGE)
                {   world(x, y)->construction->type = CST_TRACK_BRIDGE_IUD;}
                else if (check_group(x+1, y) == GROUP_TRACK_BRIDGE)
                {   world(x, y)->construction->type = CST_TRACK_BRIDGE_ILR;}
                else
                {   world(x, y)->construction->type = track_table[mask];}
                break;

            case GROUP_TRACK_BRIDGE:
                // Bridge neighbour priority
                if (check_group(x, y-1) == GROUP_TRACK_BRIDGE || check_group(x, y+1) == GROUP_TRACK_BRIDGE
                   || check_group(x, y-1) == GROUP_TRACK || check_group(x, y+1) == GROUP_TRACK)
                {
                    mask |= FLAG_UP;
                    world(x, y)->construction->type = CST_TRACK_BRIDGE_UD;
                }
                else if (check_group(x-1, y) == GROUP_TRACK_BRIDGE || check_group(x+1, y) == GROUP_TRACK_BRIDGE
                    || check_group(x-1, y) == GROUP_TRACK || check_group(x+1, y) == GROUP_TRACK)
                {
                    mask |= FLAG_LEFT;
                    world(x, y)->construction->type = CST_TRACK_BRIDGE_LR;
                }
                else //a lonely bridge tile
                {   world(x, y)->construction->type = CST_TRACK_BRIDGE_LR;}
                world(x, y)->construction->flags &= mask0;
                world(x, y)->construction->flags |= mask;
                break;

            case GROUP_ROAD:
                if (check_group(x, y - 1) == GROUP_ROAD
                ||  check_group(x, y - 1) == GROUP_TRACK)
                {   mask |= FLAG_UP;}
                if (check_group(x - 1, y) == GROUP_ROAD
                ||  check_group(x - 1, y) == GROUP_TRACK)
                {   mask |= FLAG_LEFT;}

                switch (check_topgroup(x + 1, y))
                {
                    case GROUP_TRACK:
                    case GROUP_ROAD:
                    case GROUP_COMMUNE:
                    case GROUP_COALMINE:
                    case GROUP_OREMINE:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                    case GROUP_RECYCLE:
                    case GROUP_TIP:
                    case GROUP_PORT:
                    case GROUP_COAL_POWER:
                        mask |= FLAG_RIGHT;
                        break;
                }
                switch (check_topgroup(x, y + 1))
                {
                    case GROUP_TRACK:
                    case GROUP_ROAD:
                    case GROUP_COMMUNE:
                    case GROUP_COALMINE:
                    case GROUP_OREMINE:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                    case GROUP_RECYCLE:
                    case GROUP_TIP:
                    case GROUP_PORT:
                    case GROUP_COAL_POWER:
                        mask |= FLAG_DOWN;
                        break;
                }
                world(x, y)->construction->flags &= mask0;
                world(x, y)->construction->flags |= mask; //tflags
                // A road section between 2 bridge sections
                // in this special case we use a pillar bridge section with green
                if ((check_group(x, y-1) == GROUP_ROAD_BRIDGE && (
                        check_group(x, y+1) == GROUP_ROAD_BRIDGE || check_group(x, y+2) == GROUP_ROAD_BRIDGE))
                        || (check_group(x, y+1) == GROUP_ROAD_BRIDGE && (
                        check_group(x, y-1) == GROUP_ROAD_BRIDGE || check_group(x, y-2) == GROUP_ROAD_BRIDGE)))
                {   world(x, y)->construction->type = CST_ROAD_BRIDGE_UDPG;}
                else if ((check_group(x-1, y) == GROUP_ROAD_BRIDGE && (
                        check_group(x+1, y) == GROUP_ROAD_BRIDGE || check_group(x+2, y) == GROUP_ROAD_BRIDGE))
                        || (check_group(x+1, y) == GROUP_ROAD_BRIDGE && (
                        check_group(x-1, y) == GROUP_ROAD_BRIDGE || check_group(x-2, y) == GROUP_ROAD_BRIDGE)))
                {   world(x, y)->construction->type = CST_ROAD_BRIDGE_LRPG;}
                // Build bridge entrance2
                else if (check_group(x, y-1) == GROUP_ROAD_BRIDGE)
                {   world(x, y)->construction->type = CST_ROAD_BRIDGE_O2UD;}
                else if (check_group(x-1, y) == GROUP_ROAD_BRIDGE)
                {   world(x, y)->construction->type = CST_ROAD_BRIDGE_O2LR;}
                else if (check_group(x, y+1) == GROUP_ROAD_BRIDGE)
                {   world(x, y)->construction->type = CST_ROAD_BRIDGE_I2UD;}
                else if (check_group(x+1, y) == GROUP_ROAD_BRIDGE)
                {   world(x, y)->construction->type = CST_ROAD_BRIDGE_I2LR;}
                // Build bridge entrance1
                else if (check_group(x, y-2) == GROUP_ROAD_BRIDGE && check_group(x, y-1) == GROUP_ROAD)
                {   world(x, y)->construction->type = CST_ROAD_BRIDGE_O1UD;}
                else if (check_group(x-2, y) == GROUP_ROAD_BRIDGE && check_group(x-1, y) == GROUP_ROAD)
                {   world(x, y)->construction->type = CST_ROAD_BRIDGE_O1LR;}
                else if (check_group(x, y+2) == GROUP_ROAD_BRIDGE && check_group(x, y+1) == GROUP_ROAD)
                {   world(x, y)->construction->type = CST_ROAD_BRIDGE_I1UD;}
                else if (check_group(x+2, y) == GROUP_ROAD_BRIDGE && check_group(x+1, y) == GROUP_ROAD)
                {   world(x, y)->construction->type = CST_ROAD_BRIDGE_I1LR;}
                else
                {   world(x, y)->construction->type = road_table[mask];}
                break;

            case GROUP_ROAD_BRIDGE:
                // Bridge neighbour priority
                if (check_group(x, y-1) == GROUP_ROAD_BRIDGE || check_group(x, y+1) == GROUP_ROAD_BRIDGE)
                {
                    mask |= FLAG_UP;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_UDP;
                }
                else if (check_group(x-1, y) == GROUP_ROAD_BRIDGE || check_group(x+1, y) == GROUP_ROAD_BRIDGE)
                {
                    mask |= FLAG_LEFT;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_LRP;
                }
                else if (check_group(x, y-1) == GROUP_ROAD || check_group(x, y+1) == GROUP_ROAD)
                {
                    mask |= FLAG_UP;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_UD;
                }
                else if (check_group(x-1, y) == GROUP_ROAD || check_group(x+1, y) == GROUP_ROAD)
                {
                    mask |= FLAG_LEFT;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_LR;
                }
                else
                {   world(x, y)->construction->type = CST_ROAD_BRIDGE_LRP;}
                world(x, y)->construction->flags &= mask0;
                world(x, y)->construction->flags |= mask;
                break;

            case GROUP_RAIL:
                if (check_group(x, y - 1) == GROUP_RAIL)
                {   mask |= FLAG_UP;}
                if (check_group(x - 1, y) == GROUP_RAIL)
                {   mask |= FLAG_LEFT;}

                switch (check_topgroup(x + 1, y)) {
                    case GROUP_RAIL:
                    case GROUP_COMMUNE:
                    case GROUP_COALMINE:
                    case GROUP_OREMINE:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                    case GROUP_RECYCLE:
                    case GROUP_TIP:
                    case GROUP_PORT:
                    case GROUP_COAL_POWER:
                        mask |= FLAG_RIGHT;
                        break;
                }
                switch (check_topgroup(x, y + 1)) {
                    case GROUP_RAIL:
                    case GROUP_COMMUNE:
                    case GROUP_COALMINE:
                    case GROUP_OREMINE:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                    case GROUP_RECYCLE:
                    case GROUP_TIP:
                    case GROUP_PORT:
                    case GROUP_COAL_POWER:
                        mask |= FLAG_DOWN;
                        break;
                }
                world(x, y)->construction->flags &= mask0;
                world(x, y)->construction->flags |= mask;
                // A rail section between 2 bridge sections
                // in this special case we use a pillar bridge section with green
                if ((check_group(x, y-1) == GROUP_RAIL_BRIDGE && (
                        check_group(x, y+1) == GROUP_RAIL_BRIDGE || check_group(x, y+2) == GROUP_RAIL_BRIDGE))
                        || (check_group(x, y+1) == GROUP_RAIL_BRIDGE && (
                        check_group(x, y-1) == GROUP_RAIL_BRIDGE || check_group(x, y-2) == GROUP_RAIL_BRIDGE)))
                {   world(x, y)->construction->type = CST_RAIL_BRIDGE_UDPG;}
                else if ((check_group(x-1, y) == GROUP_RAIL_BRIDGE && (
                        check_group(x+1, y) == GROUP_RAIL_BRIDGE || check_group(x+2, y) == GROUP_RAIL_BRIDGE))
                        || (check_group(x+1, y) == GROUP_RAIL_BRIDGE && (
                        check_group(x-1, y) == GROUP_RAIL_BRIDGE || check_group(x-2, y) == GROUP_RAIL_BRIDGE)))
                {   world(x, y)->construction->type = CST_RAIL_BRIDGE_LRPG;}
                // Build bridge entrance2
                else if (check_group(x, y-1) == GROUP_RAIL_BRIDGE)
                {   world(x, y)->construction->type = CST_RAIL_BRIDGE_O2UD;}
                else if (check_group(x-1, y) == GROUP_RAIL_BRIDGE)
                {   world(x, y)->construction->type = CST_RAIL_BRIDGE_O2LR;}
                else if (check_group(x, y+1) == GROUP_RAIL_BRIDGE)
                {   world(x, y)->construction->type = CST_RAIL_BRIDGE_I2UD;}
                else if (check_group(x+1, y) == GROUP_RAIL_BRIDGE)
                {   world(x, y)->construction->type = CST_RAIL_BRIDGE_I2LR;}
                // Build bridge entrance1
                else if (check_group(x, y-2) == GROUP_RAIL_BRIDGE && check_group(x, y-1) == GROUP_RAIL)
                {   world(x, y)->construction->type = CST_RAIL_BRIDGE_O1UD;}
                else if (check_group(x-2, y) == GROUP_RAIL_BRIDGE && check_group(x-1, y) == GROUP_RAIL)
                {   world(x, y)->construction->type = CST_RAIL_BRIDGE_O1LR;}
                else if (check_group(x, y+2) == GROUP_RAIL_BRIDGE && check_group(x, y+1) == GROUP_RAIL)
                {   world(x, y)->construction->type = CST_RAIL_BRIDGE_I1UD;}
                else if (check_group(x+2, y) == GROUP_RAIL_BRIDGE && check_group(x+1, y) == GROUP_RAIL)
                {   world(x, y)->construction->type = CST_RAIL_BRIDGE_I1LR;}
                else
                {   world(x, y)->construction->type = rail_table[mask];}
                break;

            case GROUP_RAIL_BRIDGE:
                // Bridge neighbour priority
                if (check_group(x, y-1) == GROUP_RAIL_BRIDGE || check_group(x, y+1) == GROUP_RAIL_BRIDGE
                   || check_group(x, y-1) == GROUP_RAIL || check_group(x, y+1) == GROUP_RAIL)
                {
                    mask |= FLAG_UP;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_UD;
                }
                else if (check_group(x-1, y) == GROUP_RAIL_BRIDGE || check_group(x+1, y) == GROUP_RAIL_BRIDGE
                    || check_group(x-1, y) == GROUP_RAIL || check_group(x+1, y) == GROUP_RAIL)
                {
                    mask |= FLAG_LEFT;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_LR;
                }
                else
                {world(x, y)->construction->type = CST_RAIL_BRIDGE_LR;}
                world(x, y)->construction->flags &= mask0;
                world(x, y)->construction->flags |= mask;
                break;

            case GROUP_WATER:
                /* up -- (ThMO) */
                if ( ((y > 0) && (world(x, y-1)->getGroup() == GROUP_PORT))
                    || check_water(x, y - 1))
                {   mask |= 8;}

                /* left -- (ThMO) */
                if ( ((x > 0) && (world(x - 1, y)->getGroup() == GROUP_PORT))
                    || check_water(x - 1, y))
                {   mask |= 4;}

                /* right -- (ThMO) */
                if (check_water(x + 1, y))
                {   mask |= 2;}

                /* down -- (ThMO) */
                if (check_water(x, y + 1))
                {   mask |= 1;}

                world(x, y)->type = water_table[mask];
                break;
            }                   /* end switch */
        }                       /* end for y*/
    }                           /* end for x*/
}

/** @file lincity/transport.cpp */

