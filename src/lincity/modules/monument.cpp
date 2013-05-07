/* ---------------------------------------------------------------------- *
 * monument.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "monument.h"

MonumentConstructionGroup monumentConstructionGroup(
    "Monument",
    FALSE,                     /* need credit? */
    GROUP_MONUMENT,
    2,                         /* size */
    GROUP_MONUMENT_COLOUR,
    GROUP_MONUMENT_COST_MUL,
    GROUP_MONUMENT_BUL_COST,
    GROUP_MONUMENT_FIREC,
    GROUP_MONUMENT_COST,
    GROUP_MONUMENT_TECH
);

Construction *MonumentConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new Monument(x, y, type);
}

void Monument::update()
{
    if ((commodityCount[STUFF_JOBS] > MONUMENT_GET_JOBS) && (completion < 100))
    {
        commodityCount[STUFF_JOBS] -= MONUMENT_GET_JOBS;
        jobs_consumed += MONUMENT_GET_JOBS;
        completion = jobs_consumed * 100 / BUILD_MONUMENT_JOBS;
    }    
    /* now choose a graphic */
    if (completion >= 100)
    {        
        type = CST_MONUMENT_5;
        flags |= FLAG_EVACUATE;
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
        type = CST_MONUMENT_4;
    else if (completion >= 60)
        type = CST_MONUMENT_3;
    else if (completion >= 40)
        type = CST_MONUMENT_2;
    else if (completion >= 20)
        type = CST_MONUMENT_1;
    else
        type = CST_MONUMENT_0;
}

void Monument::report()
{
    int i = 0;

    mps_store_sd(i++, constructionGroup->name,ID);
    i++;
    i++;
    /* Display tech contribution only after monument is complete */
    if (completion >= 100) {
        mps_store_sfp(i++, "Wisdom bestowed", tech_made * 100.0 / MAX_TECH_LEVEL);
    }
    else
    {      
        list_commodities(&i);        
        i++;        
        mps_store_sfp(i++, "Completion", completion);
    }
}

/** @file lincity/modules/monument.cpp */

