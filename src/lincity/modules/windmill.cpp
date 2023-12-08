/* ---------------------------------------------------------------------- *
 * windmill.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "windmill.h"


WindmillConstructionGroup windmillConstructionGroup(
    N_("Windmill"),
     FALSE,                     /* need credit? */
     GROUP_WINDMILL,
     GROUP_WINDMILL_SIZE,
     GROUP_WINDMILL_COLOUR,
     GROUP_WINDMILL_COST_MUL,
     GROUP_WINDMILL_BUL_COST,
     GROUP_WINDMILL_FIREC,
     GROUP_WINDMILL_COST,
     GROUP_WINDMILL_TECH,
     GROUP_WINDMILL_RANGE
);

Construction *WindmillConstructionGroup::createConstruction(int x, int y) {
    return new Windmill(x, y, this);
}

void Windmill::update()
{
    if (!(total_time%(WINDMILL_RCOST)))
    {   windmill_cost++;}
    int kwh_made = (commodityCount[STUFF_KWH] + kwh_output <= MAX_KWH_AT_WINDMILL)?kwh_output:MAX_KWH_AT_WINDMILL-commodityCount[STUFF_KWH];
    int jobs_used = WINDMILL_JOBS * kwh_made / kwh_output;

    if ((commodityCount[STUFF_JOBS] >= jobs_used)
     && (kwh_made >= WINDMILL_KWH))
    {
        consumeStuff(STUFF_JOBS, jobs_used);
        produceStuff(STUFF_KWH, kwh_made);
        animate_enable = true;
        working_days += kwh_made;
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

void Windmill::animate() {
  if(animate_enable && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(ANTIQUE_WINDMILL_ANIM_SPEED);
    ++frameIt->frame %= 3;
  }
}

void Windmill::report()
{
    int i = 0;
    mps_store_sd(i++, constructionGroup->name, ID);
    mps_store_sfp(i++, N_("busy"), float(busy) / kwh_output);
    mps_store_sfp(i++, N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, N_("Output"), kwh_output);
    // i++;
    list_commodities(&i);
}

/** @file lincity/modules/windmill.cpp */
