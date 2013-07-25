/* ---------------------------------------------------------------------- *
 * solar_power.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */


#include "solar_power.h"


// SolarPower:
SolarPowerConstructionGroup solarPowerConstructionGroup(
    "Solar Power Plant",
     TRUE,                     /* need credit? */
     GROUP_SOLAR_POWER,
     4,                         /* size */
     GROUP_SOLAR_POWER_COLOUR,
     GROUP_SOLAR_POWER_COST_MUL,
     GROUP_SOLAR_POWER_BUL_COST,
     GROUP_SOLAR_POWER_FIREC,
     GROUP_SOLAR_POWER_COST,
     GROUP_SOLAR_POWER_TECH,
     GROUP_SOLAR_POWER_RANGE
);

Construction *SolarPowerConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new SolarPower(x, y, type);
}

void SolarPower::update()
{
    if ((commodityCount[STUFF_JOBS] >= SOLAR_POWER_JOBS)
     && (commodityCount[STUFF_MWH] <= MAX_MWH_AT_SOLARPS-mwh_output))
    {
        commodityCount[STUFF_JOBS] -= SOLAR_POWER_JOBS;    
        commodityCount[STUFF_MWH] += mwh_output;
        working_days++;
    }     
    if (total_time % 100 == 0) //monthly update
    {
        busy = working_days;
        working_days = 0;
    }
}

void SolarPower::report()
{
    int i = 0;

    mps_store_sd(i++,constructionGroup->name,ID);
    i++;
    mps_store_sfp(i++, _("busy"), (busy));    
    mps_store_sfp(i++, _("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, "Output", mwh_output);  
    i++;
    list_commodities(&i);
}



/*** Solar Power ***/

/*
  int_1 is the rated output (depends on MP_TECH)
  int_2 unused
  int_3 unused
  int_4 unused
  int_5 is the current output
  int_6 is the grid it's on
  int_7 unused

  MP_TECH(x,y) is the tech level when it was built.
*/
/*
void do_power_source_solar(int x, int y)
{

    if (get_jobs(x, y, SOLAR_POWER_JOBS)) {
        MP_INFO(x, y).int_5 = MP_INFO(x, y).int_1;
        grid[MP_INFO(x, y).int_6]->avail_power += MP_INFO(x, y).int_1;
    } else {
        MP_INFO(x, y).int_5 = 0;
    }
}
*/
/*
void mps_solar_power(int x, int y)
{
    int i = 0;

    char s[12];

    mps_store_title(i++, _("Solar power station"));
    i++;
    mps_store_sfp(i++, _("Jobs"), (MP_INFO(x, y).int_5 * 100) / MP_INFO(x, y).int_1 );
    i++;

    format_power(s, sizeof(s), MP_INFO(x, y).int_1);
    mps_store_title(i++, _("Max Output"));
    mps_store_title(i++, s);
    i++;

    format_power(s, sizeof(s), MP_INFO(x, y).int_5);
    mps_store_title(i++, _("Current Output"));
    mps_store_title(i++, s);
    i++;

    mps_store_sfp(i++, _("Tech"), ( MP_TECH(x, y) * 100.0 / MAX_TECH_LEVEL ) );
    mps_store_sd(i++, _("Grid ID"), MP_INFO(x, y).int_6);
}
*/
/** @file lincity/modules/solar_power.cpp */

