/* ---------------------------------------------------------------------- *
 * src/lincity/modules/substation.cpp
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

#include "substation.h"

#include <list>                     // for _List_iterator
#include <map>                      // for map
#include <string>                   // for basic_string, operator<

#include "modules.h"

SubstationConstructionGroup substationConstructionGroup(
    N_("Power Substation"),
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

//helper groups for graphics and sound sets, don't add them to ConstructionGroup::groupMap
SubstationConstructionGroup substation_RG_ConstructionGroup = substationConstructionGroup;
SubstationConstructionGroup substation_G_ConstructionGroup  = substationConstructionGroup;


Construction *SubstationConstructionGroup::createConstruction(int x, int y) {
    return new Substation(x, y, this);
}

void Substation::update()
{
    int use_hivolt = (SUBSTATION_HIVOLT < commodityCount[STUFF_HIVOLT])?SUBSTATION_HIVOLT:commodityCount[STUFF_HIVOLT];
    if ( (use_hivolt > 0)
     && (commodityCount[STUFF_LOVOLT] <= MAX_LOVOLT_AT_SUBSTATION-2 * use_hivolt))
    {
        consumeStuff(STUFF_HIVOLT, use_hivolt);
        produceStuff(STUFF_LOVOLT, 2 * use_hivolt);
        working_days += use_hivolt;
    }
    if (total_time % 100 == 99) //monthly update
    {
        reset_prod_counters();
        busy = working_days/SUBSTATION_HIVOLT;
        working_days = 0;
    }
}

void Substation::animate() {
  if (commodityCount[STUFF_LOVOLT] > (MAX_LOVOLT_AT_SUBSTATION / 2))
    frameIt->resourceGroup = ResourceGroup::resMap["SubstationOn"];
  else if (commodityCount[STUFF_LOVOLT] > (MAX_LOVOLT_AT_SUBSTATION / 10))
    frameIt->resourceGroup = ResourceGroup::resMap["SubstationOff"];
  else
    frameIt->resourceGroup = ResourceGroup::resMap["Substation"];
  soundGroup = frameIt->resourceGroup;
}

void Substation::report()
{
    int i = 0;
    mps_store_sd(i++, constructionGroup->name, ID);
    i++;
    mps_store_sfp(i++, N_("busy"), busy);
    // i++;
    list_commodities(&i);
}

/** @file lincity/modules/substation.cpp */
