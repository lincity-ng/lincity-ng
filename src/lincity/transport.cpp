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

    static const short table[16] = {
        0, 0, 1, 2,
        0, 0, 3, 4,
        1, 5, 1, 6,
        7, 8, 9, 10
    };

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
    for (int y = originy; y < originy+h; ++y)
    {
        for (int x = originx; x < originx+w; ++x)
        {
            // First, set up a mask according to directions
            cstr = world(x, y)->construction;
            int mask = 0;
            switch (world(x, y)->getGroup())
            {
            case GROUP_POWER_LINE:
            {
                bool far = false;
                int mwh = -1;
                world(x,y)->reportingConstruction->deneighborize();
                /* up -- (ThMO) */
                mwh = world(x, y-1)->reportingConstruction?
                world(x, y-1)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                if ((far = ((y > 1) && (world(x, y-1)->is_water() || world(x, y-1)->is_transport()))))
                {
                    mwh = world(x, y-2)->reportingConstruction?
                    world(x, y-2)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                }
                if(mwh != -1)
                {
                    if (far) //suspended cables
                    {
                        //opposite edge
                        if (!world(x, y-2)->reportingConstruction->countPowercables(1))
                        {
                            world(x, y-1)->flags |= FLAG_POWER_CABLES_0;
                            cstr->link_to(world(x,y-2)->reportingConstruction);
                            mask |=8;
                        }
                    }
                    else
                    {
                        cstr->link_to(world(x,y-1)->reportingConstruction);
                        mask |=8;
                    }
                }
                else
                {   world(x, y-1)->flags &= ~FLAG_POWER_CABLES_0;}

                /* left -- (ThMO) */
                mwh = world(x-1, y)->reportingConstruction?
                world(x-1, y)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                if((far = ((x > 1) && (world(x-1, y)->is_water() || world(x-1, y)->is_transport()))))
                {
                    mwh = world(x-2, y)->reportingConstruction?
                    world(x-2, y)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                }
                if(mwh != -1)
                {
                    if (far) //suspended cables
                    {
                        //opposite edge
                        if ( !world(x-2, y)->reportingConstruction->countPowercables(2) )
                        {
                            cstr->link_to(world(x-2,y)->reportingConstruction);
                            world(x-1, y)->flags |= FLAG_POWER_CABLES_90;
                            mask |=4;
                        }
                    }
                    else
                    {
                        cstr->link_to(world(x-1,y)->reportingConstruction);
                        mask |=4;
                    }
                }
                else
                {   world(x-1, y)->flags &= ~FLAG_POWER_CABLES_90;}

                /* right -- (ThMO) */
                mwh = world(x+1, y)->reportingConstruction?
                world(x+1, y)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                if ((far = ((x < world.len() - 2) && (world(x+1, y)->is_water() || world(x+1, y)->is_transport()))))
                {
                    mwh = world(x+2, y)->reportingConstruction?
                    world(x+2, y)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                }
                if(mwh != -1)
                {
                    if (far) //suspended cables
                    {
                        //opposite edge
                        if ( !world(x+2, y)->reportingConstruction->countPowercables(4) )
                        {
                            cstr->link_to(world(x+2,y)->reportingConstruction);
                            world(x+1, y)->flags |= FLAG_POWER_CABLES_90;
                            mask |=2;
                        }
                    }
                    else
                    {
                        cstr->link_to(world(x+1,y)->reportingConstruction);
                        mask |=2;
                    }
                }
                else
                {   world(x+1, y)->flags &= ~FLAG_POWER_CABLES_90;}

                /* down -- (ThMO) */
                mwh = world(x, y+1)->reportingConstruction?
                world(x, y+1)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;
                if ((far = (y < world.len() - 2) && (world(x, y+1)->is_water() || world(x, y+1)->is_transport())))
                {
                    mwh = world(x, y+2)->reportingConstruction?
                    world(x, y+2)->reportingConstruction->tellstuff(Construction::STUFF_MWH, -2):-1;}
                if(mwh != -1)
                {
                    if (far) //suspended cables
                    {
                        //opposite edge
                        if ( !world(x, y+2)->reportingConstruction->countPowercables(8) )
                        {
                            cstr->link_to(world(x,y+2)->reportingConstruction);
                            world(x, y+1)->flags |= FLAG_POWER_CABLES_0;
                            mask |=1;
                        }
                    }
                    else
                    {
                        cstr->link_to(world(x,y+1)->reportingConstruction);
                        mask |=1;
                    }
                }
                else
                {   world(x, y+1)->flags &= ~FLAG_POWER_CABLES_0;}

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
                //only brige entrances (and bridges) are transparent
                if (cstr->type >= 11 && cstr->type <= 12)
                {   cstr->flags |= FLAG_TRANSPARENT;}
                else
                {   cstr->flags &= (~FLAG_TRANSPARENT);}
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
                cstr->flags |= FLAG_TRANSPARENT;
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
                if(cstr->type >= 11 && cstr->type <= 16)
                {   cstr->flags |= FLAG_TRANSPARENT;}
                else
                {   cstr->flags &= (~FLAG_TRANSPARENT);}
                break;

            case GROUP_ROAD_BRIDGE:
                // Bridge neighbour priority
                if (check_group(x, y-1) == GROUP_ROAD_BRIDGE || check_group(x, y+1) == GROUP_ROAD_BRIDGE)
                {   cstr->type = 0;}
                else if (check_group(x-1, y) == GROUP_ROAD_BRIDGE || check_group(x+1, y) == GROUP_ROAD_BRIDGE)
                {   cstr->type = 1;}
                else if (check_group(x, y-1) == GROUP_ROAD || check_group(x, y+1) == GROUP_ROAD)
                {   world(x, y)->construction->type = 0;}//2
                else if (check_group(x-1, y) == GROUP_ROAD || check_group(x+1, y) == GROUP_ROAD)
                {   world(x, y)->construction->type = 1;}//3
                else
                {  cstr->type = 1;}
                cstr->flags |= FLAG_TRANSPARENT;
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
                {   cstr->type = 13;}
                else if (check_group(x-1, y) == GROUP_RAIL_BRIDGE)
                {   cstr->type = 14;}
                else if (check_group(x, y+1) == GROUP_RAIL_BRIDGE)
                {   cstr->type = 15;}
                else if (check_group(x+1, y) == GROUP_RAIL_BRIDGE)
                {   cstr->type = 16;}
                // Build bridge entrance1
                else if (check_group(x, y-2) == GROUP_RAIL_BRIDGE && check_group(x, y-1) == GROUP_RAIL)
                {   cstr->type = 17;}
                else if (check_group(x-2, y) == GROUP_RAIL_BRIDGE && check_group(x-1, y) == GROUP_RAIL)
                {   cstr->type = 18;}
                else if (check_group(x, y+2) == GROUP_RAIL_BRIDGE && check_group(x, y+1) == GROUP_RAIL)
                {   cstr->type = 19;}
                else if (check_group(x+2, y) == GROUP_RAIL_BRIDGE && check_group(x+1, y) == GROUP_RAIL)
                {   cstr->type = 20;}
                //railroad crossings
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
                if(cstr->type >= 11 && cstr->type <= 16)
                {   cstr->flags |= FLAG_TRANSPARENT;}
                else
                {   cstr->flags &= (~FLAG_TRANSPARENT);}
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
                {   cstr->type = 1;}
                cstr->flags |= FLAG_TRANSPARENT;
                break;
/*          // now handled by desert_water_frontiers
            case GROUP_WATER:

                if ( ((y > 0) && (world(x, y-1)->getGroup() == GROUP_PORT))
                    || check_water(x, y - 1))
                {   mask |= 8;}

                if ( ((x > 0) && (world(x - 1, y)->getGroup() == GROUP_PORT))
                    || check_water(x - 1, y))
                {   mask |= 4;}

                if (check_water(x + 1, y))
                {   mask |= 2;}

                if (check_water(x, y + 1))
                {   mask |= 1;}

                world(x, y)->type = mask;
                break;
*/
            }                   /* end switch */
        }                       /* end for y*/
    }                           /* end for x*/
}

/** @file lincity/transport.cpp */

