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
//#include "stats.h"              /* for transport_cost */
#include "all_buildings.h"
#include "engglobs.h"
#include "engine.h"
#include "modules/all_modules.h" //for knowing the Windmill in connect_transport
#include <iostream>

/*
void general_transport(int x, int y, int max_waste){}
*/
    /* 30. Oct 1996:
     * we'll use a loop with pointers here instead of doin' each
     * operation by hand.  this reduces code complexity and should
     * lead to a higher cache hit ratio - theoretically
     * (ThMO)
     *
     * 12. Dec 1996:
     * as this is a heavy used routine, another speedup improvement is
     * needed.  we'll now use 1 pointer, which will be incremented and
     * 1 .. 4 constant indices, which replaces the old pointer-pure
     * version.
     * advantages:  elimination of unnecessary pointer increments.
     * Note:  this *only* works, if the related addresses use one and the
     *        same address space - which is naturally for 2-dimensional
     *        arrays.
     * (ThMO)
     */

    /* Jan 2008: AL1
     * Due to current computer speed, no more need to worry about optimisation.
     * general_transport is less than 10% of the total simulation time AND
     * the simulation is very fast and is bounded by anim rate:
     * It could be about 5 times faster if we remove SDL_Delay in animation stuff
     *          tested on AMD Athlon XP 2200+ (1600 Mhz) + 750 MB
     *          with decent graphic card (GeForce 420 MX with 16MB)
     */

    /*   O---------------------->x
     *   |      | Up     |
     *   |  Left| Center |Right
     *   |      | Down   |
     *   v
     *   y
     */

int collect_transport_info(int x, int y, Construction::Commodities stuff_ID, int center_ratio)
{
    Construction * repcons = world(x, y)->reportingConstruction;
         
    if (repcons && repcons->commodityCount.count(stuff_ID))           
    {                    
        int loc_lvl = repcons->commodityCount[stuff_ID];
        int loc_cap = repcons->constructionGroup->commodityRuleCount[stuff_ID].maxload;
       
        if (repcons->flags & FLAG_EVACUATE)
        {	
			return loc_lvl?TRANSPORT_QUANTA:-1;
		}
                      
#ifdef DEBUG        
        if (loc_lvl > loc_cap)
        {
            std::cout<<"fixed "<<commodityNames[stuff_ID]<<" > maxload at "<<repcons->constructionGroup->name<<" x,y = "<<x<<","<<y<<std::endl;
            repcons->commodityCount[stuff_ID] = loc_cap;            
            loc_lvl = loc_cap;
        }
        if (loc_lvl < 0)
        {
            std::cout<<"fixed "<<commodityNames[stuff_ID]<<" < 0 at "<<repcons->constructionGroup->name<<" x,y = "<<x<<","<<y<<std::endl;
            repcons->commodityCount[stuff_ID] = loc_cap;            
            loc_lvl = 0;
        }                
       
        if (loc_cap < 1)
        {
            std::cout<<"maxload "<<commodityNames[stuff_ID]<<" <= 0 error at "<<repcons->constructionGroup->name<<" x,y = "<<x<<","<<y<<std::endl;
        }
#endif      
        int loc_ratio = loc_lvl * TRANSPORT_QUANTA / (loc_cap);       
        if ((center_ratio == -1) || (
        loc_ratio>center_ratio?repcons->constructionGroup->commodityRuleCount[stuff_ID].give:
			repcons->constructionGroup->commodityRuleCount[stuff_ID].take) )
        {   //only tell actual stock if we would tentaively participate in transport
            return (loc_ratio);
        }    
    }       
    return -1;
}

