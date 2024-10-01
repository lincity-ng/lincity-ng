/* ---------------------------------------------------------------------- *
 * src/lincity/modules/tip.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2024 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "tip.h"

#include <list>                     // for _List_iterator

#include "modules.h"

// Tip:
TipConstructionGroup tipConstructionGroup(
    N_("Land Fill"),
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
    total_waste -= degration_days / TIP_DEGRADE_TIME;
    degration_days %= TIP_DEGRADE_TIME;

    if ((commodityCount[STUFF_WASTE] >= WASTE_BURRIED)
    && (commodityCount[STUFF_WASTE]*100/TIP_TAKES_WASTE > CRITICAL_WASTE_LEVEL)
    && (total_waste + WASTE_BURRIED <= MAX_WASTE_AT_TIP))
    {
        consumeStuff(STUFF_WASTE, WASTE_BURRIED);
        total_waste += WASTE_BURRIED;
        working_days++;
        sust_dig_ore_coal_tip_flag = 0;
    }
    else if ((commodityCount[STUFF_WASTE] + WASTE_BURRIED <= TIP_TAKES_WASTE)
    && (commodityCount[STUFF_WASTE]*100/TIP_TAKES_WASTE < CRITICAL_WASTE_LEVEL)
    && (total_waste > 0))
    {
        int waste_dug = (WASTE_BURRIED < total_waste)?WASTE_BURRIED:total_waste;
        produceStuff(STUFF_WASTE, waste_dug);
        total_waste -= waste_dug;
        working_days++;
    }
    if ((total_time % 100) == 99)
    {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }
}

void Tip::animate() {
  int i = (total_waste /3 * 22) / MAX_WASTE_AT_TIP;
  if (total_waste > 0 && i < 8)
  {   i++;}
  frameIt->frame = i;
}

void Tip::report()
{
    int i = 0;

    mps_store_sd(i++, constructionGroup->name, ID);
    i++;
    mps_store_sfp(i++,N_("busy"), busy);
    mps_store_sd(i++, N_("Waste"), total_waste);
    mps_store_sfp(i++, N_("Filled"), (float)total_waste*100/MAX_WASTE_AT_TIP);
    // i++;
    list_commodities(&i);
}

/** @file lincity/modules/tip.cpp */
