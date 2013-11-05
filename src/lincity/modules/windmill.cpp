/* ---------------------------------------------------------------------- *
 * windmill.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "windmill.h"


WindmillConstructionGroup windmillConstructionGroup(
    "Windmill",
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
        commodityCount[STUFF_JOBS] -= jobs_used;
        commodityCount[STUFF_KWH] += kwh_made;
        animate = true;
        working_days += kwh_made;
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
        anim = real_time + ANTIQUE_WINDMILL_ANIM_SPEED;
    }
}

void Windmill::report()
{
    int i = 0;
    mps_store_sd(i++,constructionGroup->name,ID);
    mps_store_sfp(i++, "busy", float(busy) / kwh_output);
    mps_store_sfp(i++, "Tech", (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, "Output", kwh_output);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/windmill.cpp */

