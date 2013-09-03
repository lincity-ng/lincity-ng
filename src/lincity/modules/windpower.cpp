/* ---------------------------------------------------------------------- *
 * windpower.cpp
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "windpower.h"

WindpowerConstructionGroup windpowerConstructionGroup(
    "Wind Power",
     TRUE,                     /* need credit? */
     GROUP_WIND_POWER,
     GROUP_WIND_POWER_SIZE,
     GROUP_WIND_POWER_COLOUR,
     GROUP_WIND_POWER_COST_MUL,
     GROUP_WIND_POWER_BUL_COST,
     GROUP_WIND_POWER_FIREC,
     GROUP_WIND_POWER_COST,
     GROUP_WIND_POWER_TECH,
     GROUP_WIND_POWER_RANGE
);

Construction *WindpowerConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new Windpower(x, y, type); //We are also building a windmill
}

void Windpower::update()
{

    if (!(total_time%(WIND_POWER_RCOST)))
    {   windmill_cost++;}
    if ((commodityCount[STUFF_JOBS] >= WIND_POWER_JOBS)
     && (commodityCount[STUFF_MWH] <= MAX_MWH_AT_WIND_POWER-mwh_output))
    {
        commodityCount[STUFF_JOBS] -= WIND_POWER_JOBS;
        commodityCount[STUFF_MWH] += mwh_output;
        animate = true;
        working_days++;
    }
    else
    {
        animate = false;
    }
    //monthly update
    if (total_time % 100 == 0)
    {
        busy = working_days;
        working_days = 0;
    }
    //Animation
    if (animate && (real_time > anim))
    {
        sail_count++;
        anim = real_time + WIND_POWER_ANIM_SPEED;
        sail_count %= 3;
        if (commodityCount[STUFF_MWH] > MAX_MWH_AT_WIND_POWER/2)
        {
            type = CST_WINDMILL_1_G + sail_count;
        }
        else if (commodityCount[STUFF_MWH] > MAX_MWH_AT_WIND_POWER/10)
        {
            type = CST_WINDMILL_1_RG + sail_count;
        }
        else
        {
            type = CST_WINDMILL_1_R + sail_count;
        }
    }
}


void Windpower::report()
{
    int i = 0;
    mps_store_sd(i++,constructionGroup->name,ID);
    mps_store_sfp(i++, _("busy"), busy);
    mps_store_sfp(i++, _("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, "Output", mwh_output);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/windpower.cpp */

