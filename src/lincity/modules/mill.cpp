/* ---------------------------------------------------------------------- *
 * mill.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "mill.h"


MillConstructionGroup millConstructionGroup(
    "Textile Mill",
    FALSE,                     /* need credit? */
    GROUP_MILL,
    GROUP_MILL_SIZE,
    GROUP_MILL_COLOUR,
    GROUP_MILL_COST_MUL,
    GROUP_MILL_BUL_COST,
    GROUP_MILL_FIREC,
    GROUP_MILL_COST,
    GROUP_MILL_TECH,
    GROUP_MILL_RANGE
);

Construction *MillConstructionGroup::createConstruction(int x, int y) {
    return new Mill(x, y, this);
}

void Mill::update()
{
    bool use_coal = (commodityCount[STUFF_COAL]*MAX_KWH_AT_MILL > commodityCount[STUFF_KWH]*MAX_COAL_AT_MILL);
    flags &= ~(FLAG_POWERED);
    if ((use_coal?commodityCount[STUFF_COAL]:commodityCount[STUFF_KWH]) >= (use_coal?COAL_USED_BY_MILL:COAL_USED_BY_MILL * MILL_POWER_PER_COAL)
    && (flags |= FLAG_POWERED, commodityCount[STUFF_FOOD] >= FOOD_USED_BY_MILL)
    && (commodityCount[STUFF_JOBS] >= MILL_JOBS)
    && (commodityCount[STUFF_GOODS] <= MAX_GOODS_AT_MILL - GOODS_MADE_BY_MILL))
    {
        (use_coal?commodityCount[STUFF_COAL]:commodityCount[STUFF_KWH]) -= (use_coal?COAL_USED_BY_MILL:COAL_USED_BY_MILL * MILL_POWER_PER_COAL);
        commodityCount[STUFF_FOOD] -= FOOD_USED_BY_MILL;
        commodityCount[STUFF_JOBS] -= MILL_JOBS;
        commodityCount[STUFF_GOODS] += GOODS_MADE_BY_MILL;
        ++working_days;
        animate = true;
        if ((++pol_count %= 7) == 0)
            world(x,y)->pollution++;
    } else
    {
        type = 0;
        animate = false;
    }
    //monthly update
    if (total_time % 100 == 0)
    {
        busy = working_days;
        working_days = 0;
    }
    //Animation
    if (real_time >= anim && animate)
    {
        anim = real_time + MILL_ANIM_SPEED;
        if(++type >= constructionGroup->graphicsInfoVector.size())
        {   type = 1;}
    }
}

void Mill::report()
{
    int i = 0;
    mps_store_sd(i++, constructionGroup->name,ID);
    mps_store_sfp(i++, _("busy"), (float) busy);
    i++;
    list_commodities(&i);
}


/** @file lincity/modules/mill.cpp */

