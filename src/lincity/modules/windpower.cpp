/* ---------------------------------------------------------------------- *
 * src/lincity/modules/windpower.cpp
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

#include "windpower.h"

#include <list>                     // for _List_iterator
#include <map>                      // for map

#include "modules.h"

WindpowerConstructionGroup windpowerConstructionGroup(
    N_("Wind Power"),
     TRUE,                     /* need credit? */
     GROUP_WIND_POWER,
     GROUP_WIND_POWER_SIZE,
     GROUP_WIND_POWER_COLOUR,
     GROUP_WIND_POWER_COST_MUL,
     GROUP_WIND_POWER_BUL_COST,
     GROUP_WIND_POWER_FIREC,
     GROUP_WIND_POWER_COST,
     GROUP_WIND_POWER_TECH,
     GROUP_WIND_POWER_RANGE
);

//WindpowerConstructionGroup windpower_RG_ConstructionGroup = windpowerConstructionGroup;
//WindpowerConstructionGroup windpower_G_ConstructionGroup = windpowerConstructionGroup;

Construction *WindpowerConstructionGroup::createConstruction() {
  return new Windpower(this);
}

void Windpower::update()
{
    if (!(total_time%(WIND_POWER_RCOST)))
    {   windmill_cost++;}
    int hivolt_made = (commodityCount[STUFF_HIVOLT] + hivolt_output <= MAX_HIVOLT_AT_WIND_POWER)?hivolt_output:MAX_HIVOLT_AT_WIND_POWER-commodityCount[STUFF_HIVOLT];
    int labor_used = WIND_POWER_LABOR * hivolt_made/hivolt_output;

    if ((commodityCount[STUFF_LABOR] >= labor_used)
     && hivolt_made > WIND_POWER_HIVOLT)
    {
        consumeStuff(STUFF_LABOR, labor_used);
        produceStuff(STUFF_HIVOLT, hivolt_made);
        animate_enable = true;
        working_days += hivolt_made;
    }
    else
    {   animate_enable = false;}
    //monthly update
    if (total_time % 100 == 99)
    {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }
}

void Windpower::animate() {
  if(animate_enable && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(WIND_POWER_ANIM_SPEED);
    ++frameIt->frame %= 3;
  }

  if (commodityCount[STUFF_HIVOLT] > MAX_HIVOLT_AT_WIND_POWER/2)
    frameIt->resourceGroup = ResourceGroup::resMap["WindMillHTechG"];
  else if (commodityCount[STUFF_HIVOLT] > MAX_HIVOLT_AT_WIND_POWER/10)
    frameIt->resourceGroup = ResourceGroup::resMap["WindMillHTechRG"];
  else
    frameIt->resourceGroup = ResourceGroup::resMap["WindMillHTech"];
  soundGroup = frameIt->resourceGroup;
}


void Windpower::report()
{
    int i = 0;
    mps_store_title(i, constructionGroup->name);
    mps_store_sfp(i++, N_("busy"), float(busy) / hivolt_output);
    mps_store_sfp(i++, N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, N_("Output"), hivolt_output);
    // i++;
    list_commodities(&i);
}

void Windpower::save(xmlTextWriterPtr xmlWriter) {
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tech", "%d", tech);
}

bool Windpower::loadMember(xmlpp::TextReader& xmlReader) {
  std::string name = xmlReader.get_name();
  if(name == "tech") tech = std::stoi(xmlReader.read_inner_xml());
  if(name == "mwh_output");
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/windpower.cpp */
