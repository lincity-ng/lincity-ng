/* ---------------------------------------------------------------------- *
 * src/lincity/modules/solar_power.cpp
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

#include "solar_power.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <string>                         // for basic_string, operator==

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"             // for GROUP_SOLAR_POWER
#include "lincity/lin-city.hpp"           // for MAX_TECH_LEVEL, TRUE
#include "lincity/world.hpp"              // for World
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

SolarPowerConstructionGroup solarPowerConstructionGroup(
    N_("Solar Power Plant"),
    N_("Solar Power Plants"),
     TRUE,                     /* need credit? */
     GROUP_SOLAR_POWER,
     GROUP_SOLAR_POWER_SIZE,
     GROUP_SOLAR_POWER_COLOUR,
     GROUP_SOLAR_POWER_COST_MUL,
     GROUP_SOLAR_POWER_BUL_COST,
     GROUP_SOLAR_POWER_FIREC,
     GROUP_SOLAR_POWER_COST,
     GROUP_SOLAR_POWER_TECH,
     GROUP_SOLAR_POWER_RANGE
);

Construction *SolarPowerConstructionGroup::createConstruction(World& world) {
  return new SolarPower(world, this);
}

SolarPower::SolarPower(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->tech = world.tech_level;
  this->working_days = 0;
  this->busy = 0;
  initialize_commodities();

  commodityMaxCons[STUFF_LABOR] = 100 * SOLAR_POWER_LABOR;
}

void SolarPower::update()
{
    int hivolt_made = (commodityCount[STUFF_HIVOLT] + hivolt_output <= MAX_HIVOLT_AT_SOLARPS)?hivolt_output:MAX_HIVOLT_AT_SOLARPS-commodityCount[STUFF_HIVOLT];
    int labor_used = SOLAR_POWER_LABOR * hivolt_made / hivolt_output;

    if ((commodityCount[STUFF_LABOR] >= labor_used)
     && (hivolt_made >= POWERS_SOLAR_OUTPUT))
    {
        consumeStuff(STUFF_LABOR, labor_used);
        produceStuff(STUFF_HIVOLT, hivolt_made);
        working_days += hivolt_made;
    }
    if(world.total_time % 100 == 99) {
      reset_prod_counters();
      busy = working_days / hivolt_output;
      working_days = 0;
    }
}

void SolarPower::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  mps.add_sfp(N_("busy"), (busy));
  mps.add_sfp(N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
  mps.add_sd(N_("Output"), hivolt_output);
  list_commodities(mps, production);
}

void SolarPower::place(MapPoint point) {
  Construction::place(point);

  this->hivolt_output = (int)(POWERS_SOLAR_OUTPUT +
    (((double)tech * POWERS_SOLAR_OUTPUT) / MAX_TECH_LEVEL));

  commodityMaxProd[STUFF_HIVOLT] = 100 * hivolt_output;
}

void SolarPower::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"tech", xmlFormat<int>(tech));
  Construction::save(xmlWriter);
}

bool SolarPower::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string name = xmlReader.get_name();
  if(name == "tech") tech = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "mwh_output");
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/solar_power.cpp */
