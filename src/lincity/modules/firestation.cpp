/* ---------------------------------------------------------------------- *
 * firestation.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */


#include "firestation.h"


// FireStation:
FireStationConstructionGroup fireStationConstructionGroup(
    "Fire station",
    FALSE,                     /* need credit? */
    GROUP_FIRESTATION,
    GROUP_FIRESTATION_SIZE,
    GROUP_FIRESTATION_COLOUR,
    GROUP_FIRESTATION_COST_MUL,
    GROUP_FIRESTATION_BUL_COST,
    GROUP_FIRESTATION_FIREC,
    GROUP_FIRESTATION_COST,
    GROUP_FIRESTATION_TECH,
    GROUP_FIRESTATION_RANGE
);

Construction *FireStationConstructionGroup::createConstruction(int x, int y ) {
    return new FireStation(x, y, this);
}

void FireStation::update()
{
    ++daycount;
    if (commodityCount[STUFF_JOBS] >= FIRESTATION_JOBS
    &&  commodityCount[STUFF_GOODS] >= FIRESTATION_GOODS
    &&  commodityCount[STUFF_WASTE] + (FIRESTATION_GOODS / 3) <= MAX_WASTE_AT_FIRESTATION)
    {
        commodityCount[STUFF_JOBS] -= FIRESTATION_JOBS;
        commodityCount[STUFF_GOODS] -= FIRESTATION_GOODS;
        commodityCount[STUFF_WASTE] += (FIRESTATION_GOODS / 3);
        ++covercount;
        ++working_days;
    }
    //monthly update
    if (total_time % 100 == 0)
    {
        busy = working_days;
        working_days = 0;
    }
    //animate
    if (animate && real_time > anim)
    {
        anim = real_time + FIRESTATION_ANIMATION_SPEED;
        ++type;
        if(type == 6)
        {   anim += 10*FIRESTATION_ANIMATION_SPEED;}
        if(type >= constructionGroup->graphicsInfoVector.size())
        {
            type = 0;
            animate = false;
        }
    }
    /* That's all. Cover is done by different functions every 3 months or so. */
    fire_cost += FIRESTATION_RUNNING_COST;
    if(refresh_cover)
    {   cover();}
}

void FireStation::cover()
{
    if(covercount + COVER_TOLERANCE_DAYS < daycount)
    {
        daycount = 0;
        active = false;
        return;
    }
    active = true;
    covercount -= daycount;
    daycount = 0;
    animate = true;
    for(int yy = ys; yy < ye; ++yy)
    {
        for(int xx = xs; xx < xe; ++xx)
        {   world(xx,yy)->flags |= FLAG_FIRE_COVER;}
    }
}

void FireStation::report()
{
    int i = 0;
    const char* p;
    mps_store_sd(i++,constructionGroup->name,ID);
    mps_store_sfp(i++, "busy", (float) busy);
    i++;
    list_commodities(&i);
    p = active?"Yes":"No";
    mps_store_ss(i++, "Fire Protection", p);
}

/** @file lincity/modules/firestation.cpp */

