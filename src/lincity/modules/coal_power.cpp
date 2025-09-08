/* ---------------------------------------------------------------------- *
 * src/lincity/modules/coal_power.cpp
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

#include "coal_power.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <stdlib.h>                       // for rand
#include <map>                            // for map
#include <string>                         // for basic_string, operator<
#include <vector>                         // for vector

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"             // for GROUP_COAL_POWER
#include "lincity/lin-city.hpp"           // for MAX_TECH_LEVEL, ANIM_THRESHOLD
#include "lincity/resources.hpp"          // for ExtraFrame, ResourceGroup
#include "lincity/world.hpp"              // for World, Map, MapTile
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

Coal_powerConstructionGroup coal_powerConstructionGroup(
     N_("Coal Power Station"),
     N_("Coal Power Stations"),
     FALSE,                     /* need credit? */
     GROUP_COAL_POWER,
     GROUP_COAL_POWER_SIZE,
     GROUP_COAL_POWER_COLOUR,
     GROUP_COAL_POWER_COST_MUL,
     GROUP_COAL_POWER_BUL_COST,
     GROUP_COAL_POWER_FIREC,
     GROUP_COAL_POWER_COST,
     GROUP_COAL_POWER_TECH,
     GROUP_COAL_POWER_RANGE
);

Coal_power::Coal_power(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->anim = 0;
  this->tech = world.tech_level;
  this->working_days = 0;
  this->busy = 0;
  initialize_commodities();

  commodityMaxCons[STUFF_LABOR] = 100 * LABOR_COALPS_GENERATE;
  commodityMaxCons[STUFF_COAL] = 100 *
    (POWERS_COAL_OUTPUT / POWER_PER_COAL);
  // commodityMaxProd[STUFF_HIVOLT] = 100 * hivolt_output;
}

Coal_power::~Coal_power() {
}

//helper groups for graphics and sound sets, dont add them to ConstructionGroup::groupMap
//Coal_powerConstructionGroup coal_power_low_ConstructionGroup  = coal_powerConstructionGroup;
//Coal_powerConstructionGroup coal_power_med_ConstructionGroup  = coal_powerConstructionGroup;
//Coal_powerConstructionGroup coal_power_full_ConstructionGroup = coal_powerConstructionGroup;

Construction *Coal_powerConstructionGroup::createConstruction(World& world) {
  return new Coal_power(world, this);
}

void Coal_power::update()
{
    int hivolt_made = (
      commodityCount[STUFF_HIVOLT] + hivolt_output <= MAX_HIVOLT_AT_COALPS)?hivolt_output:MAX_HIVOLT_AT_COALPS-commodityCount[STUFF_HIVOLT];
    int labor_used = LABOR_COALPS_GENERATE*(hivolt_made/100)/(hivolt_output/100);
    int coal_used = POWERS_COAL_OUTPUT / POWER_PER_COAL * (hivolt_made/100) /(hivolt_output/100);
    if ((commodityCount[STUFF_LABOR] >= labor_used )
     && (commodityCount[STUFF_COAL] >= coal_used)
     && (hivolt_made >= POWERS_COAL_OUTPUT))
    {
        consumeStuff(STUFF_LABOR, labor_used);
        consumeStuff(STUFF_COAL, coal_used);
        produceStuff(STUFF_HIVOLT, hivolt_made);
        world.map(point)->pollution += POWERS_COAL_POLLUTION *(hivolt_made/100)/(hivolt_output/100);
        working_days += (hivolt_made/100);
    }
    //monthly update
    if (world.total_time % 100 == 99) {
        reset_prod_counters();
        busy = working_days / (hivolt_output/100);
        working_days = 0;
    }
}

void Coal_power::animate(unsigned long real_time) {

  if(real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(SMOKE_ANIM_SPEED);
    int active = 9*busy/100;
    for(int i = 0; i < frits.size(); i++) {
      auto& frit = frits[i];
      const int s = frit->resourceGroup->graphicsInfoVector.size();
      if(i >= active) {
        frit->frame = -1;
      }
      else if(frit->frame < 0 || rand() % 1600 != 0) {
        // always randomize new plumes and sometimes existing ones
        frit->frame = rand() % s;
      }
      else if(++frit->frame >= s) {
        frit->frame = 0;
      }
    }
  }

  if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_COALPS*4/5))
  {   frameIt->resourceGroup = ResourceGroup::resMap["PowerCoalFull"];}
  else if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_COALPS / 2))
  {   frameIt->resourceGroup = ResourceGroup::resMap["PowerCoalMed"];}
  else if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_COALPS / 10))
  {   frameIt->resourceGroup = ResourceGroup::resMap["PowerCoalLow"];}
  else
  {   frameIt->resourceGroup = ResourceGroup::resMap["PowerCoalEmpty"];}
  soundGroup = frameIt->resourceGroup;
}

void Coal_power::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.add_sfp(N_("busy"), busy);
  mps.add_sfp(N_("Tech"), (float)(tech * 100.0) / MAX_TECH_LEVEL);
  mps.add_sd(N_("Output"), hivolt_output);
  list_commodities(mps, production);
}

void Coal_power::init_resources() {
  Construction::init_resources();

  MapTile& tile = *world.map(point);
  for(auto& frit : frits) {
    frit = tile.createframe();
    frit->resourceGroup = ResourceGroup::resMap["BlackSmoke"];
    frit->frame = -1; // hide smoke
  }
  frits[0]->move_x = 5;
  frits[0]->move_y = -378;
  frits[1]->move_x = 29;
  frits[1]->move_y = -390;
  frits[2]->move_x = 52;
  frits[2]->move_y = -397;
  frits[3]->move_x = 76;
  frits[3]->move_y = -409;
  frits[4]->move_x = 65;
  frits[4]->move_y = -348;
  frits[5]->move_x = 89;
  frits[5]->move_y = -360;
  frits[6]->move_x = 112;
  frits[6]->move_y = -371;
  frits[7]->move_x = 136;
  frits[7]->move_y = -383;
}

void
Coal_power::detach() {
  MapTile& tile = *world.map(point);
  for(const auto& frit : frits) {
    tile.killframe(frit);
  }
  Construction::detach();
}

void Coal_power::place(MapPoint point) {
  Construction::place(point);

  this->hivolt_output = (int)(POWERS_COAL_OUTPUT +
    (((double)tech * POWERS_COAL_OUTPUT) / MAX_TECH_LEVEL));
  commodityMaxProd[STUFF_HIVOLT] = 100 * hivolt_output;
}

void Coal_power::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"tech", xmlFormat(tech));
  Construction::save(xmlWriter);
}

bool
Coal_power::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version
) {
  std::string name = xmlReader.get_name();
  if(name == "tech") tech = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "mwh_output");
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/coal_power.cpp */
