/* ---------------------------------------------------------------------- *
 * solar_power.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */


#include "solar_power.h"

#include "modules.h"


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
    int hivolt_made = (commodityCount[STUFF_HIVOLT] + hivolt_output <= MAX_HIVOLT_AT_SOLARPS)?hivolt_output:MAX_HIVOLT_AT_SOLARPS-commodityCount[STUFF_HIVOLT];
    int labor_used = SOLAR_POWER_LABOR * hivolt_made / hivolt_output;

    if ((commodityCount[STUFF_LABOR] >= labor_used)
     && (hivolt_made >= POWERS_SOLAR_OUTPUT))
    {
        consumeStuff(STUFF_LABOR, labor_used);
        produceStuff(STUFF_HIVOLT, hivolt_made);
        working_days += hivolt_made;
    }
    if (total_time % 100 == 99) //monthly update
    {
        reset_prod_counters();
        busy = working_days / hivolt_output;
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
    mps_store_sd(i++, N_("Output"), hivolt_output);
    // i++;
    list_commodities(&i);
}

/** @file lincity/modules/solar_power.cpp */
