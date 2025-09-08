/* ---------------------------------------------------------------------- *
 * src/lincity/modules/substation.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2025 David Bears <dbear4q@gmail.com>
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

#include "substation.hpp"

#include <list>                     // for _List_iterator
#include <map>                      // for map
#include <string>                   // for basic_string, operator<

#include "lincity-ng/Mps.hpp"       // for Mps
#include "lincity/groups.hpp"         // for GROUP_SUBSTATION
#include "lincity/lin-city.hpp"       // for FALSE
#include "lincity/resources.hpp"    // for ExtraFrame, ResourceGroup
#include "lincity/world.hpp"          // for World
#include "util/gettextutil.hpp"

SubstationConstructionGroup substationConstructionGroup(
    N_("Power Substation"),
    N_("Power Substations"),
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


Construction *SubstationConstructionGroup::createConstruction(World& world) {
  return new Substation(world, this);
}

Substation::Substation(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->working_days = 0;
  this->busy = 0;
  initialize_commodities();

  commodityMaxCons[STUFF_HIVOLT] = 100 * SUBSTATION_HIVOLT;
  commodityMaxProd[STUFF_LOVOLT] = 100 * 2 * SUBSTATION_HIVOLT;
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
    if(world.total_time % 100 == 99) {
      reset_prod_counters();
      busy = working_days/SUBSTATION_HIVOLT;
      working_days = 0;
    }
}

void Substation::animate(unsigned long real_time) {
  if (commodityCount[STUFF_LOVOLT] > (MAX_LOVOLT_AT_SUBSTATION / 2))
    frameIt->resourceGroup = ResourceGroup::resMap["SubstationOn"];
  else if (commodityCount[STUFF_LOVOLT] > (MAX_LOVOLT_AT_SUBSTATION / 10))
    frameIt->resourceGroup = ResourceGroup::resMap["SubstationOff"];
  else
    frameIt->resourceGroup = ResourceGroup::resMap["Substation"];
  soundGroup = frameIt->resourceGroup;
}

void Substation::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  mps.add_sfp(N_("busy"), busy);
  list_commodities(mps, production);
}

/** @file lincity/modules/substation.cpp */
