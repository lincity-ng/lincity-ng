/* ---------------------------------------------------------------------- *
 * blacksmith.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "blacksmith.h"

BlacksmithConstructionGroup blacksmithConstructionGroup(
    "Blacksmith",
    FALSE,                     /* need credit? */
    GROUP_BLACKSMITH,
    GROUP_BLACKSMITH_SIZE,
    GROUP_BLACKSMITH_COLOUR,
    GROUP_BLACKSMITH_COST_MUL,
    GROUP_BLACKSMITH_BUL_COST,
    GROUP_BLACKSMITH_FIREC,
    GROUP_BLACKSMITH_COST,
    GROUP_BLACKSMITH_TECH,
    GROUP_BLACKSMITH_RANGE
);

Construction *BlacksmithConstructionGroup::createConstruction(int x, int y) {
    return new Blacksmith(x, y, this);
}

void Blacksmith::update()
{
    //monthly update
    if (total_time % 100 == 0)
    {
        busy = working_days;
        working_days = 0;
    }
    if (pauseCounter++ < 0)
    {   return;}
    if ((commodityCount[STUFF_GOODS] + GOODS_MADE_BY_BLACKSMITH <= MAX_GOODS_AT_BLACKSMITH )
        && (commodityCount[STUFF_COAL] >= BLACKSMITH_COAL_USED)
        && (commodityCount[STUFF_STEEL] >= BLACKSMITH_STEEL_USED)
        && (commodityCount[STUFF_JOBS] >= BLACKSMITH_JOBS))
    {
        commodityCount[STUFF_GOODS] += GOODS_MADE_BY_BLACKSMITH;
        commodityCount[STUFF_COAL] -= BLACKSMITH_COAL_USED;
        commodityCount[STUFF_STEEL] -= BLACKSMITH_STEEL_USED;
        commodityCount[STUFF_JOBS] -= BLACKSMITH_JOBS;
        working_days++;
        if ((goods_made += GOODS_MADE_BY_BLACKSMITH) >= BLACKSMITH_BATCH)
        {
            animate = true;
            world(x,y)->pollution++;
            goods_made = 0;
        }
    }
    else
    {
        type = 0;
        animate = false;
        pauseCounter = -BLACKSMITH_CLOSE_TIME;
        return;
    }
    //animation
    if (animate && real_time > anim)
    {
        anim = real_time + BLACKSMITH_ANIM_SPEED;
        if(++type >= constructionGroup->graphicsInfoVector.size())
        {
            type = 1;
            animate = false;
        }
    }

}

void Blacksmith::report()
{
    int i = 0;

    mps_store_sd(i++, constructionGroup->name,ID);
    i++;
    mps_store_sfp(i++, _("busy"), (float) busy);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/blacksmith.cpp */

