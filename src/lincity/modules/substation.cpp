/* ---------------------------------------------------------------------- *
 * substation.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "substation.h"
#include "lincity-ng/Sound.hpp"

SubstationConstructionGroup substationConstructionGroup(
    "Power Substation",
     FALSE,                     /* need credit? */
     GROUP_SUBSTATION,
     GROUP_SUBSTATION_SIZE,
     GROUP_SUBSTATION_COLOUR,
     GROUP_SUBSTATION_COST_MUL,
     GROUP_SUBSTATION_BUL_COST,
     GROUP_SUBSTATION_FIREC,
     GROUP_SUBSTATION_COST,
     GROUP_SUBSTATION_TECH,
     GROUP_SUBSTATION_RANGE
);

//helper groups for graphics and sound sets, dont add them to ConstructionGroup::groupMap
SubstationConstructionGroup substation_RG_ConstructionGroup = substationConstructionGroup;
SubstationConstructionGroup substation_G_ConstructionGroup  = substationConstructionGroup;


Construction *SubstationConstructionGroup::createConstruction(int x, int y) {
    return new Substation(x, y, this);
}

void Substation::update()
{
    int use_MWH = (SUBSTATION_MWH < commodityCount[STUFF_MWH])?SUBSTATION_MWH:commodityCount[STUFF_MWH];
    if ( (use_MWH > 0)
     && (commodityCount[STUFF_KWH] <= MAX_KWH_AT_SUBSTATION-2 * use_MWH))
    {
        commodityCount[STUFF_MWH] -= use_MWH;
        commodityCount[STUFF_KWH] += 2 * use_MWH;
        working_days += use_MWH;
    }
    if (total_time % 100 == 0) //monthly update
    {
        busy = working_days/SUBSTATION_MWH;
        working_days = 0;
    }
    /* choose a graphic */
    if (commodityCount[STUFF_KWH] > (MAX_KWH_AT_SUBSTATION / 2))
    {   constructionGroup = &substation_G_ConstructionGroup;}
    else if (commodityCount[STUFF_KWH] > (MAX_KWH_AT_SUBSTATION / 10))
    {   constructionGroup = &substation_RG_ConstructionGroup;}
    else
    {   constructionGroup = &substationConstructionGroup;}
}

void Substation::report()
{
    int i = 0;
    mps_store_sd(i++,constructionGroup->name,ID);
    i++;
    mps_store_sfp(i++, "busy", busy);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/substation.cpp */

