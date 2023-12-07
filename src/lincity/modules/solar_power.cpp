/* ---------------------------------------------------------------------- *
 * solar_power.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */


#include "solar_power.h"


// SolarPower:
SolarPowerConstructionGroup solarPowerConstructionGroup(
    N_("Solar Power Plant"),
     TRUE,                     /* need credit? */
     GROUP_SOLAR_POWER,
     GROUP_SOLAR_POWER_SIZE,
     GROUP_SOLAR_POWER_COLOUR,
     GROUP_SOLAR_POWER_COST_MUL,
     GROUP_SOLAR_POWER_BUL_COST,
     GROUP_SOLAR_POWER_FIREC,
     GROUP_SOLAR_POWER_COST,
     GROUP_SOLAR_POWER_TECH,
     GROUP_SOLAR_POWER_RANGE
);

Construction *SolarPowerConstructionGroup::createConstruction(int x, int y) {
    return new SolarPower(x, y, this);
}

void SolarPower::update()
{
    int mwh_made = (commodityCount[STUFF_MWH] + mwh_output <= MAX_MWH_AT_SOLARPS)?mwh_output:MAX_MWH_AT_SOLARPS-commodityCount[STUFF_MWH];
    int jobs_used = SOLAR_POWER_JOBS * mwh_made / mwh_output;

    if ((commodityCount[STUFF_JOBS] >= jobs_used)
     && (mwh_made >= POWERS_SOLAR_OUTPUT))
    {
        consumeStuff(STUFF_JOBS, jobs_used);
        produceStuff(STUFF_MWH, mwh_made);
        working_days += mwh_made;
    }
    if (total_time % 100 == 99) //monthly update
    {
        reset_prod_counters();
        busy = working_days / mwh_output;
        working_days = 0;
    }
}

void SolarPower::report()
{
    int i = 0;

    mps_store_sd(i++, constructionGroup->name, ID);
    i++;
    mps_store_sfp(i++, N_("busy"), (busy));
    mps_store_sfp(i++, N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, N_("Output"), mwh_output);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/solar_power.cpp */
