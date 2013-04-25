/* ---------------------------------------------------------------------- *
 * health_centre.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */


#include "health_centre.h"



// Health Centre:
HealthCentreConstructionGroup healthCentreConstructionGroup(
     "Health centre",
     FALSE,                     /* need credit? */
     GROUP_HEALTH,
     2,                         /* size */
     GROUP_HEALTH_COLOUR,
     GROUP_HEALTH_COST_MUL,
     GROUP_HEALTH_BUL_COST,
     GROUP_HEALTH_FIREC,
     GROUP_HEALTH_COST,
     GROUP_HEALTH_TECH
);

Construction *HealthCentreConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new HealthCentre(x, y, type);
}

void HealthCentre::update()
{
    /* That's all. Cover is done by different functions every 3 months or so. */
    health_cost += HEALTH_RUNNING_COST;
}

void HealthCentre::cover()
{
    int xx, x1, x2, y1, y2;
    if (commodityCount[STUFF_JOBS] < (HEALTH_CENTRE_JOBS * DAYS_BETWEEN_COVER)
    ||  commodityCount[STUFF_GOODS] < (HEALTH_CENTRE_GOODS * DAYS_BETWEEN_COVER)
    ||  commodityCount[STUFF_WASTE] + (HEALTH_CENTRE_GOODS * DAYS_BETWEEN_COVER / 3) > MAX_WASTE_AT_HEALTH_CENTRE)
    {
        busy = false;        
        return;
    }
    commodityCount[STUFF_JOBS] -= (HEALTH_CENTRE_JOBS * DAYS_BETWEEN_COVER);
    commodityCount[STUFF_GOODS] -= (HEALTH_CENTRE_GOODS * DAYS_BETWEEN_COVER);
    busy = true;    
    x1 = x - HEALTH_CENTRE_RANGE;
    if (x1 < 0)
        x1 = 0;
    x2 = x + HEALTH_CENTRE_RANGE;
    if (x2 > WORLD_SIDE_LEN)
        x2 = WORLD_SIDE_LEN;
    y1 = y - HEALTH_CENTRE_RANGE;
    if (y1 < 0)
        y1 = 0;
    y2 = y + HEALTH_CENTRE_RANGE;
    if (y2 > WORLD_SIDE_LEN)
        y2 = WORLD_SIDE_LEN;
    for (; y1 < y2; y1++)
    {
        for (xx = x1; xx < x2; xx++)
        {
            world(xx, y1)->flags |= FLAG_HEALTH_COVER;
        }
    }
}

void HealthCentre::report()
{
    int i = 0;
    const char* p;

    mps_store_sd(i++,constructionGroup->name,ID);
    i++;
    list_commodities(&i);
    p =  busy?"Yes":"No";    
    mps_store_ss(i++, "Health Care", p);
}

/** @file lincity/modules/health_centre.cpp */

