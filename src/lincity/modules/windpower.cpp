/* ---------------------------------------------------------------------- *
 * src/lincity/modules/windpower.cpp
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

#include "windpower.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <list>                           // for _List_iterator
#include <map>                            // for map
#include <string>                         // for basic_string, operator==

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"             // for GROUP_WIND_POWER
#include "lincity/lin-city.hpp"           // for MAX_TECH_LEVEL, ANIM_THRESHOLD
#include "lincity/resources.hpp"          // for ExtraFrame, ResourceGroup
#include "lincity/stats.hpp"              // for Stat, Stats
#include "lincity/world.hpp"              // for World
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

WindpowerConstructionGroup windpowerConstructionGroup(
  N_("Wind Power"),
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

Construction *WindpowerConstructionGroup::createConstruction(World& world) {
  return new Windpower(world, this);
}

Windpower::Windpower(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->anim = 0;
  this->animate_enable = false;
  this->tech = world.tech_level;
  this->working_days = 0;
  this->busy = 0;
  initialize_commodities();

  commodityMaxCons[STUFF_LABOR] = 100 * WIND_POWER_LABOR;
  // commodityMaxProd[STUFF_HIVOLT] = 100 * hivolt_output;
}

void Windpower::update()
{
    if(world.total_time % WIND_POWER_RCOST)
      world.stats.expenses.windmill++;
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
    if(world.total_time % 100 == 99) {
      reset_prod_counters();
      busy = working_days;
      working_days = 0;
    }
}

void Windpower::animate(unsigned long real_time) {
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


void Windpower::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.add_sfp(N_("busy"), float(busy) / hivolt_output);
  mps.add_sfp(N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
  mps.add_sd(N_("Output"), hivolt_output);
  list_commodities(mps, production);
}

void Windpower::place(MapPoint point) {
  Construction::place(point);

  this->hivolt_output = (int)(WIND_POWER_HIVOLT +
    (((double)tech * WIND_POWER_HIVOLT) / MAX_TECH_LEVEL));

  commodityMaxProd[STUFF_HIVOLT] = 100 * hivolt_output;
}

void Windpower::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"tech", xmlFormat<int>(tech));
  Construction::save(xmlWriter);
}

bool Windpower::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string name = xmlReader.get_name();
  if(name == "tech") tech = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "mwh_output");
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/windpower.cpp */
