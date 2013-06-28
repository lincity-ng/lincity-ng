/* ---------------------------------------------------------------------- *
 * track_road_powerline.cpp
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "track_road_rail.h"

//FIXME would like to include this one so report could depend on overlay mode
//#include "lincity-ng/GameView.hpp"

// Track:
TransportConstructionGroup trackConstructionGroup(
    "Track",
    FALSE,                     /* need credit? */
    GROUP_TRACK,
    1,                         /* size */
    GROUP_TRACK_COLOUR,
    GROUP_TRACK_COST_MUL,
    GROUP_TRACK_BUL_COST,
    GROUP_TRACK_FIREC,
    GROUP_TRACK_COST,
    GROUP_TRACK_TECH
);

// TrackBridge:
TransportConstructionGroup trackbridgeConstructionGroup(
    "Track (Bridge)",
    FALSE,                     /* need credit? */
    GROUP_TRACK_BRIDGE,
    1,                         /* size */
    GROUP_TRACK_BRIDGE_COLOUR,
    GROUP_TRACK_BRIDGE_COST_MUL,
    GROUP_TRACK_BRIDGE_BUL_COST,
    GROUP_TRACK_BRIDGE_FIREC,
    GROUP_TRACK_BRIDGE_COST,
    GROUP_TRACK_BRIDGE_TECH
);

// Road:
TransportConstructionGroup roadConstructionGroup(
    "Road",
    FALSE,                     /* need credit? */
    GROUP_ROAD,
    1,                         /* size */
    GROUP_ROAD_COLOUR,
    GROUP_ROAD_COST_MUL,
    GROUP_ROAD_BUL_COST,
    GROUP_ROAD_FIREC,
    GROUP_ROAD_COST,
    GROUP_ROAD_TECH
);
// RoadBridge:
TransportConstructionGroup roadbridgeConstructionGroup(
    "Road (Bridge)",
    FALSE,                     /* need credit? */
    GROUP_ROAD_BRIDGE,
    1,                         /* size */
    GROUP_ROAD_BRIDGE_COLOUR,
    GROUP_ROAD_BRIDGE_COST_MUL,
    GROUP_ROAD_BRIDGE_BUL_COST,
    GROUP_ROAD_BRIDGE_FIREC,
    GROUP_ROAD_BRIDGE_COST,
    GROUP_ROAD_BRIDGE_TECH
);


// Rail:
TransportConstructionGroup railConstructionGroup(
    "Rail",
    FALSE,                     /* need credit? */
    GROUP_RAIL,
    1,                         /* size */
    GROUP_RAIL_COLOUR,
    GROUP_RAIL_COST_MUL,
    GROUP_RAIL_BUL_COST,
    GROUP_RAIL_FIREC,
    GROUP_RAIL_COST,
    GROUP_RAIL_TECH
);
// RailBridge:
TransportConstructionGroup railbridgeConstructionGroup(
    "Rail (Bridge)",
    FALSE,                     /* need credit? */
    GROUP_RAIL_BRIDGE,
    1,                         /* size */
    GROUP_RAIL_BRIDGE_COLOUR,
    GROUP_RAIL_BRIDGE_COST_MUL,
    GROUP_RAIL_BRIDGE_BUL_COST,
    GROUP_RAIL_BRIDGE_FIREC,
    GROUP_RAIL_BRIDGE_COST,
    GROUP_RAIL_BRIDGE_TECH
);


Construction *TransportConstructionGroup::createConstruction(int x, int y, unsigned short type)
{
    return new Transport(x, y, type);
}

