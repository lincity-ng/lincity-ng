/* ---------------------------------------------------------------------- *
 * monument.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "monument.h"
#include "lincity-ng/Sound.hpp"

extern int mps_x, mps_y;

MonumentConstructionGroup monumentConstructionGroup(
    "Monument",
    FALSE,                     /* need credit? */
    GROUP_MONUMENT,
    GROUP_MONUMENT_SIZE,
    GROUP_MONUMENT_COLOUR,
    GROUP_MONUMENT_COST_MUL,
    GROUP_MONUMENT_BUL_COST,
    GROUP_MONUMENT_FIREC,
    GROUP_MONUMENT_COST,
    GROUP_MONUMENT_TECH,
    GROUP_MONUMENT_RANGE
);

MonumentConstructionGroup monumentFinishedConstructionGroup = monumentConstructionGroup;

Construction *MonumentConstructionGroup::createConstruction(int x, int y) {
    return new Monument(x, y, this);
}

void Monument::update()
{
    if ((commodityCount[STUFF_JOBS] > MONUMENT_GET_JOBS) && (completion < 100))
    {
        commodityCount[STUFF_JOBS] -= MONUMENT_GET_JOBS;
        jobs_consumed += MONUMENT_GET_JOBS;
        completion = jobs_consumed * 100 / BUILD_MONUMENT_JOBS;
        ++working_days;
    }
    //monthly update
    if (total_time % 100 == 0)
    {
        busy = working_days;
        working_days = 0;
        if(commodityCount[STUFF_JOBS]==0 && completed)
        {   deneighborize();}
    }
    /* now choose a graphic */
    if (completion >= 100)
    {
        if(!completed)
        {
            completed = true;
            type = 0;
            flags |= (FLAG_EVACUATE | FLAG_NEVER_EVACUATE);
            constructionGroup = &monumentFinishedConstructionGroup;
            if (mps_x == x && mps_y == y)
            {   mps_set(x, y, MPS_MAP);}
            //dont clear commodiyCount for savegame compatability
        }
        /* inc tech level only if fully built and tech less
           than MONUMENT_TECH_EXPIRE */
        if (tech_level < (MONUMENT_TECH_EXPIRE * 1000)
            && (total_time % MONUMENT_DAYS_PER_TECH) == 1)
        {
            tail_off++;
            if (tail_off > (tech_level / 10000) - 2)
            {
                tech_level++;
                tech_made++;
                tail_off = 0;
            }
        }
    }
    else if (completion >= 80)
    {   type = 4;}
    else if (completion >= 60)
    {   type = 3;}
    else if (completion >= 40)
    {   type = 2;}
    else if (completion >= 20)
    {   type = 1;}
    else
    {   type = 0;}
}

void Monument::report()
{
    int i = 0;

    mps_store_sd(i++, constructionGroup->name,ID);
    i++;
    /* Display tech contribution only after monument is complete */
    if (completion >= 100) {
        i++;
        mps_store_sfp(i++, "Wisdom bestowed", tech_made * 100.0 / MAX_TECH_LEVEL);
    }
    else
    {
        mps_store_sfp(i++, _("busy"), (float) busy);
        i++;
        list_commodities(&i);
        i++;
        mps_store_sfp(i++, "Completion", completion);
    }
}


/** @file lincity/modules/monument.cpp */

