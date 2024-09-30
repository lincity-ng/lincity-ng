/* ---------------------------------------------------------------------- *
 * src/lincity/modules/recycle.cpp
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

#include "recycle.h"

#include "modules.h"

RecycleConstructionGroup recycleConstructionGroup(
    N_("Recycling Centre"),
    FALSE,                     /* need credit? */
    GROUP_RECYCLE,
    GROUP_RECYCLE_SIZE,
    GROUP_RECYCLE_COLOUR,
    GROUP_RECYCLE_COST_MUL,
    GROUP_RECYCLE_BUL_COST,
    GROUP_RECYCLE_FIREC,
    GROUP_RECYCLE_COST,
    GROUP_RECYCLE_TECH,
    GROUP_RECYCLE_RANGE
);

Construction *RecycleConstructionGroup::createConstruction(int x, int y) {
    return new Recycle(x, y, this);
}

void Recycle::update()
{
    recycle_cost += RECYCLE_RUNNING_COST;

    // always recycle waste and only make steel & ore if there are free capacities
    if (commodityCount[STUFF_WASTE] >= WASTE_RECYCLED
        && commodityCount[STUFF_LOVOLT] >= LOVOLT_RECYCLE_WASTE
        && commodityCount[STUFF_LABOR] >= RECYCLE_LABOR)
    {
        consumeStuff(STUFF_LABOR, RECYCLE_LABOR);
        consumeStuff(STUFF_LOVOLT, LOVOLT_RECYCLE_WASTE);
        consumeStuff(STUFF_WASTE, WASTE_RECYCLED);
        working_days++;
        // rather loose ore / steel than stop recycling the waste
        produceStuff(STUFF_ORE, make_ore);
        produceStuff(STUFF_STEEL, make_steel);
        if(commodityCount[STUFF_ORE]>MAX_ORE_AT_RECYCLE)
        {   levelStuff(STUFF_ORE, MAX_ORE_AT_RECYCLE);}
        if(commodityCount[STUFF_STEEL]>MAX_STEEL_AT_RECYCLE)
        {   levelStuff(STUFF_STEEL, MAX_STEEL_AT_RECYCLE);}

    }
    // monthly update
    if (total_time % 100 == 99)
    {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }
    // if we've still >90% waste in stock, burn some waste cleanly.
    if (commodityCount[STUFF_WASTE] > (MAX_WASTE_AT_RECYCLE * 9 / 10))
    {   consumeStuff(STUFF_WASTE, BURN_WASTE_AT_RECYCLE);}
}

void Recycle::report()
{
    int i = 0;

    mps_store_sd(i++, constructionGroup->name, ID);
    i++;
    mps_store_sfp(i++, N_("Tech"), tech * 100.0f / MAX_TECH_LEVEL);
    mps_store_sfp(i++, N_("Efficiency Ore"), (float) make_ore * 100 / WASTE_RECYCLED);
    mps_store_sfp(i++, N_("Efficiency Steel"),(float) make_steel * 100 / WASTE_RECYCLED);
    mps_store_sfp(i++, N_("busy"), busy);
    // i++;
    list_commodities(&i);
}

/** @file lincity/modules/recycle.cpp */
