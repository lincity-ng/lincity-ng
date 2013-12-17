/* ---------------------------------------------------------------------- *
 * track_road_powerline.cpp
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "track_road_rail.h"
#include "fire.h"
#include "lincity-ng/Sound.hpp"

// Track:
TransportConstructionGroup trackConstructionGroup(
    "Track",
    FALSE,                     /* need credit? */
    GROUP_TRACK,
    GROUP_TRANSPORT_SIZE,
    GROUP_TRACK_COLOUR,
    GROUP_TRACK_COST_MUL,
    GROUP_TRACK_BUL_COST,
    GROUP_TRACK_FIREC,
    GROUP_TRACK_COST,
    GROUP_TRACK_TECH,
    GROUP_TRANSPORT_RANGE
);

// TrackBridge:
TransportConstructionGroup trackbridgeConstructionGroup(
    "Track (Bridge)",
    FALSE,                     /* need credit? */
    GROUP_TRACK_BRIDGE,
    GROUP_TRANSPORT_SIZE,
    GROUP_TRACK_BRIDGE_COLOUR,
    GROUP_TRACK_BRIDGE_COST_MUL,
    GROUP_TRACK_BRIDGE_BUL_COST,
    GROUP_TRACK_BRIDGE_FIREC,
    GROUP_TRACK_BRIDGE_COST,
    GROUP_TRACK_BRIDGE_TECH,
    GROUP_TRANSPORT_RANGE
);

// Road:
TransportConstructionGroup roadConstructionGroup(
    "Road",
    FALSE,                     /* need credit? */
    GROUP_ROAD,
    GROUP_TRANSPORT_SIZE,
    GROUP_ROAD_COLOUR,
    GROUP_ROAD_COST_MUL,
    GROUP_ROAD_BUL_COST,
    GROUP_ROAD_FIREC,
    GROUP_ROAD_COST,
    GROUP_ROAD_TECH,
    GROUP_TRANSPORT_RANGE
);
// RoadBridge:
TransportConstructionGroup roadbridgeConstructionGroup(
    "Road (Bridge)",
    FALSE,                     /* need credit? */
    GROUP_ROAD_BRIDGE,
    GROUP_TRANSPORT_SIZE,
    GROUP_ROAD_BRIDGE_COLOUR,
    GROUP_ROAD_BRIDGE_COST_MUL,
    GROUP_ROAD_BRIDGE_BUL_COST,
    GROUP_ROAD_BRIDGE_FIREC,
    GROUP_ROAD_BRIDGE_COST,
    GROUP_ROAD_BRIDGE_TECH,
    GROUP_TRANSPORT_RANGE
);


// Rail:
TransportConstructionGroup railConstructionGroup(
    "Rail",
    FALSE,                     /* need credit? */
    GROUP_RAIL,
    GROUP_TRANSPORT_SIZE,
    GROUP_RAIL_COLOUR,
    GROUP_RAIL_COST_MUL,
    GROUP_RAIL_BUL_COST,
    GROUP_RAIL_FIREC,
    GROUP_RAIL_COST,
    GROUP_RAIL_TECH,
    GROUP_TRANSPORT_RANGE
);
// RailBridge:
TransportConstructionGroup railbridgeConstructionGroup(
    "Rail (Bridge)",
    FALSE,                     /* need credit? */
    GROUP_RAIL_BRIDGE,
    GROUP_TRANSPORT_SIZE,
    GROUP_RAIL_BRIDGE_COLOUR,
    GROUP_RAIL_BRIDGE_COST_MUL,
    GROUP_RAIL_BRIDGE_BUL_COST,
    GROUP_RAIL_BRIDGE_FIREC,
    GROUP_RAIL_BRIDGE_COST,
    GROUP_RAIL_BRIDGE_TECH,
    GROUP_TRANSPORT_RANGE
);

Construction *TransportConstructionGroup::createConstruction(int x, int y)
{
    return new Transport(x, y, this);
}

