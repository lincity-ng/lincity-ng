/* ---------------------------------------------------------------------- *
 * oremine.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */


#include "oremine.h"

//#include "../transport.h"



// Oremine:
OremineConstructionGroup oremineConstructionGroup(
    "Ore Mine",
     FALSE,                     /* need credit? */
     GROUP_OREMINE,
     4,                         /* size */
     GROUP_OREMINE_COLOUR,
     GROUP_OREMINE_COST_MUL,
     GROUP_OREMINE_BUL_COST,
     GROUP_OREMINE_FIREC,
     GROUP_OREMINE_COST,
     GROUP_OREMINE_TECH
);

Construction *OremineConstructionGroup::createConstruction(int x, int y, unsigned short type)
{
    return new Oremine(x, y, type);
}
/*
// Empty Oremine:
EmptyOremineConstructionGroup emptyOremineConstructionGroup(
    "Ore Mine (Empty)",
     FALSE,                     
     GROUP_OREMINE,
     4,                         
     GROUP_OREMINE_COLOUR,
     GROUP_OREMINE_COST_MUL,
     GROUP_OREMINE_BUL_COST,
     GROUP_OREMINE_FIREC,
     GROUP_OREMINE_COST,
     GROUP_OREMINE_TECH
);
//Dont register this one in group map
Construction *EmptyOremineConstructionGroup::createConstruction(int , int , unsigned short )
{
    assert(false);
    return NULL;
}
*/

void Oremine::update()
{   
    int xx,yy;
    animate = false;    
           
    if(commodityCount[STUFF_JOBS] >= JOBS_LOAD_ORE && commodityCount[STUFF_ORE] > 0)
    {    
        commodityCount[STUFF_JOBS] -= JOBS_LOAD_ORE;  
    }   
    // see if we can/need to extract some underground ore    
    if ((total_ore_reserve) 
    && (commodityCount[STUFF_ORE] <= ORE_LEVEL_TARGET * (MAX_ORE_AT_MINE - ORE_PER_RESERVE)/100)
    && (commodityCount[STUFF_JOBS] >= JOBS_DIG_ORE)) 
    {   
        for (yy = y; (yy < y + 4) && !animate; yy++)
        {
            for (xx = x; (xx < x +4) && !animate; xx++)
            {
                if (world(xx,yy)->ore_reserve > 0) 
                {
                    world(xx,yy)->ore_reserve--;
                    total_ore_reserve--;
                    commodityCount[STUFF_ORE] += ORE_PER_RESERVE;
                    commodityCount[STUFF_JOBS] -= JOBS_DIG_ORE;                        
                    //FIXME ore_tax should be handled upon delivery
                    //ore_made += ORE_PER_RESERVE;  
                    sust_dig_ore_coal_tip_flag = 0;
                    animate = true;
                    busy_days++;  
                }
            }
        }        
    }
    // return the ore to ore_reserve if there is enough sustainable ore available     
    else if ((commodityCount[STUFF_ORE] - ORE_PER_RESERVE > ORE_LEVEL_TARGET * (MAX_ORE_AT_MINE )/100)
    && (commodityCount[STUFF_JOBS] >= JOBS_DIG_ORE))
    {   
        for (yy = y; (yy < y + 4) && !animate; yy++)
        {
            for (xx = x; (xx < x +4) && !animate; xx++)
            {
                if (world(xx,yy)->ore_reserve < ORE_RESERVE) 
                {
                    world(xx,yy)->ore_reserve++;
                    total_ore_reserve++;
                    commodityCount[STUFF_ORE] -= ORE_PER_RESERVE;
                    commodityCount[STUFF_JOBS] -= JOBS_DIG_ORE;                        
                    sust_dig_ore_coal_tip_flag = 1;
                    animate = true;
                    busy_days++;  
                }
            }
        }        
    }
     
    //Monthly update of activity    
    if (total_time % 100 == 0)
    {
        busy = busy_days;
        busy_days = 0;
    }   
    // Anim according to ore mine activity
    if (animate && real_time > anim) 
    {
        if (real_time > days_offset) 
        {
            days_offset = real_time + (16 * OREMINE_ANIMATION_SPEED) + (rand() % (16 * OREMINE_ANIMATION_SPEED));
        }  
        //faster animation for more active mines
        anim = real_time + ((14 - busy/11) * OREMINE_ANIMATION_SPEED);
        anim_count = (anim_count + days_offset) & 15;
        switch (anim_count)
        {
            case (0):
                type = CST_OREMINE_1;
                break;
            case (1):
                type = CST_OREMINE_2;
                break;
            case (2):
                type = CST_OREMINE_3;
                break;
            case (3):
                type = CST_OREMINE_4;
                break;
            case (4):
                type = CST_OREMINE_5;
                break;
            case (5):
                type = CST_OREMINE_6;
                break;
            case (6):
                type = CST_OREMINE_7;
                break;
            case (7):
                type = CST_OREMINE_8;
                break;
            case (8):
                type = CST_OREMINE_7;
                break;
            case (9):
                type = CST_OREMINE_6;
                break;
            case (10):
                type = CST_OREMINE_5;
                break;
            case (11):
                type = CST_OREMINE_4;
                break;
            case (12):
                type = CST_OREMINE_5;
                break;
            case (13):
                type = CST_OREMINE_4;
                break;
            case (14):
                type = CST_OREMINE_3;
                break;
            case (15):
                type = CST_OREMINE_2;
                break;
        }//endswitch
    }//end if animate
    
    //Evacuate Mine if no more deposits
    if ( !(flags & FLAG_EVACUATE) && (total_ore_reserve < 1) )
    {
		flags |= FLAG_EVACUATE;
		//constructionGroup = &emptyOremineConstructionGroup;
	}
    
    //Abandon the Oremine if it is really empty  
    if ((total_ore_reserve < 1)
      &&(commodityCount[STUFF_JOBS] < 1)
      &&(commodityCount[STUFF_ORE] < 1) )
    {
        ConstructionManager::submitRequest
            (
                new OreMineDeletionRequest(this)
            );
    }

}

void Oremine::report()
{
    int i = 0;
    mps_store_sd(i++, constructionGroup->name, ID);   
    mps_store_sfp(i++,"busy",busy);
    mps_store_sddp(i++,"Deposits", total_ore_reserve, (16 * ORE_RESERVE));    
    i++;
    list_commodities(&i);    
}

/** @file lincity/modules/oremine.cpp */

