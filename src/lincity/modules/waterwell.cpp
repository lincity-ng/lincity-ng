/* ---------------------------------------------------------------------- *
 * waterwell.c
 * Copyright (C) 2007 Alain Baeckeroot (alain.baeckeroot@laposte.net)
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "waterwell.h"

WaterwellConstructionGroup waterwellConstructionGroup(
    "Water tower",
     FALSE,                     /* need credit? */
     GROUP_WATERWELL,
     GROUP_WATERWELL_SIZE,
     GROUP_WATERWELL_COLOUR,
     GROUP_WATERWELL_COST_MUL,
     GROUP_WATERWELL_BUL_COST,
     GROUP_WATERWELL_FIREC,
     GROUP_WATERWELL_COST,
     GROUP_WATERWELL_TECH,
     GROUP_WATERWELL_RANGE
);

Construction *WaterwellConstructionGroup::createConstruction(int x, int y) {
    return new Waterwell(x, y, this);
}

void Waterwell::update()
{
    if(commodityCount[STUFF_WATER] + water_output <= MAX_WATER_AT_WATERWELL)
    {
        working_days++;
        commodityCount[STUFF_WATER] += water_output;
    }
    //monthly update
    if ((total_time % 100) == 0)
    {
        busy = working_days;
        working_days = 0;
    }
}

void Waterwell::report()
{
    int i = 0;

    const char *p;

    mps_store_sd(i++,constructionGroup->name, ID);
    i++;
    mps_store_sddp(i++, "Fertility", ugwCount, constructionGroup->size * constructionGroup->size);
    mps_store_sfp(i++,"busy", busy);
    mps_store_sddp(i++, _("Pollution"), world(x,y)->pollution, MAX_POLLUTION_AT_WATERWELL);
    p = world(x,y)->pollution>MAX_POLLUTION_AT_WATERWELL?"No":"Yes";
    mps_store_ss(i++, _("Drinkable"), p);
    list_commodities(&i);
}

/** @file lincity/modules/waterwell.cpp */

