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
     GROUP_OREMINE_SIZE,
     GROUP_OREMINE_COLOUR,
     GROUP_OREMINE_COST_MUL,
     GROUP_OREMINE_BUL_COST,
     GROUP_OREMINE_FIREC,
     GROUP_OREMINE_COST,
     GROUP_OREMINE_TECH,
     GROUP_OREMINE_RANGE
);

Construction *OremineConstructionGroup::createConstruction(int x, int y)
{
    return new Oremine(x, y, this);
}

void Oremine::update()
{
    int xx,yy;
    animate = false;

    // see if we can/need to extract some underground ore
    if ((total_ore_reserve)
    && (commodityCount[STUFF_ORE] <= ORE_LEVEL_TARGET * (MAX_ORE_AT_MINE - ORE_PER_RESERVE)/100)
    && (commodityCount[STUFF_JOBS] >= OREMINE_JOBS))
    {
        for (yy = y; (yy < y + constructionGroup->size) && !animate; yy++)
        {
            for (xx = x; (xx < x + constructionGroup->size) && !animate; xx++)
            {
                if (world(xx,yy)->ore_reserve > 0)
                {
                    world(xx,yy)->ore_reserve--;
                    world(xx,yy)->flags |= FLAG_ALTERED;
                    total_ore_reserve--;
                    commodityCount[STUFF_ORE] += ORE_PER_RESERVE;
                    commodityCount[STUFF_JOBS] -= OREMINE_JOBS;
                    //FIXME ore_tax should be handled upon delivery
                    //ore_made += ORE_PER_RESERVE;
                    if (total_ore_reserve < (constructionGroup->size * constructionGroup->size * ORE_RESERVE))
                    {   sust_dig_ore_coal_tip_flag = 0;}
                    animate = true;
                    working_days++;
                }
            }
        }
    }
    // return the ore to ore_reserve if there is enough sustainable ore available
    else if ((commodityCount[STUFF_ORE] - ORE_PER_RESERVE > ORE_LEVEL_TARGET * (MAX_ORE_AT_MINE )/100)
    && (commodityCount[STUFF_JOBS] >= JOBS_DIG_ORE))
    {
        for (yy = y; (yy < y + constructionGroup->size) && !animate; yy++)
        {
            for (xx = x; (xx < x + constructionGroup->size) && !animate; xx++)
            {
                if (world(xx,yy)->ore_reserve < (3 * ORE_RESERVE/2))
                {
                    world(xx,yy)->ore_reserve++;
                    world(xx,yy)->flags |= FLAG_ALTERED;
                    total_ore_reserve++;
                    commodityCount[STUFF_ORE] -= ORE_PER_RESERVE;
                    commodityCount[STUFF_JOBS] -= OREMINE_JOBS;
                    animate = true;
                    working_days++;
                }
            }
        }
    }

    //Monthly update of activity
    if (total_time % 100 == 0)
    {
        busy = working_days;
        working_days = 0;
    }
    // Anim according to ore mine activity
    if (animate && real_time > anim)
    {
        if (real_time > days_offset)
        {   days_offset = real_time + (16 * OREMINE_ANIMATION_SPEED) + (rand() % (16 * OREMINE_ANIMATION_SPEED));}
        //faster animation for more active mines
        anim = real_time + ((14 - busy/11) * OREMINE_ANIMATION_SPEED);
        anim_count = (anim_count + days_offset) & 15;
        switch (anim_count)
        {
            case (0):
                type = 0;
                break;
            case (1):
                type = 1;
                break;
            case (2):
                type = 2;
                break;
            case (3):
                type = 3;
                break;
            case (4):
                type = 4;
                break;
            case (5):
                type = 5;
                break;
            case (6):
                type = 6;
                break;
            case (7):
                type = 7;
                break;
            case (8):
                type = 6;
                break;
            case (9):
                type = 5;
                break;
            case (10):
                type = 4;
                break;
            case (11):
                type = 3;
                break;
            case (12):
                type = 4;
                break;
            case (13):
                type = 3;
                break;
            case (14):
                type = 2;
                break;
            case (15):
                type = 1;
                break;
        }//endswitch
    }//end if animate

    //Evacuate Mine if no more deposits
    if ( total_ore_reserve == 0 )
    {   flags |= FLAG_EVACUATE;}

    //Abandon the Oremine if it is really empty
    if ((total_ore_reserve == 0)
      &&(commodityCount[STUFF_JOBS] == 0)
      &&(commodityCount[STUFF_ORE] == 0) )
    {   ConstructionManager::submitRequest(new OreMineDeletionRequest(this));}
}

void Oremine::report()
{
    int i = 0;
    mps_store_sd(i++, constructionGroup->name, ID);
    mps_store_sfp(i++,"busy",busy);
    mps_store_sddp(i++,"Deposits", total_ore_reserve, (constructionGroup->size * constructionGroup->size * ORE_RESERVE));
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/oremine.cpp */