int equilibrate_transport_stuff(int x, int y, int *rem_lvl, int rem_cap ,int ratio, Construction::Commodities stuff_ID)
{
    Construction * repcons = world(x, y)->reportingConstruction;
    int flow, traffic;
    int *loc_lvl;
    int loc_cap;
    int transport_rate = TRANSPORT_RATE;

/*
	This will happen if mines are evacuated
    if (ratio > TRANSPORT_QUANTA)
        std::cout<<"target ratio > TRANSPORT_QUANTA at "<<world(x, y)->reportingConstruction->constructionGroup->name<<" x,y = "<<x<<","<<y<<std::endl;
*/    
    //Double speed transport with passive partners       
    if (!(repcons->flags & FLAG_IS_TRANSPORT))
    {
        transport_rate = TRANSPORT_RATE/2;
        if(transport_rate == 0)
        {
            transport_rate = 1;
        }
    }
    if (repcons && repcons->commodityCount.count(stuff_ID) ) // someone who cares about stuff_id 
    {                   
        loc_lvl = &(repcons->commodityCount[stuff_ID]);
		loc_cap = repcons->constructionGroup->commodityRuleCount[stuff_ID].maxload;
        if (!(repcons->flags & FLAG_EVACUATE))
        {			                   
			flow = (ratio * (loc_cap) / TRANSPORT_QUANTA) - (*loc_lvl);
			if (((flow > 0) && (!(repcons->constructionGroup->commodityRuleCount[stuff_ID].take) ))
			|| ((flow < 0) && !(repcons->constructionGroup->commodityRuleCount[stuff_ID].give)))
			{   //construction refuses the flow
				//std::cout << "."; //happens still often               
				return 0;
			}                
			if (flow > 0)
			{              
				if (flow * transport_rate > rem_cap )
					flow = rem_cap / transport_rate;
				if (flow > *rem_lvl)
					flow = *rem_lvl;
			}
			else if (flow < 0)
			{
				if(-flow * transport_rate > rem_cap)
					flow = - rem_cap / transport_rate;
				if (-flow > (rem_cap-*rem_lvl))
					flow = -(rem_cap-*rem_lvl);
			}
			else if ( !((repcons->flags & FLAG_IS_TRANSPORT || repcons->flags & FLAG_EVACUATE) 
					|| (repcons->constructionGroup->group == GROUP_MARKET)) )
			// transport tiles and markets tolerate insignifiact flow
			{
				//constructions doublecheck if the can get/put a least one item if flow would be nominally insiginficant
				if ( (*loc_lvl < *rem_lvl) && (*loc_lvl < loc_cap) ) // feed but dont overfeeding
					flow = 1; 
				else if (*loc_lvl > *rem_lvl && (*rem_lvl < rem_cap) ) // spill but dont flood 
					flow = -1;
			}       
			// limit local demand to remote quantity of stuff
			if (flow > *rem_lvl)
			{
				flow = *rem_lvl;
			}
			//limit remote demand to local quantity of stuff
			if (flow < -*loc_lvl)
			{
				flow = -*loc_lvl;
			}
			if (!(repcons->flags & FLAG_IS_TRANSPORT) && (flow > 0) 
				&& repcons->constructionGroup->group != GROUP_MARKET) 
			//something is given to a consumer 
			{
				switch (stuff_ID)
				{
					case (Construction::STUFF_JOBS) :
						income_tax += flow;
						break;
					case (Construction::STUFF_GOODS) :
						goods_tax += flow;
						goods_used += flow;
					case (Construction::STUFF_COAL) :
						coal_tax += flow;
						break;
					default:
						break;
				}              
			}             
		}
		else // we are evacuating
		{
			flow = -(rem_cap-*rem_lvl);
			if (-flow > *loc_lvl)
			{	flow = -*loc_lvl;}
		}
        traffic = flow * TRANSPORT_QUANTA / rem_cap;
        // incomming and outgoing traffic dont cancel but add up        
        if (traffic < 0)
        {
            traffic = -traffic;
        }      
        *loc_lvl += flow; 
        *rem_lvl -= flow;
        if (*loc_lvl < 0)
            std::cout<<"remote load < 0 error at "<<world(x, y)->reportingConstruction->constructionGroup->name<<" x,y = "<<x<<","<<y<<std::endl;
        
        if ((repcons->flags & FLAG_IS_TRANSPORT) || (repcons->flags & FLAG_POWER_LINE))
        {
            return traffic; //handled transport neighbor or powerline
        }
        else
        {
            return 0; //loading and unloading is not considered flowing traffic
        }    
    }
    return -1; //there was nothing to handle
}

