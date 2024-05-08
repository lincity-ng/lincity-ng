/* ---------------------------------------------------------------------- *
 * transport.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#include "transport.h"

#include <stdio.h>                // for NULL
#include <list>                   // for _List_iterator

#include "commodities.hpp"        // for Commodity
#include "engglobs.h"             // for world
#include "engine.h"               // for check_group, check_topgroup
#include "groups.h"               // for GROUP_RAIL, GROUP_ROAD, GROUP_RAIL_...
#include "lin-city.h"             // for FLAG_TRANSPARENT, FLAG_INVISIBLE
#include "lintypes.h"             // for Construction
#include "modules/all_modules.h"  // for TransportConstructionGroup, railCon...
#include "resources.hpp"          // for ExtraFrame
#include "world.h"                // for World, MapTile



void connect_transport(int originx, int originy, int lastx, int lasty)
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
    {   originx = 1;}
    if (originy <= 0)
    {   originy = 1;}
    if (lastx >= world.len())
    {   lastx = world.len() - 1;}
    if (lasty >= world.len())
    {   lasty = world.len() - 1;}

    for (int y = originy; y <= lasty; ++y)
    {
        for (int x = originx; x <= lastx; ++x)
        {
            // First, set up a mask according to directions
            cstr = world(x, y)->construction;
            int* frame = cstr ? &(cstr->frameIt->frame) : NULL;
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
                world(x, y-1)->reportingConstruction->tellstuff(STUFF_MWH, -2):-1;
                if ((far = ((y > 1) && (world(x, y-1)->is_water() || world(x, y-1)->is_transport()))))
                {
                    mwh = world(x, y-2)->reportingConstruction?
                    world(x, y-2)->reportingConstruction->tellstuff(STUFF_MWH, -2):-1;
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
                world(x-1, y)->reportingConstruction->tellstuff(STUFF_MWH, -2):-1;
                if((far = ((x > 1) && (world(x-1, y)->is_water() || world(x-1, y)->is_transport()))))
                {
                    mwh = world(x-2, y)->reportingConstruction?
                    world(x-2, y)->reportingConstruction->tellstuff(STUFF_MWH, -2):-1;
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
                world(x+1, y)->reportingConstruction->tellstuff(STUFF_MWH, -2):-1;
                if ((far = ((x < world.len() - 2) && (world(x+1, y)->is_water() || world(x+1, y)->is_transport()))))
                {
                    mwh = world(x+2, y)->reportingConstruction?
                    world(x+2, y)->reportingConstruction->tellstuff(STUFF_MWH, -2):-1;
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
                world(x, y+1)->reportingConstruction->tellstuff(STUFF_MWH, -2):-1;
                if ((far = (y < world.len() - 2) && (world(x, y+1)->is_water() || world(x, y+1)->is_transport())))
                {
                    mwh = world(x, y+2)->reportingConstruction?
                    world(x, y+2)->reportingConstruction->tellstuff(STUFF_MWH, -2):-1;}
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

                *frame = power_table[mask];
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
                {   *frame = 11;}
                else if ((check_group(x-1, y) == GROUP_TRACK_BRIDGE && (
                        check_group(x+1, y) == GROUP_TRACK_BRIDGE || check_group(x+2, y) == GROUP_TRACK_BRIDGE))
                        || (check_group(x+1, y) == GROUP_TRACK_BRIDGE && (
                        check_group(x-1, y) == GROUP_TRACK_BRIDGE || check_group(x-2, y) == GROUP_TRACK_BRIDGE)))
                {   *frame = 12;}
                // Set according bridge entrance if any
                else if (check_group(x, y-1) == GROUP_TRACK_BRIDGE)
                {   *frame = 13;}
                else if (check_group(x-1, y) == GROUP_TRACK_BRIDGE)
                {   *frame = 14;}
                else if (check_group(x, y+1) == GROUP_TRACK_BRIDGE)
                {   *frame = 15;}
                else if (check_group(x+1, y) == GROUP_TRACK_BRIDGE)
                {   *frame = 16;}
                else if (check_group(x+1, y) == GROUP_RAIL &&
                         check_group(x-1, y) == GROUP_RAIL &&
                         check_group(x, y+1) == GROUP_TRACK &&
                         check_group(x, y-1) == GROUP_TRACK)
                {
                    railConstructionGroup.placeItem(x,y);
                    cstr = world(x,y)->construction;
                    frame = &(cstr->frameIt->frame);
                    *frame = 21;
                    //rewind the loops
                    x -= 2;
                    y -= 2;
                    if (x < originx)
                    {   x = originx;}
                    if (y < originy)
                    {   y = originy;}

                }
                else if (check_group(x, y+1) == GROUP_RAIL &&
                         check_group(x, y-1) == GROUP_RAIL &&
                         check_group(x+1, y) == GROUP_TRACK &&
                         check_group(x-1, y) == GROUP_TRACK)
                {
                    railConstructionGroup.placeItem(x,y);
                    cstr = world(x,y)->construction;
                    frame = &(cstr->frameIt->frame);
                    *frame = 22;
                    //rewind the loops
                    x -= 2;
                    y -= 2;
                    if (x < originx)
                    {   x = originx;}
                    if (y < originy)
                    {   y = originy;}
                }
                else
                {   *frame = table[mask];}
                //only brige entrances (and bridges) are transparent
                if (*frame >= 11 && *frame <= 12)
                {
                    cstr->flags |= FLAG_TRANSPARENT;
                    world(x,y)->flags &= (~FLAG_INVISIBLE);
                }
                else
                {
                   cstr->flags &= (~FLAG_TRANSPARENT);
                   world(x,y)->flags |= FLAG_INVISIBLE;
                }
                break;

            case GROUP_TRACK_BRIDGE:
                // Bridge neighbour priority
                if (check_group(x, y-1) == GROUP_TRACK_BRIDGE || check_group(x, y+1) == GROUP_TRACK_BRIDGE
                   || check_group(x, y-1) == GROUP_TRACK || check_group(x, y+1) == GROUP_TRACK)
                {
                    mask |= 2;
                    *frame = 0;
                }
                else if (check_group(x-1, y) == GROUP_TRACK_BRIDGE || check_group(x+1, y) == GROUP_TRACK_BRIDGE
                    || check_group(x-1, y) == GROUP_TRACK || check_group(x+1, y) == GROUP_TRACK)
                {
                    mask |= 1;
                    *frame = 1;
                }
                else //a lonely bridge tile
                {   *frame = 1;}
                cstr->flags |= FLAG_TRANSPARENT;
                world(x,y)->flags &= (~FLAG_INVISIBLE);
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
                {   *frame = 11;}
                else if ((check_group(x-1, y) == GROUP_ROAD_BRIDGE && (
                        check_group(x+1, y) == GROUP_ROAD_BRIDGE || check_group(x+2, y) == GROUP_ROAD_BRIDGE))
                        || (check_group(x+1, y) == GROUP_ROAD_BRIDGE && (
                        check_group(x-1, y) == GROUP_ROAD_BRIDGE || check_group(x-2, y) == GROUP_ROAD_BRIDGE)))
                {   *frame = 12;}
                // Build bridge entrance2
                else if (check_group(x, y-1) == GROUP_ROAD_BRIDGE)
                {   *frame = 13;}
                else if (check_group(x-1, y) == GROUP_ROAD_BRIDGE)
                {   *frame = 14;}
                else if (check_group(x, y+1) == GROUP_ROAD_BRIDGE)
                {   *frame = 15;}
                else if (check_group(x+1, y) == GROUP_ROAD_BRIDGE)
                {   *frame = 16;}
                // Build bridge entrance1
                else if (check_group(x, y-2) == GROUP_ROAD_BRIDGE && check_group(x, y-1) == GROUP_ROAD)
                {   *frame = 17;}
                else if (check_group(x-2, y) == GROUP_ROAD_BRIDGE && check_group(x-1, y) == GROUP_ROAD)
                {   *frame = 18;}
                else if (check_group(x, y+2) == GROUP_ROAD_BRIDGE && check_group(x, y+1) == GROUP_ROAD)
                {   *frame = 19;}
                else if (check_group(x+2, y) == GROUP_ROAD_BRIDGE && check_group(x+1, y) == GROUP_ROAD)
                {   *frame = 20;}
                else if (check_group(x+1, y) == GROUP_RAIL &&
                         check_group(x-1, y) == GROUP_RAIL &&
                         check_group(x, y+1) == GROUP_ROAD &&
                         check_group(x, y-1) == GROUP_ROAD)
                {
                    railConstructionGroup.placeItem(x,y);
                    cstr = world(x,y)->construction;
                    frame = &(cstr->frameIt->frame);
                    *frame = 23;
                    //rewind the loops
                    x -= 2;
                    y -= 2;
                    if (x < originx)
                    {   x = originx;}
                    if (y < originy)
                    {   y = originy;}
                }
                else if (check_group(x, y+1) == GROUP_RAIL &&
                         check_group(x, y-1) == GROUP_RAIL &&
                         check_group(x+1, y) == GROUP_ROAD &&
                         check_group(x-1, y) == GROUP_ROAD)
                {
                    railConstructionGroup.placeItem(x,y);
                    cstr = world(x,y)->construction;
                    frame = &(cstr->frameIt->frame);
                    *frame = 24;
                    //rewind the loops
                    x -= 2;
                    y -= 2;
                    if (x < originx)
                    {   x = originx;}
                    if (y < originy)
                    {   y = originy;}
                }
                else
                {   *frame = table[mask];}
                if(*frame >= 11 && *frame <= 16)
                {
                    cstr->flags |= FLAG_TRANSPARENT;
                    world(x,y)->flags &= (~FLAG_INVISIBLE);
                }
                else
                {
                   cstr->flags &= (~FLAG_TRANSPARENT);
                   world(x,y)->flags |= FLAG_INVISIBLE;
                }
                break;

            case GROUP_ROAD_BRIDGE:
                // Bridge neighbour priority
                if (check_group(x, y-1) == GROUP_ROAD_BRIDGE || check_group(x, y+1) == GROUP_ROAD_BRIDGE)
                {   *frame = 0;}
                else if (check_group(x-1, y) == GROUP_ROAD_BRIDGE || check_group(x+1, y) == GROUP_ROAD_BRIDGE)
                {   *frame = 1;}
                else if (check_group(x, y-1) == GROUP_ROAD || check_group(x, y+1) == GROUP_ROAD)
                {   *frame = 0;}//2
                else if (check_group(x-1, y) == GROUP_ROAD || check_group(x+1, y) == GROUP_ROAD)
                {   *frame = 1;}//3
                else
                {  *frame = 1;}
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
                {   *frame = 11;}
                else if ((check_group(x-1, y) == GROUP_RAIL_BRIDGE && (
                        check_group(x+1, y) == GROUP_RAIL_BRIDGE || check_group(x+2, y) == GROUP_RAIL_BRIDGE))
                        || (check_group(x+1, y) == GROUP_RAIL_BRIDGE && (
                        check_group(x-1, y) == GROUP_RAIL_BRIDGE || check_group(x-2, y) == GROUP_RAIL_BRIDGE)))
                {   *frame = 12;}
                // Build bridge entrance2
                else if (check_group(x, y-1) == GROUP_RAIL_BRIDGE)
                {   *frame = 13;}
                else if (check_group(x-1, y) == GROUP_RAIL_BRIDGE)
                {   *frame = 14;}
                else if (check_group(x, y+1) == GROUP_RAIL_BRIDGE)
                {   *frame = 15;}
                else if (check_group(x+1, y) == GROUP_RAIL_BRIDGE)
                {   *frame = 16;}
                // Build bridge entrance1
                else if (check_group(x, y-2) == GROUP_RAIL_BRIDGE && check_group(x, y-1) == GROUP_RAIL)
                {   *frame = 17;}
                else if (check_group(x-2, y) == GROUP_RAIL_BRIDGE && check_group(x-1, y) == GROUP_RAIL)
                {   *frame = 18;}
                else if (check_group(x, y+2) == GROUP_RAIL_BRIDGE && check_group(x, y+1) == GROUP_RAIL)
                {   *frame = 19;}
                else if (check_group(x+2, y) == GROUP_RAIL_BRIDGE && check_group(x+1, y) == GROUP_RAIL)
                {   *frame = 20;}
                //railroad crossings
                else if (check_group(x+1, y) == GROUP_TRACK &&
                         check_group(x-1, y) == GROUP_TRACK)
                {   *frame = 22; }
                else if (check_group(x, y+1) == GROUP_TRACK &&
                         check_group(x, y-1) == GROUP_TRACK)
                {   *frame = 21; }
                else if (check_group(x+1, y) == GROUP_ROAD &&
                         check_group(x-1, y) == GROUP_ROAD)
                {   *frame = 24; }
                else if (check_group(x, y+1) == GROUP_ROAD &&
                         check_group(x, y-1) == GROUP_ROAD)
                {   *frame = 23; }

                else
                {   *frame = table[mask];}
                if(*frame >= 11 && *frame <= 16)
                {
                    cstr->flags |= FLAG_TRANSPARENT;
                    world(x,y)->flags &= (~FLAG_INVISIBLE);
                }
                else
                {
                       cstr->flags &= (~FLAG_TRANSPARENT);
                       world(x,y)->flags |= FLAG_INVISIBLE;
                }
                break;

            case GROUP_RAIL_BRIDGE:
                // Bridge neighbour priority
                if (check_group(x, y-1) == GROUP_RAIL_BRIDGE || check_group(x, y+1) == GROUP_RAIL_BRIDGE
                   || check_group(x, y-1) == GROUP_RAIL || check_group(x, y+1) == GROUP_RAIL)
                {   *frame = 0;}
                else if (check_group(x-1, y) == GROUP_RAIL_BRIDGE || check_group(x+1, y) == GROUP_RAIL_BRIDGE
                    || check_group(x-1, y) == GROUP_RAIL || check_group(x+1, y) == GROUP_RAIL)
                {   *frame = 1;}
                else
                {   *frame = 1;}
                cstr->flags |= FLAG_TRANSPARENT;
                world(x,y)->flags &= (~FLAG_INVISIBLE);
                break;
            }                   /* end switch */
        }                       /* end for y*/
    }                           /* end for x*/
}

/** @file lincity/transport.cpp */
