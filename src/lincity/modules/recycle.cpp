/* ---------------------------------------------------------------------- *
 * src/lincity/modules/recycle.cpp
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

#include "recycle.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <string>                         // for basic_string, operator==

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"             // for GROUP_RECYCLE
#include "lincity/lin-city.hpp"           // for MAX_TECH_LEVEL, FALSE
#include "lincity/messages.hpp"           // for OutOfMoneyMessage
#include "lincity/stats.hpp"              // for Stats
#include "lincity/world.hpp"              // for World
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

RecycleConstructionGroup recycleConstructionGroup(
    N_("Recycling Center"),
    N_("Recycling Centers"),
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

Construction *RecycleConstructionGroup::createConstruction(World& world) {
  return new Recycle(world, this);
}

Recycle::Recycle(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->busy = 0;
  this->working_days = 0;
  this->tech = world.tech_level;
  initialize_commodities();

  commodityMaxCons[STUFF_LABOR] = 100 * RECYCLE_LABOR;
  commodityMaxCons[STUFF_LOVOLT] = 100 * LOVOLT_RECYCLE_WASTE;
  commodityMaxCons[STUFF_WASTE] = 100 *
    (WASTE_RECYCLED + BURN_WASTE_AT_RECYCLE);
  // commodityMaxProd[STUFF_ORE] = 100 * make_ore;
  // commodityMaxProd[STUFF_STEEL] = 100 * make_steel;
}

void Recycle::update() {
  try {
    world.expense(RECYCLE_RUNNING_COST, world.stats.expenses.recycle);

    // always recycle waste and only make steel & ore if there are free capacities
    if (commodityCount[STUFF_WASTE] >= WASTE_RECYCLED
      && commodityCount[STUFF_LOVOLT] >= LOVOLT_RECYCLE_WASTE
      && commodityCount[STUFF_LABOR] >= RECYCLE_LABOR
    ) {
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
  } catch(const OutOfMoneyMessage::Exception& ex) { }

  // monthly update
  if(world.total_time % 100 == 99) {
    reset_prod_counters();
    busy = working_days;
    working_days = 0;
  }
  // if we've still >90% waste in stock, burn some waste cleanly.
  if (commodityCount[STUFF_WASTE] > (MAX_WASTE_AT_RECYCLE * 9 / 10))
  {   consumeStuff(STUFF_WASTE, BURN_WASTE_AT_RECYCLE);}
}

void Recycle::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  mps.add_sfp(N_("Tech"), tech * 100.0f / MAX_TECH_LEVEL);
  mps.add_sfp(N_("Efficiency Ore"), (float) make_ore * 100 / WASTE_RECYCLED);
  mps.add_sfp(N_("Efficiency Steel"),(float) make_steel * 100 / WASTE_RECYCLED);
  mps.add_sfp(N_("busy"), busy);
  list_commodities(mps, production);
}

void Recycle::place(MapPoint point) {
  Construction::place(point);

  int efficiency =
    (WASTE_RECYCLED * (10 + ((50 * tech) / MAX_TECH_LEVEL))) / 100;
  if (efficiency > (WASTE_RECYCLED * 8) / 10)
    efficiency = (WASTE_RECYCLED * 8) / 10;
  this->make_ore = efficiency;
  this->make_steel = efficiency / 50;

  commodityMaxProd[STUFF_ORE] = 100 * make_ore;
  commodityMaxProd[STUFF_STEEL] = 100 * make_steel;
}

void Recycle::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"tech", xmlFormat<int>(tech));
  Construction::save(xmlWriter);
}

bool Recycle::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string name = xmlReader.get_name();
  if(name == "tech") tech = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "make_ore");
  else if(name == "make_steel");
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/recycle.cpp */
