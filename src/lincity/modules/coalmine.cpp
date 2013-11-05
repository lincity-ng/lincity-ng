/* ---------------------------------------------------------------------- *
 * coalmine.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */


#include "coalmine.h"

// Coalmine:
CoalmineConstructionGroup coalmineConstructionGroup(
    "Coal Mine",
     FALSE,                     /* need credit? */
     GROUP_COALMINE,
     GROUP_COALMINE_SIZE,
     GROUP_COALMINE_COLOUR,
     GROUP_COALMINE_COST_MUL,
     GROUP_COALMINE_BUL_COST,
     GROUP_COALMINE_FIREC,
     GROUP_COALMINE_COST,
     GROUP_COALMINE_TECH,
     GROUP_COALMINE_RANGE
);

CoalmineConstructionGroup coalmine_L_ConstructionGroup = coalmineConstructionGroup;
CoalmineConstructionGroup coalmine_M_ConstructionGroup = coalmineConstructionGroup;
CoalmineConstructionGroup coalmine_H_ConstructionGroup = coalmineConstructionGroup;


Construction *CoalmineConstructionGroup::createConstruction(int x, int y)
{
    return new Coalmine(x, y, this);
}

void Coalmine::update()
{
    bool coal_found = false;
    //scan available coal_reserve in range
    current_coal_reserve = 0;
    for (int yy = ys; yy < ye ; yy++)
    {
        for (int xx = xs; xx < xe ; xx++)
        {   current_coal_reserve += world(xx,yy)->coal_reserve;}
    }
    // mine some coal
    if ((current_coal_reserve > 0)
    && (commodityCount[STUFF_COAL] <= TARGET_COAL_LEVEL * (MAX_COAL_AT_MINE - COAL_PER_RESERVE)/100)
    && (commodityCount[STUFF_JOBS] >= COALMINE_JOBS))
    {
        for (int yy = ys; (yy < ye) && !coal_found; yy++)
        {
            for (int xx = xs; (xx < xe) && !coal_found; xx++)
            {
                if (world(xx,yy)->coal_reserve > 0)
                {
                    world(xx,yy)->coal_reserve--;
                    world(xx,yy)->pollution += COALMINE_POLLUTION;
                    world(xx,yy)->flags |= FLAG_ALTERED;
                    commodityCount[STUFF_COAL] += COAL_PER_RESERVE;
                    commodityCount[STUFF_JOBS] -= COALMINE_JOBS;
                    if (current_coal_reserve < initial_coal_reserve)
                    {   sust_dig_ore_coal_tip_flag = 0;}
                    coal_found = true;
                    working_days++;
                }
            }
        }
    }
    else if ((commodityCount[STUFF_COAL] - COAL_PER_RESERVE > TARGET_COAL_LEVEL * (MAX_COAL_AT_MINE)/100)
    && (commodityCount[STUFF_JOBS] >= COALMINE_JOBS))
    {
        for (int yy = ys; (yy < ye) && !coal_found; yy++)
        {
            for (int xx = xs; (xx < xe) && !coal_found; xx++)
            {
                if (world(xx,yy)->coal_reserve < COAL_RESERVE_SIZE)
                {
                    world(xx,yy)->coal_reserve++;
                    world(xx,yy)->flags |= FLAG_ALTERED;
                    commodityCount[STUFF_COAL] -= COAL_PER_RESERVE;
                    commodityCount[STUFF_JOBS] -= COALMINE_JOBS;
                    coal_found = true;
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
    //choose type depending on availabe coal
    if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_MINE - (MAX_COAL_AT_MINE / 4)))//75%
    {   constructionGroup = &coalmine_H_ConstructionGroup;}
    else if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_MINE / 2))//50%
    {   constructionGroup = &coalmine_M_ConstructionGroup;}
    else if (commodityCount[STUFF_COAL] > 0)//something
    {   constructionGroup = &coalmine_L_ConstructionGroup;}
    else//nothing
    {   constructionGroup = &coalmineConstructionGroup;}

    //Evacuate Mine if no more deposits
    if (current_coal_reserve == 0 )
    {   flags |= FLAG_EVACUATE;}

    //Abandon the Coalmine if it is really empty
    if ((current_coal_reserve == 0)
      &&(commodityCount[STUFF_JOBS] == 0)
      &&(commodityCount[STUFF_COAL] == 0) )
    {   ConstructionManager::submitRequest(new ConstructionDeletionRequest(this));}
}

void Coalmine::report()
{
    int i = 0;
    mps_store_sd(i++, constructionGroup->name, ID);
    mps_store_sfp(i++, "busy", busy);
    mps_store_sddp(i++, "Deposits", current_coal_reserve, initial_coal_reserve);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/coalmine.cpp */

