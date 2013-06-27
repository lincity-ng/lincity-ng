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
     2,                         /* size */
     GROUP_CRICKET_COLOUR,
     GROUP_CRICKET_COST_MUL,
     GROUP_CRICKET_BUL_COST,
     GROUP_CRICKET_FIREC,
     GROUP_CRICKET_COST,
     GROUP_CRICKET_TECH
);

Construction *CricketConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new Cricket(x, y, type);
}

void Cricket::update()
{   
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
}

void Cricket::cover()
{
    int xx, x1, x2, y1, y2;
    if (commodityCount[STUFF_JOBS] < (CRICKET_JOBS * DAYS_BETWEEN_COVER)
    ||  commodityCount[STUFF_GOODS] < (CRICKET_GOODS * DAYS_BETWEEN_COVER)
    ||  commodityCount[STUFF_WASTE] + (CRICKET_GOODS * DAYS_BETWEEN_COVER / 3) > MAX_WASTE_AT_CRICKET)
    {    
        busy = false;        
        return;
    }    
    commodityCount[STUFF_JOBS] -= (CRICKET_JOBS * DAYS_BETWEEN_COVER);
    commodityCount[STUFF_GOODS] -= (CRICKET_GOODS * DAYS_BETWEEN_COVER);
    commodityCount[STUFF_WASTE] += (CRICKET_GOODS * DAYS_BETWEEN_COVER / 3);
    animate = true;    /* turn on animation */
    busy = true;
    int len = world.len();
    x1 = x - CRICKET_RANGE;
    if (x1 < 0)
        x1 = 0;
    x2 = x + CRICKET_RANGE;
    if (x2 > len)
        x2 = len;
    y1 = y - CRICKET_RANGE;
    if (y1 < 0)
        y1 = 0;
    y2 = y + CRICKET_RANGE;
    if (y2 > len)
        y2 = len;
    for (; y1 < y2; y1++)
    {
        for (xx = x1; xx < x2; xx++)
        {
            world(xx, y1)->flags |= FLAG_CRICKET_COVER;
        }
    }
}

void Cricket::report()
{
    int i = 0;
    const char* p;

    mps_store_sd(i++,constructionGroup->name,ID);
    i++;
    list_commodities(&i);
    p =  busy?"Yes":"No";
    mps_store_ss(i++, "Public sports", p);
}

/** @file lincity/modules/cricket.cpp */