void connect_transport(int originx, int originy, int w, int h)
{
    // sets the correct TYPE depending on neighbours, => gives the correct tile to display
    int x, y; 
    int mask, tflags, mask0;
    short group;

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

#if	FLAG_LEFT != 1 || FLAG_UP != 2 || FLAG_RIGHT != 4 || FLAG_DOWN != 8
#error  connect_transport(): you loose
#error  the algorithm depends on proper flag settings -- (ThMO)
#endif

    /* Adjust originx,originy,w,h to proper range */
    if (originx <= 0) {
        originx = 1;
        w -= 1 - originx;
    }
    if (originy <= 0) {
        originy = 1;
        h -= 1 - originy;
    }
    if (originx + w >= world.len()) {
        w = world.len() - originx;
    }
    if (originy + h >= world.len()) {
        h = world.len() - originy;
    }
    mask0 = ~(FLAG_LEFT |  FLAG_UP | FLAG_RIGHT | FLAG_DOWN );
    for (x = originx; x < originx + w; x++)
    {
        for (y = originy; y < originy + h; y++)
        {
            // First, set up a mask according to directions         
            mask = 0;          
            switch (world(x, y)->getGroup())
            {
            case GROUP_POWER_LINE:
                /* power may be transferred */
                /* up -- (ThMO) */
                group = check_group(x, y - 1);
                /* see if dug under track, rail or road */
                if ((y > 1) && (world(x, y-1)->is_water() || world(x, y-1)->is_transport()))
                    group = check_group(x, y - 2);
                switch (group)
                {
				/*
                    case GROUP_WINDMILL:
                        if ( !(static_cast<Windmill *>(world(x, y)->construction)->is_modern) ) // not a hightech WINDMILL
                            break;
				*/ 
                    case GROUP_WIND_POWER:        
                    case GROUP_POWER_LINE:
                    case GROUP_SOLAR_POWER:
                    case GROUP_SUBSTATION:
                    case GROUP_COAL_POWER:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                        mask |= 8;
                        break;
                }

                /* left -- (ThMO) */
                group = check_group(x - 1, y);
                if (x > 1 && (world(x-1, y)->is_water() || world(x-1, y)->is_transport()))
                    group = check_group(x - 2, y);
                switch (group)
                {
                /*
                    case GROUP_WINDMILL:
                        if ( !(static_cast<Windmill *>(world(x, y)->construction)->is_modern) ) // not a hightech WINDMILL
                            break;
				*/ 
                    case GROUP_WIND_POWER:
                    case GROUP_POWER_LINE:
                    case GROUP_SOLAR_POWER:
                    case GROUP_SUBSTATION:
                    case GROUP_COAL_POWER:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                        mask |= 4;
                        break;
                }

                /* right -- (ThMO) */
                group = check_group(x + 1, y);
                if (x < world.len() - 2 && (world(x+1, y)->is_water() || world(x+1, y)->is_transport()))
                    group = check_group(x + 2, y);
                switch (group)
                {
				/*
                    case GROUP_WINDMILL:
                        if ( !(static_cast<Windmill *>(world(x, y)->construction)->is_modern) ) // not a hightech WINDMILL
                            break;
				*/ 
                    case GROUP_WIND_POWER:
                    case GROUP_POWER_LINE:
                    case GROUP_SOLAR_POWER:
                    case GROUP_SUBSTATION:
                    case GROUP_COAL_POWER:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                        mask |= 2;
                        break;
                }

                /* down -- (ThMO) */
                group = check_group(x, y + 1);
                if (y < world.len() - 2 && (world(x, y+1)->is_water() || world(x, y+1)->is_transport())) 
                    group = check_group(x, y + 2);
                switch (group)
                {
				/*
                    case GROUP_WINDMILL:
                        if ( !(static_cast<Windmill *>(world(x, y)->construction)->is_modern) ) // not a hightech WINDMILL
                            break;
				*/ 
                    case GROUP_WIND_POWER:
                    case GROUP_POWER_LINE:
                    case GROUP_SOLAR_POWER:
                    case GROUP_SUBSTATION:
                    case GROUP_COAL_POWER:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                        ++mask;
                        break;
                }

                /* Next, set the connectivity into MP_TYPE */
                world(x, y)->construction->type = power_table[mask];
                world(x, y)->construction->flags &= mask0; // clear connection flags
                world(x, y)->construction->flags |= mask; // set connection flags
                break;

            case GROUP_TRACK:
                if (check_group(x, y - 1) == GROUP_TRACK 
                ||  check_group(x, y - 1) == GROUP_ROAD)
                    mask |= FLAG_UP;
                if (check_group(x - 1, y) == GROUP_TRACK
                ||  check_group(x - 1, y) == GROUP_ROAD)
                    mask |= FLAG_LEFT;
                tflags = mask;

                switch (check_topgroup(x + 1, y))
                {
                    case GROUP_ROAD:                   
                    case GROUP_TRACK:
                    case GROUP_TRACK_BRIDGE:
                        tflags |= FLAG_RIGHT;
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
                        tflags |= FLAG_DOWN;
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
                world(x, y)->construction->flags |= tflags;
//                MP_INFO(x, y).flags &= ~(FLAG_UP | FLAG_DOWN | FLAG_LEFT | FLAG_RIGHT);
//                MP_INFO(x, y).flags |= tflags;

                // A track section between 2 bridge sections
                // in this special case we use a pillar bridge section with green
                if ((check_group(x, y-1) == GROUP_TRACK_BRIDGE && (
                        check_group(x, y+1) == GROUP_TRACK_BRIDGE || check_group(x, y+2) == GROUP_TRACK_BRIDGE))
                        || (check_group(x, y+1) == GROUP_TRACK_BRIDGE && (
                        check_group(x, y-1) == GROUP_TRACK_BRIDGE || check_group(x, y-2) == GROUP_TRACK_BRIDGE)))
                {
                     //MP_TYPE(x, y) = CST_TRACK_BRIDGE_UDP;
                     world(x, y)->construction->type = CST_TRACK_BRIDGE_UDP;
                }
                else if ((check_group(x-1, y) == GROUP_TRACK_BRIDGE && (
                        check_group(x+1, y) == GROUP_TRACK_BRIDGE || check_group(x+2, y) == GROUP_TRACK_BRIDGE))
                        || (check_group(x+1, y) == GROUP_TRACK_BRIDGE && (
                        check_group(x-1, y) == GROUP_TRACK_BRIDGE || check_group(x-2, y) == GROUP_TRACK_BRIDGE)))
                {
                    //MP_TYPE(x, y) = CST_TRACK_BRIDGE_LRP;
                    world(x, y)->construction->type = CST_TRACK_BRIDGE_LRP;
                }
                // Set according bridge entrance if any
                else if (check_group(x, y-1) == GROUP_TRACK_BRIDGE)
                {
                    //MP_TYPE(x, y) = CST_TRACK_BRIDGE_OUD;
                    world(x, y)->construction->type = CST_TRACK_BRIDGE_OUD;
                }
                else if (check_group(x-1, y) == GROUP_TRACK_BRIDGE)
                {
                    //MP_TYPE(x, y) = CST_TRACK_BRIDGE_OLR;
                    world(x, y)->construction->type = CST_TRACK_BRIDGE_OLR;
                }
                else if (check_group(x, y+1) == GROUP_TRACK_BRIDGE)
                {
                    //MP_TYPE(x, y) = CST_TRACK_BRIDGE_IUD;
                    world(x, y)->construction->type = CST_TRACK_BRIDGE_IUD;
                }
                else if (check_group(x+1, y) == GROUP_TRACK_BRIDGE)
                {
                    //MP_TYPE(x, y) = CST_TRACK_BRIDGE_ILR;
                    world(x, y)->construction->type = CST_TRACK_BRIDGE_ILR;
                }
                else
                {
                    //MP_TYPE(x, y) = track_table[mask];
                    world(x, y)->construction->type = track_table[mask];
                }
                (dynamic_cast<Transport*>(world(x, y)->construction))->old_type = world(x, y)->construction->type;
                break;
                

            case GROUP_TRACK_BRIDGE:
                // Bridge neighbour priority
                if (check_group(x, y-1) == GROUP_TRACK_BRIDGE || check_group(x, y+1) == GROUP_TRACK_BRIDGE
                   || check_group(x, y-1) == GROUP_TRACK || check_group(x, y+1) == GROUP_TRACK)
                {
                    mask |= FLAG_UP;
                    //MP_TYPE(x, y) = CST_TRACK_BRIDGE_UD;
                    world(x, y)->construction->type = CST_TRACK_BRIDGE_UD;
                }
                else if (check_group(x-1, y) == GROUP_TRACK_BRIDGE || check_group(x+1, y) == GROUP_TRACK_BRIDGE
                    || check_group(x-1, y) == GROUP_TRACK || check_group(x+1, y) == GROUP_TRACK)
                {
                    mask |= FLAG_LEFT;
                    //MP_TYPE(x, y) = CST_TRACK_BRIDGE_LR;
                    world(x, y)->construction->type = CST_TRACK_BRIDGE_LR;
                }
                else //a lonely bridge tile
                {
                    world(x, y)->construction->type = CST_TRACK_BRIDGE_LR;                    
                }
                world(x, y)->construction->flags &= mask0;
                world(x, y)->construction->flags |= mask;                
//                MP_INFO(x, y).flags &= ~(FLAG_UP | FLAG_DOWN | FLAG_LEFT | FLAG_RIGHT);
//                MP_INFO(x, y).flags |= mask;
                (dynamic_cast<Transport*>(world(x, y)->construction))->old_type = world(x, y)->construction->type;
                break;
                

            case GROUP_ROAD:
                if (check_group(x, y - 1) == GROUP_ROAD
                ||  check_group(x, y - 1) == GROUP_TRACK)
                    mask |= FLAG_UP;
                if (check_group(x - 1, y) == GROUP_ROAD
                ||  check_group(x - 1, y) == GROUP_TRACK)
                    mask |= FLAG_LEFT;
                tflags = mask;

                switch (check_topgroup(x + 1, y))
                {
                    case GROUP_TRACK:                    
                    case GROUP_ROAD:
                        tflags |= FLAG_RIGHT;
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
                        tflags |= FLAG_DOWN;
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
                world(x, y)->construction->flags |= tflags;                
//                MP_INFO(x, y).flags &= ~(FLAG_UP | FLAG_DOWN | FLAG_LEFT | FLAG_RIGHT);
//                MP_INFO(x, y).flags |= tflags;
                // A road section between 2 bridge sections
                // in this special case we use a pillar bridge section with green
                if ((check_group(x, y-1) == GROUP_ROAD_BRIDGE && (
                        check_group(x, y+1) == GROUP_ROAD_BRIDGE || check_group(x, y+2) == GROUP_ROAD_BRIDGE))
                        || (check_group(x, y+1) == GROUP_ROAD_BRIDGE && (
                        check_group(x, y-1) == GROUP_ROAD_BRIDGE || check_group(x, y-2) == GROUP_ROAD_BRIDGE)))
                {
                     //MP_TYPE(x, y) = CST_ROAD_BRIDGE_UDPG;
                     world(x, y)->construction->type = CST_ROAD_BRIDGE_UDPG;
                }
                else if ((check_group(x-1, y) == GROUP_ROAD_BRIDGE && (
                        check_group(x+1, y) == GROUP_ROAD_BRIDGE || check_group(x+2, y) == GROUP_ROAD_BRIDGE))
                        || (check_group(x+1, y) == GROUP_ROAD_BRIDGE && (
                        check_group(x-1, y) == GROUP_ROAD_BRIDGE || check_group(x-2, y) == GROUP_ROAD_BRIDGE)))
                {
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_LRPG;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_LRPG;
                }
                // Build bridge entrance2
                else if (check_group(x, y-1) == GROUP_ROAD_BRIDGE)
                {
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_O2UD;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_O2UD;
                }
                else if (check_group(x-1, y) == GROUP_ROAD_BRIDGE)
                {
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_O2LR;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_O2LR;
                }
                else if (check_group(x, y+1) == GROUP_ROAD_BRIDGE)
                {
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_I2UD;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_I2UD;
                }
                else if (check_group(x+1, y) == GROUP_ROAD_BRIDGE)
                {
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_I2LR;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_I2LR;
                }
                // Build bridge entrance1
                else if (check_group(x, y-2) == GROUP_ROAD_BRIDGE && check_group(x, y-1) == GROUP_ROAD)
                {
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_O1UD;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_O1UD;
                }
                else if (check_group(x-2, y) == GROUP_ROAD_BRIDGE && check_group(x-1, y) == GROUP_ROAD)
                {
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_O1LR;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_O1LR;
                }
                else if (check_group(x, y+2) == GROUP_ROAD_BRIDGE && check_group(x, y+1) == GROUP_ROAD)
                {
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_I1UD;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_I1UD;
                }
                else if (check_group(x+2, y) == GROUP_ROAD_BRIDGE && check_group(x+1, y) == GROUP_ROAD)
                {
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_I1LR;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_I1LR;
                }
                else
                {
                    //MP_TYPE(x, y) = road_table[mask];
                    world(x, y)->construction->type = road_table[mask];
                }           
                (dynamic_cast<Transport*>(world(x, y)->construction))->old_type = world(x, y)->construction->type;
                break;

            case GROUP_ROAD_BRIDGE:
                // Bridge neighbour priority
                if (check_group(x, y-1) == GROUP_ROAD_BRIDGE || check_group(x, y+1) == GROUP_ROAD_BRIDGE)
                {
                    mask |= FLAG_UP;
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_UDP;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_UDP;
                }
                else if (check_group(x-1, y) == GROUP_ROAD_BRIDGE || check_group(x+1, y) == GROUP_ROAD_BRIDGE)
                {
                    mask |= FLAG_LEFT;
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_LRP;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_LRP;
                }
                else if (check_group(x, y-1) == GROUP_ROAD || check_group(x, y+1) == GROUP_ROAD)
                {
                    mask |= FLAG_UP;
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_UD;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_UD;
                }
                else if (check_group(x-1, y) == GROUP_ROAD || check_group(x+1, y) == GROUP_ROAD)
                {
                    mask |= FLAG_LEFT;
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_LR;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_LR;
                }
                else
                {
                    //MP_TYPE(x, y) = CST_ROAD_BRIDGE_LRP;
                    world(x, y)->construction->type = CST_ROAD_BRIDGE_LRP;
                }
                world(x, y)->construction->flags &= mask0;
                world(x, y)->construction->flags |= mask;                /////////////////////////////////////////////////////////////////should it be tflags here?
//                MP_INFO(x, y).flags &= ~(FLAG_UP | FLAG_DOWN | FLAG_LEFT | FLAG_RIGHT);
//                MP_INFO(x, y).flags |= mask;
                (dynamic_cast<Transport*>(world(x, y)->construction))->old_type = world(x, y)->construction->type;
                break;

            case GROUP_RAIL:
                if (check_group(x, y - 1) == GROUP_RAIL)
                    mask |= FLAG_UP;
                if (check_group(x - 1, y) == GROUP_RAIL)
                    mask |= FLAG_LEFT;
                tflags = mask;

                switch (check_topgroup(x + 1, y)) {
                    case GROUP_RAIL:
                        tflags |= FLAG_RIGHT;
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
                        tflags |= FLAG_DOWN;
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
                world(x, y)->construction->flags |= tflags;                
//                MP_INFO(x, y).flags &= ~(FLAG_UP | FLAG_DOWN | FLAG_LEFT | FLAG_RIGHT);
//                MP_INFO(x, y).flags |= tflags;
                // A rail section between 2 bridge sections
                // in this special case we use a pillar bridge section with green
                if ((check_group(x, y-1) == GROUP_RAIL_BRIDGE && (
                        check_group(x, y+1) == GROUP_RAIL_BRIDGE || check_group(x, y+2) == GROUP_RAIL_BRIDGE))
                        || (check_group(x, y+1) == GROUP_RAIL_BRIDGE && (
                        check_group(x, y-1) == GROUP_RAIL_BRIDGE || check_group(x, y-2) == GROUP_RAIL_BRIDGE)))
                {
                     //MP_TYPE(x, y) = CST_RAIL_BRIDGE_UDPG;
                     world(x, y)->construction->type = CST_RAIL_BRIDGE_UDPG;
                }
                else if ((check_group(x-1, y) == GROUP_RAIL_BRIDGE && (
                        check_group(x+1, y) == GROUP_RAIL_BRIDGE || check_group(x+2, y) == GROUP_RAIL_BRIDGE))
                        || (check_group(x+1, y) == GROUP_RAIL_BRIDGE && (
                        check_group(x-1, y) == GROUP_RAIL_BRIDGE || check_group(x-2, y) == GROUP_RAIL_BRIDGE)))
                {
                    //MP_TYPE(x, y) = CST_RAIL_BRIDGE_LRPG;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_LRPG;
                }
                // Build bridge entrance2
                else if (check_group(x, y-1) == GROUP_RAIL_BRIDGE)
                {
                    //MP_TYPE(x, y) = CST_RAIL_BRIDGE_O2UD;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_O2UD;
                }
                else if (check_group(x-1, y) == GROUP_RAIL_BRIDGE)
                {
                    //MP_TYPE(x, y) = CST_RAIL_BRIDGE_O2LR;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_O2LR;
                }
                else if (check_group(x, y+1) == GROUP_RAIL_BRIDGE)
                {
                    //MP_TYPE(x, y) = CST_RAIL_BRIDGE_I2UD;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_I2UD;
                }
                else if (check_group(x+1, y) == GROUP_RAIL_BRIDGE)
                {
                    //MP_TYPE(x, y) = CST_RAIL_BRIDGE_I2LR;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_I2LR;
                }
                // Build bridge entrance1
                else if (check_group(x, y-2) == GROUP_RAIL_BRIDGE && check_group(x, y-1) == GROUP_RAIL)
                {
                    //MP_TYPE(x, y) = CST_RAIL_BRIDGE_O1UD;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_O1UD;
                }
                else if (check_group(x-2, y) == GROUP_RAIL_BRIDGE && check_group(x-1, y) == GROUP_RAIL)
                {
                    //MP_TYPE(x, y) = CST_RAIL_BRIDGE_O1LR;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_O1LR;
                }
                else if (check_group(x, y+2) == GROUP_RAIL_BRIDGE && check_group(x, y+1) == GROUP_RAIL)
                {
                    //MP_TYPE(x, y) = CST_RAIL_BRIDGE_I1UD;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_I1UD;
                }
                else if (check_group(x+2, y) == GROUP_RAIL_BRIDGE && check_group(x+1, y) == GROUP_RAIL)
                {
                    //MP_TYPE(x, y) = CST_RAIL_BRIDGE_I1LR;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_I1LR;
                }
                else
                {
                    //MP_TYPE(x, y) = rail_table[mask];
                    world(x, y)->construction->type = rail_table[mask]; 
                }
                (dynamic_cast<Transport*>(world(x, y)->construction))->old_type = world(x, y)->construction->type;
                break;

            case GROUP_RAIL_BRIDGE:
                // Bridge neighbour priority
                if (check_group(x, y-1) == GROUP_RAIL_BRIDGE || check_group(x, y+1) == GROUP_RAIL_BRIDGE
                   || check_group(x, y-1) == GROUP_RAIL || check_group(x, y+1) == GROUP_RAIL)
                {
                    mask |= FLAG_UP;
                    //MP_TYPE(x, y) = CST_RAIL_BRIDGE_UD;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_UD;
                }
                else if (check_group(x-1, y) == GROUP_RAIL_BRIDGE || check_group(x+1, y) == GROUP_RAIL_BRIDGE
                    || check_group(x-1, y) == GROUP_RAIL || check_group(x+1, y) == GROUP_RAIL)
                {
                    mask |= FLAG_LEFT;
                    //MP_TYPE(x, y) = CST_RAIL_BRIDGE_LR;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_LR;
                }
                else
                {
                    //MP_TYPE(x, y) = CST_RAIL_BRIDGE_LR;
                    world(x, y)->construction->type = CST_RAIL_BRIDGE_LR;
                }
                world(x, y)->construction->flags &= mask0;
                world(x, y)->construction->flags |= mask;
//                MP_INFO(x, y).flags &= ~(FLAG_UP | FLAG_DOWN | FLAG_LEFT | FLAG_RIGHT);
//                MP_INFO(x, y).flags |= mask;
                (dynamic_cast<Transport*>(world(x, y)->construction))->old_type = world(x, y)->construction->type;
                break;

            case GROUP_WATER:
                /* up -- (ThMO) */
                if ( ((y > 0) && (world(x, y-1)->getGroup() == GROUP_PORT))
                    //(MP_TYPE(x, y - 1) == CST_USED && check_group(MP_INFO(x, y - 1).int_1, MP_INFO(x, y - 1).int_2) == GROUP_PORT)
                        || check_water(x, y - 1))
                    mask |= 8;

                /* left -- (ThMO) */
                if ( ((x > 0) && (world(x - 1, y)->getGroup() == GROUP_PORT))
                    //(MP_TYPE(x - 1,y) == CST_USED && check_group(MP_INFO(x - 1, y).int_1, MP_INFO(x - 1, y).int_2) == GROUP_PORT)
                        || check_water(x - 1, y))
                    mask |= 4;

                /* right -- (ThMO) */
                if (check_water(x + 1, y))
                    mask |= 2;

                /* down -- (ThMO) */
                if (check_water(x, y + 1))
                    mask |= 1;

                //MP_TYPE(x, y) = water_table[mask];
                world(x, y)->type = water_table[mask];
                break;
            }                   /* end switch */      
        }                       /* end for */
    }                           /* end for */   
}

/** @file lincity/transport.cpp */