void Transport::stuff_flow()
{
    /* Nov 2012:
    stuff_flow may only be used by new constructions. It checks for adjacent neigbohrs via collect_transport_info 
    and equlibrates all commodities from Construction::commodityCount with eqilibrate_transport_stuff    
     */

    /*   O---------------------->x
     *   |      | Up     |
     *   |  Left| Center |Right
     *   |      | Down   |
     *   v
     *   y
     */
    int ratio, center_lvl, center_cap;
    int traffic, max_traffic;
    int left_lvl, right_lvl, up_lvl, down_lvl, n;
    Commodities stuff_ID;
    std::map<Commodities, int>::iterator stuff_it;

    /*begin for over all different stuff*/
    for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++ )
	{
        stuff_ID = stuff_it->first;        
        center_lvl = stuff_it->second;
        center_cap = constructionGroup->commodityRuleCount[stuff_ID].maxload;
        //TODO maybe use a loop and rotation matrix here?         
        /*see how much stuff is there around*/
        /*ignore markets, they distribute stuff only actively themselfes*/
        ratio = (center_lvl * TRANSPORT_QUANTA / (center_cap) );        
        left_lvl = (world(x-1, y)->getGroup() == GROUP_MARKET)?-1:collect_transport_info(x-1 ,y , stuff_ID, ratio );//left
        right_lvl = (world(x+1, y)->getGroup() == GROUP_MARKET)?-1:collect_transport_info(x+1 ,y , stuff_ID, ratio );//right
        up_lvl = (world(x, y-1)->getGroup() == GROUP_MARKET)?-1:collect_transport_info(x ,y-1 , stuff_ID, ratio );//up
        down_lvl = (world(x, y+1)->getGroup() == GROUP_MARKET)?-1:collect_transport_info(x ,y+1 , stuff_ID, ratio );//down
        
        //calculate the not weighted average filling
        n = 1;        
        if (left_lvl != -1)
        {             
            ratio += left_lvl;            
            n++;
        }        
        if (right_lvl != -1)        
        {
            ratio += right_lvl;            
            n++;
        }        
        if (up_lvl != -1)
        {
            ratio += up_lvl;            
            n++;
        }        
        if (down_lvl != -1)
        {
            ratio += down_lvl;            
            n++;
        }
        
        ratio /= n;
        max_traffic = 0;
        //make flow towards ratio       
        if (left_lvl != -1)
        { 
            traffic = (equilibrate_transport_stuff(x-1, y, &center_lvl, center_cap, ratio, stuff_ID));//left
            if( traffic > max_traffic)
            {
                max_traffic = traffic;
            }
        }        
        if (right_lvl != -1)        
        {
            traffic = (equilibrate_transport_stuff(x+1, y, &center_lvl, center_cap, ratio, stuff_ID));//right
            if( traffic > max_traffic)
            {
                max_traffic = traffic;
            }
        }        
        if (up_lvl != -1)
        {
            traffic = (equilibrate_transport_stuff(x, y-1, &center_lvl, center_cap, ratio, stuff_ID));//up
            if( traffic > max_traffic)
            {
                max_traffic = traffic;
            }
        }
        if (down_lvl != -1)
        {
            traffic = (equilibrate_transport_stuff(x, y+1, &center_lvl, center_cap, ratio, stuff_ID));//down
            if( traffic > max_traffic)
            {
                max_traffic = traffic;
            }
        }
        //do some smoothing to suppress fluctuations from random order
        // max possible ~90 % 
        trafficCount[stuff_ID] = (9 * trafficCount[stuff_ID] + max_traffic) / 10;
/*        
        if (center_lvl < 0)
            std::cout<<"center load < 0 error at "<<world(x,y)->reportingConstruction->constructionGroup->name<<" x,y = "<<x<<","<<y<<std::endl;
*/
        stuff_it->second = center_lvl; //update center_lvl
       
        /*handle waste spill*/
        if (stuff_ID == STUFF_WASTE)
        {
            if (center_lvl > 9 * center_cap / 10 && !burning_waste)
            {
                old_type = type;        
                type = CST_FIRE_1;
                anim = real_time + WASTE_BURN_TIME;
                burning_waste = true;
                commodityCount[STUFF_WASTE] -= WASTE_BURN_ON_TRANSPORT;
                world(x,y)->pollution+= WASTE_BURN_ON_TRANSPORT_POLLUTE;                                 
            }
            else if (burning_waste && real_time > anim)
            {
                type = old_type;
                burning_waste = false;
            }
        }        
        
    } //endfor all different STUFF   
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
    stuff_flow();
    if (commodityCount[STUFF_KWH] >= KWH_LOSS_ON_TRANSPORT)
    {
        commodityCount[STUFF_KWH] -= KWH_LOSS_ON_TRANSPORT;    
    }
    else if (commodityCount[STUFF_KWH] > 0)
    {
        commodityCount[STUFF_KWH]--;   
    }
}

//FIXME quick function to test display traffic values
void Transport::list_traffic(int *i)
{
    std::map<Commodities, int>::iterator stuff_it;
    for(stuff_it = trafficCount.begin() ; stuff_it != trafficCount.end() ; stuff_it++)
    {
        if(*i < 14)
        {
            mps_store_sfp((*i)++, commodityNames[stuff_it->first], (float) stuff_it->second * 100 * TRANSPORT_RATE / TRANSPORT_QUANTA);
        }
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

/** @file lincity/modules/track_road_rail_powerline.cpp */