void Transport::update()
{
    switch (constructionGroup->group)
    {
        case GROUP_TRACK:
        case GROUP_TRACK_BRIDGE:
            //tracks have no side effects
        break;
        case GROUP_ROAD:
        case GROUP_ROAD_BRIDGE:
            ++transport_cost;
            if (total_time % DAYS_PER_ROAD_POLLUTION == 0)
                world(x,y)->pollution += ROAD_POLLUTION;
            if ((total_time & ROAD_GOODS_USED_MASK) == 0 && commodityCount[STUFF_GOODS] > 0)
            {
                --commodityCount[STUFF_GOODS];
                ++commodityCount[STUFF_WASTE];
            }
        break;
        case GROUP_RAIL:
        case GROUP_RAIL_BRIDGE:
            transport_cost += 3;
            if (total_time % DAYS_PER_RAIL_POLLUTION == 0)
                world(x,y)->pollution += RAIL_POLLUTION;
            if ((total_time & RAIL_GOODS_USED_MASK) == 0 && commodityCount[STUFF_GOODS] > 0)
            {
                --commodityCount[STUFF_GOODS];
                ++commodityCount[STUFF_WASTE];
            }
            if ((total_time & RAIL_STEEL_USED_MASK) == 0 && commodityCount[STUFF_STEEL] > 0)
            {
                --commodityCount[STUFF_STEEL];
                ++commodityCount[STUFF_WASTE];
            }
        break;
    }
    if (commodityCount[STUFF_KWH] >= KWH_LOSS_ON_TRANSPORT)
    {
        commodityCount[STUFF_KWH] -= KWH_LOSS_ON_TRANSPORT;
    }
    else if (commodityCount[STUFF_KWH] > 0)
    {
        commodityCount[STUFF_KWH]--;
    }

    if (commodityCount[STUFF_WASTE] > 9 * constructionGroup->commodityRuleCount[STUFF_WASTE].maxload / 10)
    {
        anim = real_time + WASTE_BURN_TIME;
        commodityCount[STUFF_WASTE] -= WASTE_BURN_ON_TRANSPORT;
        world(x,y)->pollution += WASTE_BURN_ON_TRANSPORT_POLLUTE;
        if(!burning_waste)
        {
            burning_waste = true;
            Construction *fire = fireConstructionGroup.createConstruction(x, y);
            world(x,y)->construction = fire;
            //waste burning never spreads
            (dynamic_cast<Fire*>(fire))->flags |= FLAG_IS_GHOST;
            ::constructionCount.add_construction(fire);
        }
    }
    else if(burning_waste && (real_time > anim))
    {
        burning_waste = false;
        ::constructionCount.remove_construction(world(x,y)->construction);
#ifdef DEBUG
        assert(world(x,y)->construction->neighbors.empty());
#endif
        delete world(x,y)->construction;
        world(x,y)->construction = this;
    }
}

void Transport::list_traffic(int *i)
{
    std::map<Commodities, int>::iterator stuff_it;
    for(stuff_it = trafficCount.begin() ; stuff_it != trafficCount.end() ; stuff_it++)
    {
        if(*i < 14)
        {   mps_store_sfp((*i)++, commodityNames[stuff_it->first], (float) stuff_it->second * 100 * TRANSPORT_RATE / TRANSPORT_QUANTA);}
    }
}

void Transport::report()
{
    int i = 0;

    mps_store_sd(i++,constructionGroup->name,subgroupID);
    i++;
    if(mps_map_page == 1)
    {
        mps_store_title(i++,"Traffic");
        list_traffic(&i);
    }
    else
    {
        mps_store_title(i++,"Commodities");
        list_commodities(&i);
    }
}

void Transport::playSound()
{
    if(constructionGroup->sounds_loaded)
    {
        unsigned short g = constructionGroup->group;
        if ((g == GROUP_ROAD) || (g == GROUP_ROAD_BRIDGE))
        {
            int avg = 0;
            std::map<Commodities, int>::iterator stuff_it;
            for(stuff_it = trafficCount.begin() ; stuff_it != trafficCount.end() ; stuff_it++)
            {   avg += (stuff_it->second * 100 * TRANSPORT_RATE / TRANSPORT_QUANTA);}
            if(avg > 0) //equiv to size > 0
            {   avg /= trafficCount.size();}
            int num_sounds = constructionGroup->chunks.size()/2;
            if(avg > 5)
            {   getSound()->playASound(constructionGroup->chunks[rand()%num_sounds]);}
            else
            {   getSound()->playASound(constructionGroup->chunks[num_sounds+rand()%num_sounds]);}
        }
        else
        {
            int s = constructionGroup->chunks.size();
            getSound()->playASound(constructionGroup->chunks[rand()%s]);
        }
    }
}


/** @file lincity/modules/track_road_rail_powerline.cpp */

