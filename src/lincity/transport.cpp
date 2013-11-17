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
#include "modules/all_modules.h"
#include "engglobs.h"
#include "engine.h"
#include <iostream>



void connect_transport(int originx, int originy, int w, int h)
{
    // sets the correct TYPE depending on neighbours, => gives the correct tile to display
    static const short power_table[16] = {
        0, 1, 0, 2,
        0, 3, 0, 4,
        1, 1, 5, 6,
        7, 8, 9, 10
    };
/*
    static const short track_table[16] = {
        CST_TRACK_LR, CST_TRACK_LR, CST_TRACK_UD, CST_TRACK_LU,
        CST_TRACK_LR, CST_TRACK_LR, CST_TRACK_UR, CST_TRACK_LUR,
        CST_TRACK_UD, CST_TRACK_LD, CST_TRACK_UD, CST_TRACK_LUD,
        CST_TRACK_DR, CST_TRACK_LDR, CST_TRACK_UDR, CST_TRACK_LUDR
    };
*/
    static const short table[16] = {
        0, 0, 1, 2,
        0, 0, 3, 4,
        1, 5, 1, 6,
        7, 8, 9, 10
    };


/*
    static const short road_table[16] = {
        CST_ROAD_LR, CST_ROAD_LR, CST_ROAD_UD, CST_ROAD_LU,
        CST_ROAD_LR, CST_ROAD_LR, CST_ROAD_UR, CST_ROAD_LUR,
        CST_ROAD_UD, CST_ROAD_LD, CST_ROAD_UD, CST_ROAD_LUD,
        CST_ROAD_DR, CST_ROAD_LDR, CST_ROAD_UDR, CST_ROAD_LUDR
    };
*/
/*
    static const short rail_table[16] = {
        CST_RAIL_LR, CST_RAIL_LR, CST_RAIL_UD, CST_RAIL_LU,
        CST_RAIL_LR, CST_RAIL_LR, CST_RAIL_UR, CST_RAIL_LUR,
        CST_RAIL_UD, CST_RAIL_LD, CST_RAIL_UD, CST_RAIL_LUD,
        CST_RAIL_DR, CST_RAIL_LDR, CST_RAIL_UDR, CST_RAIL_LUDR
    };
*/
/*
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
*/

    Construction *cstr = 0;
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
    for (int x = originx; x < originx + w; ++x)
    {
        for (int y = originy; y < originy + h; ++y)
        {
            // First, set up a mask according to directions
            cstr = world(x, y)->construction;
            int mask = 0;
            int mwh = -1;
            switch (world(x, y)->getGroup())
            {
            case GROUP_POWER_LINE:
            {
                bool far = false;
                world(x,y)->reportingConstruction->deneighborize();
                //world(x,y)->flags &= ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
                /* power may be transferred */
                /* up -- (ThMO) */
                world(x, y-1)->flags &= ~FLAG_POWER_CABLES_0;
                mwh = world(x, y-1)->reportingConstruction?
                world(x, y-1)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                /* see if dug under track, rail or road */
                if ((far = ((y > 1) && (world(x, y-1)->is_water() || world(x, y-1)->is_transport()))))
                {
                    mwh = world(x, y-2)->reportingConstruction?
                    world(x, y-2)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                }
                if(mwh != -1)
                {
                    mask |=8;
                    int y2 = far?(y-2):(y-1);
                    cstr->link_to(world(x,y2)->reportingConstruction);
                    if (far) //suspended cables
                    {   world(x, y-1)->flags |= FLAG_POWER_CABLES_0;}
                }

                /* left -- (ThMO) */
                world(x-1, y)->flags &= ~FLAG_POWER_CABLES_90;
                mwh = world(x-1, y)->reportingConstruction?
                world(x-1, y)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                if((far = ((x > 1) && (world(x-1, y)->is_water() || world(x-1, y)->is_transport()))))
                {
                    mwh = world(x-2, y)->reportingConstruction?
                    world(x-2, y)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                }
                if(mwh != -1)
                {
                    mask |=4;
                    int x2 = far?(x-2):(x-1);
                    cstr->link_to(world(x2,y)->reportingConstruction);
                    if (far) //suspended cables
                    {   world(x-1, y)->flags |= FLAG_POWER_CABLES_90;}
                }

                /* right -- (ThMO) */
                world(x+1, y)->flags &= ~FLAG_POWER_CABLES_90;
                mwh = world(x+1, y)->reportingConstruction?
                world(x+1, y)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                if ((far = ((x < world.len() - 2) && (world(x+1, y)->is_water() || world(x+1, y)->is_transport()))))
                {
                    mwh = world(x+2, y)->reportingConstruction?
                    world(x+2, y)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                }
                if(mwh != -1)
                {
                    mask |=2;
                    int x2 = far?(x+2):(x+1);
                    cstr->link_to(world(x2,y)->reportingConstruction);
                    if (far) //suspended cables
                    {   world(x+1, y)->flags |= FLAG_POWER_CABLES_90;}
                }

                /* down -- (ThMO) */
                world(x, y+1)->flags &= ~FLAG_POWER_CABLES_0;
                mwh = world(x, y+1)->reportingConstruction?
                world(x, y+1)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                if ((far = (y < world.len() - 2) && (world(x, y+1)->is_water() || world(x, y+1)->is_transport())))
                {
                    mwh = world(x, y+2)->reportingConstruction?
                    world(x, y+2)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;}
                if(mwh != -1)
                {
                    mask |=1;
                    int y2 = far?(y+2):(y+1);
                    cstr->link_to(world(x,y2)->reportingConstruction);
                    if (far) //suspended cables
                    {   world(x, y+1)->flags |= FLAG_POWER_CABLES_0;}
                }

                cstr->type = power_table[mask];
                break;
            }
            case GROUP_TRACK:
                if (check_group(x, y - 1) == GROUP_TRACK
                ||  check_group(x, y - 1) == GROUP_ROAD
                || (check_group(x, y - 1) == GROUP_RAIL && //rail crossing
                    check_group(x, y - 2) == GROUP_TRACK))
                {   mask |= 2;}
                if (check_group(x - 1, y) == GROUP_TRACK
                ||  check_group(x - 1, y) == GROUP_ROAD
                || (check_group(x - 1, y) == GROUP_RAIL && //rail crossing
                    check_group(x - 2, y) == GROUP_TRACK))
                {   mask |= 1;}

                switch (check_topgroup(x + 1, y))
                {
                    case GROUP_RAIL:
                        if(check_group(x + 2, y) != GROUP_TRACK)
                        {   break;}
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
                        mask |= 4;
                        break;
                }

                switch (check_topgroup(x, y + 1))
                {
                    case GROUP_RAIL:
                        if(check_group(x, y + 2) != GROUP_TRACK)
                        {   break;}
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
                        mask |= 8;
                        break;
                }
                // A track section between 2 bridge sections
                // in this special case we use a pillar bridge section with green
                if ((check_group(x, y-1) == GROUP_TRACK_BRIDGE && (
                        check_group(x, y+1) == GROUP_TRACK_BRIDGE || check_group(x, y+2) == GROUP_TRACK_BRIDGE))
                        || (check_group(x, y+1) == GROUP_TRACK_BRIDGE && (
                        check_group(x, y-1) == GROUP_TRACK_BRIDGE || check_group(x, y-2) == GROUP_TRACK_BRIDGE)))
                {   cstr->type = 11;}
                else if ((check_group(x-1, y) == GROUP_TRACK_BRIDGE && (
                        check_group(x+1, y) == GROUP_TRACK_BRIDGE || check_group(x+2, y) == GROUP_TRACK_BRIDGE))
                        || (check_group(x+1, y) == GROUP_TRACK_BRIDGE && (
                        check_group(x-1, y) == GROUP_TRACK_BRIDGE || check_group(x-2, y) == GROUP_TRACK_BRIDGE)))
                {   cstr->type = 12;}
                // Set according bridge entrance if any
                else if (check_group(x, y-1) == GROUP_TRACK_BRIDGE)
                {   cstr->type = 13;}
                else if (check_group(x-1, y) == GROUP_TRACK_BRIDGE)
                {   cstr->type = 14;}
                else if (check_group(x, y+1) == GROUP_TRACK_BRIDGE)
                {   cstr->type = 15;}
                else if (check_group(x+1, y) == GROUP_TRACK_BRIDGE)
                {   cstr->type = 16;}
                else if (check_group(x+1, y) == GROUP_RAIL &&
                         check_group(x-1, y) == GROUP_RAIL &&
                         check_group(x, y+1) == GROUP_TRACK &&
                         check_group(x, y-1) == GROUP_TRACK)
                {
                    railConstructionGroup.placeItem(x,y);
                    cstr = world(x,y)->construction;
                    cstr->type = 21;
                    y = originy;
                }
                else if (check_group(x, y+1) == GROUP_RAIL &&
                         check_group(x, y-1) == GROUP_RAIL &&
                         check_group(x+1, y) == GROUP_TRACK &&
                         check_group(x-1, y) == GROUP_TRACK)
                {
                    railConstructionGroup.placeItem(x,y);
                    cstr = world(x,y)->construction;
                    cstr->type = 22;
                    x = originx;
                }
                else
                {   cstr->type = table[mask];}
                break;

            case GROUP_TRACK_BRIDGE:
                // Bridge neighbour priority
                if (check_group(x, y-1) == GROUP_TRACK_BRIDGE || check_group(x, y+1) == GROUP_TRACK_BRIDGE
                   || check_group(x, y-1) == GROUP_TRACK || check_group(x, y+1) == GROUP_TRACK)
                {
                    mask |= 2;
                    cstr->type = 0;
                }
                else if (check_group(x-1, y) == GROUP_TRACK_BRIDGE || check_group(x+1, y) == GROUP_TRACK_BRIDGE
                    || check_group(x-1, y) == GROUP_TRACK || check_group(x+1, y) == GROUP_TRACK)
                {
                    mask |= 1;
                    cstr->type = 1;
                }
                else //a lonely bridge tile
                {   cstr->type = 1;}
                break;

            case GROUP_ROAD:
                if (check_group(x, y - 1) == GROUP_ROAD
                ||  check_group(x, y - 1) == GROUP_TRACK
                || (check_group(x, y - 1) == GROUP_RAIL && //rail crossing
                    check_group(x, y - 2) == GROUP_ROAD))
                {   mask |= 2;}
                if (check_group(x - 1, y) == GROUP_ROAD
                ||  check_group(x - 1, y) == GROUP_TRACK
                || (check_group(x - 1, y) == GROUP_RAIL && //rail crossing
                    check_group(x - 2, y) == GROUP_ROAD))
                {   mask |= 1;}

                switch (check_topgroup(x + 1, y))
                {
                    case GROUP_RAIL:
                        if(check_group(x + 2, y) != GROUP_ROAD)
                        {   break;}
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
                        mask |= 4;
                        break;
                }
                switch (check_topgroup(x, y + 1))
                {
                    case GROUP_RAIL:
                        if(check_group(x, y + 2) != GROUP_ROAD)
                        {   break;}
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
                        mask |= 8;
                        break;
                }
                // A road section between 2 bridge sections
                // in this special case we use a pillar bridge section with green
                if ((check_group(x, y-1) == GROUP_ROAD_BRIDGE && (
                        check_group(x, y+1) == GROUP_ROAD_BRIDGE || check_group(x, y+2) == GROUP_ROAD_BRIDGE))
                        || (check_group(x, y+1) == GROUP_ROAD_BRIDGE && (
                        check_group(x, y-1) == GROUP_ROAD_BRIDGE || check_group(x, y-2) == GROUP_ROAD_BRIDGE)))
                {   cstr->type = 11;}
                else if ((check_group(x-1, y) == GROUP_ROAD_BRIDGE && (
                        check_group(x+1, y) == GROUP_ROAD_BRIDGE || check_group(x+2, y) == GROUP_ROAD_BRIDGE))
                        || (check_group(x+1, y) == GROUP_ROAD_BRIDGE && (
                        check_group(x-1, y) == GROUP_ROAD_BRIDGE || check_group(x-2, y) == GROUP_ROAD_BRIDGE)))
                {   cstr->type = 12;}
                // Build bridge entrance2
                else if (check_group(x, y-1) == GROUP_ROAD_BRIDGE)
                {   cstr->type = 13;}
                else if (check_group(x-1, y) == GROUP_ROAD_BRIDGE)
                {   cstr->type = 14;}
                else if (check_group(x, y+1) == GROUP_ROAD_BRIDGE)
                {   cstr->type = 15;}
                else if (check_group(x+1, y) == GROUP_ROAD_BRIDGE)
                {   cstr->type = 16;}
                // Build bridge entrance1
                else if (check_group(x, y-2) == GROUP_ROAD_BRIDGE && check_group(x, y-1) == GROUP_ROAD)
                {   cstr->type = 17;}
                else if (check_group(x-2, y) == GROUP_ROAD_BRIDGE && check_group(x-1, y) == GROUP_ROAD)
                {   cstr->type = 18;}
                else if (check_group(x, y+2) == GROUP_ROAD_BRIDGE && check_group(x, y+1) == GROUP_ROAD)
                {   cstr->type = 19;}
                else if (check_group(x+2, y) == GROUP_ROAD_BRIDGE && check_group(x+1, y) == GROUP_ROAD)
                {   cstr->type = 20;}
                else if (check_group(x+1, y) == GROUP_RAIL &&
                         check_group(x-1, y) == GROUP_RAIL &&
                         check_group(x, y+1) == GROUP_ROAD &&
                         check_group(x, y-1) == GROUP_ROAD)
                {
                    railConstructionGroup.placeItem(x,y);
                    cstr = world(x,y)->construction;
                    cstr->type = 23;
                    y = originy;
                }
                else if (check_group(x, y+1) == GROUP_RAIL &&
                         check_group(x, y-1) == GROUP_RAIL &&
                         check_group(x+1, y) == GROUP_ROAD &&
                         check_group(x-1, y) == GROUP_ROAD)
                {
                    railConstructionGroup.placeItem(x,y);
                    cstr = world(x,y)->construction;
                    cstr->type = 24;
                    x = originx;
                }
                else
                {   cstr->type = table[mask];}
                break;

            case GROUP_ROAD_BRIDGE:
                // Bridge neighbour priority
                if (check_group(x, y-1) == GROUP_ROAD_BRIDGE || check_group(x, y+1) == GROUP_ROAD_BRIDGE)
                {   world(x, y)->construction->type = 0;}
                else if (check_group(x-1, y) == GROUP_ROAD_BRIDGE || check_group(x+1, y) == GROUP_ROAD_BRIDGE)
                {   world(x, y)->construction->type = 1;}
                else if (check_group(x, y-1) == GROUP_ROAD || check_group(x, y+1) == GROUP_ROAD)
                {   world(x, y)->construction->type = 2;}
                else if (check_group(x-1, y) == GROUP_ROAD || check_group(x+1, y) == GROUP_ROAD)
                {   world(x, y)->construction->type = 3;}
                else
                {   world(x, y)->construction->type = 1;}
                break;

            case GROUP_RAIL:
                if (check_group(x, y - 1) == GROUP_RAIL)
                {   mask |= 2;}
                if (check_group(x - 1, y) == GROUP_RAIL)
                {   mask |= 1;}

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
                        mask |= 4;
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
                        mask |= 8;
                        break;
                }
                // A rail section between 2 bridge sections
                // in this special case we use a pillar bridge section with green
                if ((check_group(x, y-1) == GROUP_RAIL_BRIDGE && (
                        check_group(x, y+1) == GROUP_RAIL_BRIDGE || check_group(x, y+2) == GROUP_RAIL_BRIDGE))
                        || (check_group(x, y+1) == GROUP_RAIL_BRIDGE && (
                        check_group(x, y-1) == GROUP_RAIL_BRIDGE || check_group(x, y-2) == GROUP_RAIL_BRIDGE)))
                {   cstr->type = 11;}
                else if ((check_group(x-1, y) == GROUP_RAIL_BRIDGE && (
                        check_group(x+1, y) == GROUP_RAIL_BRIDGE || check_group(x+2, y) == GROUP_RAIL_BRIDGE))
                        || (check_group(x+1, y) == GROUP_RAIL_BRIDGE && (
                        check_group(x-1, y) == GROUP_RAIL_BRIDGE || check_group(x-2, y) == GROUP_RAIL_BRIDGE)))
                {   cstr->type = 12;}
                // Build bridge entrance2
                else if (check_group(x, y-1) == GROUP_RAIL_BRIDGE)
                {   cstr->type = 13;}// CST_RAIL_BRIDGE_O2UD;}
                else if (check_group(x-1, y) == GROUP_RAIL_BRIDGE)
                {   cstr->type = 14;}//CST_RAIL_BRIDGE_O2LR;}
                else if (check_group(x, y+1) == GROUP_RAIL_BRIDGE)
                {   cstr->type = 15;}//CST_RAIL_BRIDGE_I2UD;}
                else if (check_group(x+1, y) == GROUP_RAIL_BRIDGE)
                {   cstr->type = 16;}//CST_RAIL_BRIDGE_I2LR;}
                // Build bridge entrance1
                else if (check_group(x, y-2) == GROUP_RAIL_BRIDGE && check_group(x, y-1) == GROUP_RAIL)
                {   cstr->type = 17;}//CST_RAIL_BRIDGE_O1UD;}
                else if (check_group(x-2, y) == GROUP_RAIL_BRIDGE && check_group(x-1, y) == GROUP_RAIL)
                {   cstr->type = 18;}//CST_RAIL_BRIDGE_O1LR;}
                else if (check_group(x, y+2) == GROUP_RAIL_BRIDGE && check_group(x, y+1) == GROUP_RAIL)
                {   cstr->type = 19;}//CST_RAIL_BRIDGE_I1UD;}
                else if (check_group(x+2, y) == GROUP_RAIL_BRIDGE && check_group(x+1, y) == GROUP_RAIL)
                {   cstr->type = 20;}//CST_RAIL_BRIDGE_I1LR;}
                else if (check_group(x+1, y) == GROUP_TRACK &&
                         check_group(x-1, y) == GROUP_TRACK)
                {   cstr->type = 22; }
                else if (check_group(x, y+1) == GROUP_TRACK &&
                         check_group(x, y-1) == GROUP_TRACK)
                {   cstr->type = 21; }
                else if (check_group(x+1, y) == GROUP_ROAD &&
                         check_group(x-1, y) == GROUP_ROAD)
                {   cstr->type = 24; }
                else if (check_group(x, y+1) == GROUP_ROAD &&
                         check_group(x, y-1) == GROUP_ROAD)
                {   cstr->type = 23; }

                else
                {   cstr->type = table[mask];}
                break;

            case GROUP_RAIL_BRIDGE:
                // Bridge neighbour priority
                if (check_group(x, y-1) == GROUP_RAIL_BRIDGE || check_group(x, y+1) == GROUP_RAIL_BRIDGE
                   || check_group(x, y-1) == GROUP_RAIL || check_group(x, y+1) == GROUP_RAIL)
                {   cstr->type = 0;}
                else if (check_group(x-1, y) == GROUP_RAIL_BRIDGE || check_group(x+1, y) == GROUP_RAIL_BRIDGE
                    || check_group(x-1, y) == GROUP_RAIL || check_group(x+1, y) == GROUP_RAIL)
                {   cstr->type = 1;}
                else
                {world(x, y)->construction->type = 1;}
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

                world(x, y)->type = mask;
                break;
            }                   /* end switch */
        }                       /* end for y*/
    }                           /* end for x*/
}

/** @file lincity/transport.cpp */

