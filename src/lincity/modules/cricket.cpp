/* ---------------------------------------------------------------------- *
 * cricket.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */


#include "cricket.h"

// cricket place:
CricketConstructionGroup cricketConstructionGroup(
    "Basketball court",
     FALSE,                     /* need credit? */
     GROUP_CRICKET,
     GROUP_CRICKET_SIZE,
     GROUP_CRICKET_COLOUR,
     GROUP_CRICKET_COST_MUL,
     GROUP_CRICKET_BUL_COST,
     GROUP_CRICKET_FIREC,
     GROUP_CRICKET_COST,
     GROUP_CRICKET_TECH,
     GROUP_CRICKET_RANGE
);

Construction *CricketConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new Cricket(x, y, type);
}

void Cricket::update()
{
    ++daycount;
    if (commodityCount[STUFF_JOBS] >= CRICKET_JOBS
    &&  commodityCount[STUFF_GOODS] >= CRICKET_GOODS
    &&  commodityCount[STUFF_WASTE] + (CRICKET_GOODS / 3) <= MAX_WASTE_AT_CRICKET)
    {
        commodityCount[STUFF_JOBS] -= CRICKET_JOBS;
        commodityCount[STUFF_GOODS] -= CRICKET_GOODS;
        commodityCount[STUFF_WASTE] += (CRICKET_GOODS / 3);
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
        anim = real_time + CRICKET_ANIMATION_SPEED;
        switch (type)
        {
            case (CST_CRICKET_1):
                type = CST_CRICKET_2;
                break;
            case (CST_CRICKET_2):
                type = CST_CRICKET_3;
                break;
            case (CST_CRICKET_3):
                type = CST_CRICKET_4;
                break;
            case (CST_CRICKET_4):
                type = CST_CRICKET_5;
                break;
            case (CST_CRICKET_5):
                type = CST_CRICKET_6;
                break;
            case (CST_CRICKET_6):
                type = CST_CRICKET_7;
                break;
            case (CST_CRICKET_7):
                type = CST_CRICKET_1;
                animate=false;    /* disable anim */
                break;
        }
    }
    /* That's all. Cover is done by different functions every 3 months or so. */
    cricket_cost += CRICKET_RUNNING_COST;
    if(refresh_cover)
    {   cover();}
}

void Cricket::cover()
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
        {   world(xx,yy)->flags |= FLAG_CRICKET_COVER;}
    }
}

void Cricket::report()
{
    int i = 0;
    const char* p;

    mps_store_sd(i++,constructionGroup->name,ID);
    mps_store_sfp(i++, "busy", (float) busy);
    i++;
    list_commodities(&i);
    p = active?"Yes":"No";
    mps_store_ss(i++, "Public sports", p);
}

/** @file lincity/modules/cricket.cpp */

