/* ---------------------------------------------------------------------- *
 * university.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "university.h"

// university place:
UniversityConstructionGroup universityConstructionGroup(
    "University",
     TRUE,                     /* need credit? */
     GROUP_UNIVERSITY,
     GROUP_UNIVERSITY_SIZE,
     GROUP_UNIVERSITY_COLOUR,
     GROUP_UNIVERSITY_COST_MUL,
     GROUP_UNIVERSITY_BUL_COST,
     GROUP_UNIVERSITY_FIREC,
     GROUP_UNIVERSITY_COST,
     GROUP_UNIVERSITY_TECH,
     GROUP_UNIVERSITY_RANGE
);

Construction *UniversityConstructionGroup::createConstruction(int x, int y) {
    return new University(x, y, this);
}

void University::update()
{
    university_cost += UNIVERSITY_RUNNING_COST;
    //do the teaching
    if (commodityCount[STUFF_JOBS] >= UNIVERSITY_JOBS
    &&  commodityCount[STUFF_GOODS] >= UNIVERSITY_GOODS
    &&  commodityCount[STUFF_WASTE] + UNIVERSITY_GOODS / 3 <= MAX_WASTE_AT_UNIVERSITY)
    {
        commodityCount[STUFF_JOBS] -= UNIVERSITY_JOBS;
        commodityCount[STUFF_GOODS] -= UNIVERSITY_GOODS;
        commodityCount[STUFF_WASTE] += UNIVERSITY_GOODS / 3;
        ++working_days;
        tech_level += UNIVERSITY_TECH_MADE;
        total_tech_made += UNIVERSITY_TECH_MADE;
    }
    //monthly update
    if ((total_time % 100) == 0)
    {
        busy = working_days;
        working_days = 0;
    }
}

void University::report()
{
    int i = 0;
    mps_store_sd(i++,constructionGroup->name,ID);
    i++;
    mps_store_sfp(i++,"busy", busy);
    mps_store_sfp(i++, _("Tech researched"), total_tech_made * 100.0 / MAX_TECH_LEVEL);
    i++;
    list_commodities(&i);
}


/** @file lincity/modules/university.cpp */

