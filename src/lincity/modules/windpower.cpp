/* ---------------------------------------------------------------------- *
 * windpower.cpp
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "windpower.h"

WindpowerConstructionGroup windpowerConstructionGroup(
    N_("Wind Power"),
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

//WindpowerConstructionGroup windpower_RG_ConstructionGroup = windpowerConstructionGroup;
//WindpowerConstructionGroup windpower_G_ConstructionGroup = windpowerConstructionGroup;

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
        consumeStuff(STUFF_JOBS, jobs_used);
        produceStuff(STUFF_MWH, mwh_made);
        animate_enable = true;
        working_days += mwh_made;
    }
    else
    {   animate_enable = false;}
    //monthly update
    if (total_time % 100 == 99)
    {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }
}

void Windpower::animate() {
  if(animate_enable && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(WIND_POWER_ANIM_SPEED);
    ++frameIt->frame %= 3;
  }

  if (commodityCount[STUFF_MWH] > MAX_MWH_AT_WIND_POWER/2)
    frameIt->resourceGroup = ResourceGroup::resMap["WindMillHTechG"];
  else if (commodityCount[STUFF_MWH] > MAX_MWH_AT_WIND_POWER/10)
    frameIt->resourceGroup = ResourceGroup::resMap["WindMillHTechRG"];
  else
    frameIt->resourceGroup = ResourceGroup::resMap["WindMillHTech"];
  soundGroup = frameIt->resourceGroup;
}


void Windpower::report()
{
    int i = 0;
    mps_store_sd(i++, constructionGroup->name, ID);
    mps_store_sfp(i++, N_("busy"), float(busy) / mwh_output);
    mps_store_sfp(i++, N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, N_("Output"), mwh_output);
    // i++;
    list_commodities(&i);
}

/** @file lincity/modules/windpower.cpp */
