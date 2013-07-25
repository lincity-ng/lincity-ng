/* ---------------------------------------------------------------------- *
 * coal_power.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "coal_power.h"

Coal_powerConstructionGroup coal_powerConstructionGroup(
    "Coal Power Station",
     FALSE,                     /* need credit? */
     GROUP_COAL_POWER,
     4,                         /* size */
     GROUP_COAL_POWER_COLOUR,
     GROUP_COAL_POWER_COST_MUL,
     GROUP_COAL_POWER_BUL_COST,
     GROUP_COAL_POWER_FIREC,
     GROUP_COAL_POWER_COST,
     GROUP_COAL_POWER_TECH,
     GROUP_COAL_POWER_RANGE
);

Construction *Coal_powerConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new Coal_power(x, y, type);
}

void Coal_power::update()
{   
    if ((commodityCount[STUFF_JOBS] >= JOBS_COALPS_GENERATE)
     && (commodityCount[STUFF_COAL] >= POWERS_COAL_OUTPUT / 250)
     && (commodityCount[STUFF_MWH] <= MAX_MWH_AT_COALPS-mwh_output))
    {
        commodityCount[STUFF_JOBS] -= JOBS_COALPS_GENERATE;
        commodityCount[STUFF_COAL] -= (POWERS_COAL_OUTPUT / 250);       
        commodityCount[STUFF_MWH] += mwh_output;
        coal_used += (POWERS_COAL_OUTPUT / 250);
        world(x,y)->pollution += POWERS_COAL_POLLUTION;
        working_days++;
    }
    //monthly update
    if (total_time % 100 == 0)
    {
        busy = working_days;
        working_days = 0;
    }
    /* choose a graphic */
    if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_COALPS*4/5))
        type = CST_POWERS_COAL_FULL;
    else if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_COALPS / 2))
        type = CST_POWERS_COAL_MED;
    else if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_COALPS / 10))
        type = CST_POWERS_COAL_LOW;
    else
        type = CST_POWERS_COAL_EMPTY;
}

void Coal_power::report()
{
    int i = 0;    
    mps_store_sd(i++,constructionGroup->name,ID);
    mps_store_sfp(i++, _("busy"), (busy));    
    mps_store_sfp(i++, _("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, "Output", mwh_output);    
    i++;
    list_commodities(&i);
}


/** @file lincity/modules/coal_power.cpp */

