/* ---------------------------------------------------------------------- *
 * src/lincity/modules/windmill.cpp
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

#include "windmill.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <list>                           // for _List_iterator
#include <string>                         // for basic_string, operator==

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"             // for GROUP_WINDMILL
#include "lincity/lin-city.hpp"           // for MAX_TECH_LEVEL, ANIM_THRESHOLD
#include "lincity/messages.hpp"           // for OutOfMoneyMessage
#include "lincity/resources.hpp"          // for ExtraFrame
#include "lincity/stats.hpp"              // for Stats
#include "lincity/world.hpp"              // for World
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

WindmillConstructionGroup windmillConstructionGroup(
  N_("Windmill"),
  N_("Windmills"),
  FALSE,                     /* need credit? */
  GROUP_WINDMILL,
  GROUP_WINDMILL_SIZE,
  GROUP_WINDMILL_COLOUR,
  GROUP_WINDMILL_COST_MUL,
  GROUP_WINDMILL_BUL_COST,
  GROUP_WINDMILL_FIREC,
  GROUP_WINDMILL_COST,
  GROUP_WINDMILL_TECH,
  GROUP_WINDMILL_RANGE
);

Construction *WindmillConstructionGroup::createConstruction(World& world) {
  return new Windmill(world, this);
}

Windmill::Windmill(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->anim = 0;
  this->animate_enable = false;
  this->tech = world.tech_level;
  this->working_days = 0;
  this->busy = 0;
  initialize_commodities();

  commodityMaxCons[STUFF_LABOR] = 100 * WINDMILL_LABOR;
  // commodityMaxProd[STUFF_LOVOLT] = 100 * lovolt_output;
}

void Windmill::update() {
  static bool paid = false;
  if(world.total_time % WINDMILL_RCOST == 0)
    paid = false;
  if(!paid)
    try {
      world.expense(1, world.stats.expenses.windmill);
      paid = true;
    } catch (OutOfMoneyMessage ex) { }

  int lovolt_made = (commodityCount[STUFF_LOVOLT] + lovolt_output <= MAX_LOVOLT_AT_WINDMILL)?lovolt_output:MAX_LOVOLT_AT_WINDMILL-commodityCount[STUFF_LOVOLT];
  int labor_used = WINDMILL_LABOR * lovolt_made / lovolt_output;

  if(paid
    && commodityCount[STUFF_LABOR] >= labor_used
    && lovolt_made >= WINDMILL_LOVOLT
  ) {
    consumeStuff(STUFF_LABOR, labor_used);
    produceStuff(STUFF_LOVOLT, lovolt_made);
    animate_enable = true;
    working_days += lovolt_made;
  }
  else {
    animate_enable = false;
  }

  //monthly update
  if(world.total_time % 100 == 99) {
    reset_prod_counters();
    busy = working_days;
    working_days = 0;
  }
}

void Windmill::animate(unsigned long real_time) {
  if(animate_enable && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(ANTIQUE_WINDMILL_ANIM_SPEED);
    ++frameIt->frame %= 3;
  }
}

void Windmill::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.add_sfp(N_("busy"), float(busy) / lovolt_output);
  mps.add_sfp(N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
  mps.add_sd(N_("Output"), lovolt_output);
  list_commodities(mps, production);
}

void Windmill::place(MapPoint point) {
  Construction::place(point);

  this->lovolt_output = (int)(WINDMILL_LOVOLT +
    (((double)tech * WINDMILL_LOVOLT) / MAX_TECH_LEVEL));

  commodityMaxProd[STUFF_LOVOLT] = 100 * lovolt_output;
}

void Windmill::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"tech", xmlFormat<int>(tech));
  Construction::save(xmlWriter);
}

bool Windmill::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string name = xmlReader.get_name();
  if(name == "tech") tech = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "kwh_output");
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/windmill.cpp */
