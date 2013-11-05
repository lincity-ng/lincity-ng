/* ---------------------------------------------------------------------- *
 * tip.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "tip.h"

// Tip:
TipConstructionGroup tipConstructionGroup(
    "Land Fill",
     FALSE,                     /* need credit? */
     GROUP_TIP,
     GROUP_TIP_SIZE,
     GROUP_TIP_COLOUR,
     GROUP_TIP_COST_MUL,
     GROUP_TIP_BUL_COST,
     GROUP_TIP_FIREC,
     GROUP_TIP_COST,
     GROUP_TIP_TECH,
     GROUP_TIP_RANGE
);

Construction *TipConstructionGroup::createConstruction(int x, int y ) {
    return new Tip(x, y, this);
}


void Tip::update()
{
    //the waste is always slowly degrading
    //max degradiation per day is about 42 (10M/240k)
    degration_days += total_waste;
    while (degration_days > TIP_DEGRADE_TIME)
    {
        degration_days -= TIP_DEGRADE_TIME;
        --total_waste;
        if (degration_days > 7 * TIP_DEGRADE_TIME)
        {
            degration_days -= 7 * TIP_DEGRADE_TIME;
            total_waste -= 7;
        }
    }

    if ((commodityCount[STUFF_WASTE] >= WASTE_BURRIED)
    && (commodityCount[STUFF_WASTE]*100/TIP_TAKES_WASTE > CRITICAL_WASTE_LEVEL)
    && (total_waste + WASTE_BURRIED < MAX_WASTE_AT_TIP))
    {
        commodityCount[STUFF_WASTE] -= WASTE_BURRIED;
        total_waste += WASTE_BURRIED;
        working_days++;
    }
    else if ((commodityCount[STUFF_WASTE] + WASTE_BURRIED <= TIP_TAKES_WASTE)
    && (commodityCount[STUFF_WASTE]*100/TIP_TAKES_WASTE < CRITICAL_WASTE_LEVEL)
    && (total_waste > 0))
    {
        int waste_dug = (WASTE_BURRIED < total_waste)?WASTE_BURRIED:total_waste;
        commodityCount[STUFF_WASTE] += waste_dug;
        total_waste -= waste_dug;
        working_days++;
    }
    if ((total_time % 100) == 0)
    {
        busy = working_days;
        working_days = 0;
        int i = (total_waste /3 * 22) / MAX_WASTE_AT_TIP;
        if (total_waste > 0 && i < 8)
        {   i++;}
        type = i;
    }
}

void Tip::report()
{
    int i = 0;

    mps_store_sd(i++,constructionGroup->name,ID);
    i++;
    mps_store_sfp(i++,"busy", busy);
    mps_store_sd(i++,"Waste", total_waste);
    mps_store_sfp(i++,_("Filled"), (float)total_waste*100/MAX_WASTE_AT_TIP);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/tip.cpp */

