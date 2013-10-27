/* ---------------------------------------------------------------------- *
 * coal_power.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "coal_power.h"
#include "lincity-ng/Sound.hpp"

Coal_powerConstructionGroup coal_powerConstructionGroup(
    "Coal Power Station",
     FALSE,                     /* need credit? */
     GROUP_COAL_POWER,
     GROUP_COAL_POWER_SIZE,
     GROUP_COAL_POWER_COLOUR,
     GROUP_COAL_POWER_COST_MUL,
     GROUP_COAL_POWER_BUL_COST,
     GROUP_COAL_POWER_FIREC,
     GROUP_COAL_POWER_COST,
     GROUP_COAL_POWER_TECH,
     GROUP_COAL_POWER_RANGE
);

Construction *Coal_powerConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new Coal_power(x, y, type, this);
}

void Coal_power::update()
{
    int mwh_made = (commodityCount[STUFF_MWH] + mwh_output <= MAX_MWH_AT_COALPS)?mwh_output:MAX_MWH_AT_COALPS-commodityCount[STUFF_MWH];
    int jobs_used = JOBS_COALPS_GENERATE*(mwh_made/100)/(mwh_output/100);
    int coal_used = POWERS_COAL_OUTPUT / POWER_PER_COAL * (mwh_made/100) /(mwh_output/100);
    if ((commodityCount[STUFF_JOBS] >= jobs_used )
     && (commodityCount[STUFF_COAL] >= coal_used)
     && (mwh_made >= POWERS_COAL_OUTPUT))
    {
        commodityCount[STUFF_JOBS] -= jobs_used;
        commodityCount[STUFF_COAL] -= coal_used;
        commodityCount[STUFF_MWH] += mwh_made;
        world(x,y)->pollution += POWERS_COAL_POLLUTION *(mwh_made/100)/(mwh_output/100);
        working_days += (mwh_made/100);
    }
    //monthly update
    if (total_time % 100 == 0)
    {
        busy = working_days / (mwh_output/100);
        working_days = 0;
    }
    /* choose a graphic */
    if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_COALPS*4/5))
    {   type = CST_POWERS_COAL_FULL;}
    else if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_COALPS / 2))
    {   type = CST_POWERS_COAL_MED;}
    else if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_COALPS / 10))
    {   type = CST_POWERS_COAL_LOW;}
    else
    {   type = CST_POWERS_COAL_EMPTY;}
}

void Coal_power::report()
{
    int i = 0;
    mps_store_sd(i++,constructionGroup->name,ID);
    mps_store_sfp(i++, _("busy"), busy);
    mps_store_sfp(i++, _("Tech"), (float)(tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, "Output", mwh_output);
    i++;
    list_commodities(&i);
}

void Coal_power::playSound()
{
    switch(type)
    {
        case CST_POWERS_COAL_EMPTY:
            getSound()->playASound(constructionGroup->chunks[0]);
        break;
        case CST_POWERS_COAL_FULL:
            getSound()->playASound(constructionGroup->chunks[1]);
        break;
        case CST_POWERS_COAL_LOW:
            getSound()->playASound(constructionGroup->chunks[2]);
        break;
        case CST_POWERS_COAL_MED:
            getSound()->playASound(constructionGroup->chunks[3]);
        break;
    }
}


/** @file lincity/modules/coal_power.cpp */

