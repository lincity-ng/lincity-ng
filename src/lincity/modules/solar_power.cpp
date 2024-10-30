/* ---------------------------------------------------------------------- *
 * src/lincity/modules/solar_power.cpp
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

#include "solar_power.h"

#include "modules.h"


// SolarPower:
SolarPowerConstructionGroup solarPowerConstructionGroup(
    N_("Solar Power Plant"),
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

Construction *SolarPowerConstructionGroup::createConstruction() {
  return new SolarPower(this);
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
    if (total_time % 100 == 99) //monthly update
    {
        reset_prod_counters();
        busy = working_days / hivolt_output;
        working_days = 0;
    }
}

void SolarPower::report()
{
    int i = 0;

    mps_store_title(i, constructionGroup->name);
    i++;
    mps_store_sfp(i++, N_("busy"), (busy));
    mps_store_sfp(i++, N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, N_("Output"), hivolt_output);
    // i++;
    list_commodities(&i);
}

void SolarPower::save(xmlTextWriterPtr xmlWriter) {
  xmlTextWriterWriteFormatElement(xmlWriter, "tech", "%d", tech);
}

bool SolarPower::loadMember(xmlpp::TextReader& xmlReader) {
  std::string name = xmlReader.get_name();
  if(name == "tech") tech = std::stoi(xmlReader.get_inner_xml());
  if(name == "mwh_output");
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/solar_power.cpp */
