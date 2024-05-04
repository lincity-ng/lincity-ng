/* ---------------------------------------------------------------------- *
 * mill.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "mill.h"

#include <list>                     // for _List_iterator
#include <string>                   // for basic_string
#include <vector>                   // for vector

#include "gui_interface/mps.h"      // for mps_store_sd, mps_store_sfp
#include "lincity/engine.h"         // for real_time
#include "lincity/groups.h"         // for GROUP_MILL
#include "lincity/lin-city.h"       // for FLAG_POWERED, ANIM_THRESHOLD, FALSE
#include "lincity/lintypes.h"       // for Commodity, ExtraFrame, Constructi...
#include "lincity/world.h"          // for World
#include "tinygettext/gettext.hpp"  // for N_


MillConstructionGroup millConstructionGroup(
    N_("Textile Mill"),
    FALSE,                     /* need credit? */
    GROUP_MILL,
    GROUP_MILL_SIZE,
    GROUP_MILL_COLOUR,
    GROUP_MILL_COST_MUL,
    GROUP_MILL_BUL_COST,
    GROUP_MILL_FIREC,
    GROUP_MILL_COST,
    GROUP_MILL_TECH,
    GROUP_MILL_RANGE
);

Construction *MillConstructionGroup::createConstruction(int x, int y) {
    return new Mill(x, y, this);
}

void Mill::update()
{
    bool use_coal = (commodityCount[STUFF_COAL]*MAX_KWH_AT_MILL > commodityCount[STUFF_KWH]*MAX_COAL_AT_MILL);
    flags &= ~(FLAG_POWERED);
    if ((use_coal?commodityCount[STUFF_COAL]:commodityCount[STUFF_KWH]) >= (use_coal?COAL_USED_BY_MILL:COAL_USED_BY_MILL * MILL_POWER_PER_COAL)
    && (flags |= FLAG_POWERED, commodityCount[STUFF_FOOD] >= FOOD_USED_BY_MILL)
    && (commodityCount[STUFF_JOBS] >= MILL_JOBS)
    && (commodityCount[STUFF_GOODS] <= MAX_GOODS_AT_MILL - GOODS_MADE_BY_MILL))
    {
        if(use_coal)
            consumeStuff(STUFF_COAL, COAL_USED_BY_MILL);
        else
            consumeStuff(STUFF_KWH, COAL_USED_BY_MILL * MILL_POWER_PER_COAL);
        consumeStuff(STUFF_FOOD, FOOD_USED_BY_MILL);
        consumeStuff(STUFF_JOBS, MILL_JOBS);
        produceStuff(STUFF_GOODS, GOODS_MADE_BY_MILL);
        ++working_days;
        animate_enable = true;
        if ((++pol_count %= 7) == 0)
        {   world(x,y)->pollution++;}
    }

    //monthly update
    if(total_time % 100 == 99) {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }
}

void Mill::animate() {
  int& frame = frameIt->frame;
  if(animate_enable && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(MILL_ANIM_SPEED);
    animate_enable = false;
    if(++frame >= (int)frameIt->resourceGroup->graphicsInfoVector.size())
      frame = 1;
  }
  else if(!busy) {
    frame = 0;
  }
}

void Mill::report()
{
    int i = 0;
    mps_store_sd(i++, constructionGroup->name, ID);
    mps_store_sfp(i++, N_("busy"), (float) busy);
    // i++;
    list_commodities(&i);
}


/** @file lincity/modules/mill.cpp */
