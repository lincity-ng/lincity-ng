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

WindpowerConstructionGroup windpower_RG_ConstructionGroup = windpowerConstructionGroup;
WindpowerConstructionGroup windpower_G_ConstructionGroup = windpowerConstructionGroup;

Construction *WindpowerConstructionGroup::createConstruction(int x, int y) {
    return new Windpower(x, y, this);
}

void Windpower::update()
{
    if (!(total_time%(WIND_POWER_RCOST)))
    {   windmill_cost++;}
    int mwh_made = (commodityCount[STUFF_MWH] + mwh_output <= MAX_MWH_AT_WIND_POWER)?mwh_output:MAX_MWH_AT_WIND_POWER-commodityCount[STUFF_MWH];
    int jobs_used = WIND_POWER_JOBS * mwh_made/mwh_output;

    if ((commodityCount[STUFF_JOBS] >= jobs_used)
     && mwh_made > WIND_POWER_MWH)
    {
        commodityCount[STUFF_JOBS] -= jobs_used;
        commodityCount[STUFF_MWH] += mwh_made;
        animate = true;
        working_days += mwh_made;
    }
    else
    {   animate = false;}
    //monthly update
    if (total_time % 100 == 0)
    {
        busy = working_days;
        working_days = 0;
    }
    //Animation
    if (animate && (real_time > anim))
    {
        ++type;
        type %= 3;
        anim = real_time + WIND_POWER_ANIM_SPEED;
        if (commodityCount[STUFF_MWH] > MAX_MWH_AT_WIND_POWER/2)
        {   constructionGroup = &windpower_G_ConstructionGroup;}
        else if (commodityCount[STUFF_MWH] > MAX_MWH_AT_WIND_POWER/10)
        {   constructionGroup = &windpower_RG_ConstructionGroup;}
        else
        {   constructionGroup = &windpowerConstructionGroup;}
    }
}


void Windpower::report()
{
    int i = 0;
    mps_store_sd(i++,constructionGroup->name,ID);
    mps_store_sfp(i++, _("busy"), float(busy) / mwh_output);
    mps_store_sfp(i++, _("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, "Output", mwh_output);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/windpower.cpp */

