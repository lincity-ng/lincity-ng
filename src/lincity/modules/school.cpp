/* ---------------------------------------------------------------------- *
 * src/lincity/modules/school.cpp
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

#include "school.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <stddef.h>                       // for NULL
#include <map>                            // for map
#include <string>                         // for basic_string, allocator

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"             // for GROUP_SCHOOL
#include "lincity/lin-city.hpp"           // for FALSE, MAX_TECH_LEVEL
#include "lincity/messages.hpp"           // for OutOfMoneyMessage
#include "lincity/resources.hpp"          // for ExtraFrame, ResourceGroup
#include "lincity/stats.hpp"              // for Stat, Stats
#include "lincity/world.hpp"              // for World, Map, MapTile
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

SchoolConstructionGroup schoolConstructionGroup(
  N_("Elementary School"),
  N_("Elementary Schools"),
  FALSE,                     /* need credit? */
  GROUP_SCHOOL,
  GROUP_SCHOOL_SIZE,
  GROUP_SCHOOL_COLOUR,
  GROUP_SCHOOL_COST_MUL,
  GROUP_SCHOOL_BUL_COST,
  GROUP_SCHOOL_FIREC,
  GROUP_SCHOOL_COST,
  GROUP_SCHOOL_TECH,
  GROUP_SCHOOL_RANGE
);

Construction *SchoolConstructionGroup::createConstruction(World& world) {
  return new School(world, this);
}

School::School(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  // this->animate_enable = false;
  this->anim = 0;
  this->anim2 = 0;
  this->working_days = 0;
  this->busy = 0;
  this->total_tech_made = 0;
  initialize_commodities();

  commodityMaxCons[STUFF_LABOR] = 100 * LABOR_MAKE_TECH_SCHOOL;
  commodityMaxCons[STUFF_GOODS] = 100 * GOODS_MAKE_TECH_SCHOOL;
  commodityMaxProd[STUFF_WASTE] = 100 * (GOODS_MAKE_TECH_SCHOOL/3);
}

School::~School() {
  if(world.map(point)->framesptr) {
    world.map(point)->framesptr->erase(frit);
    if(world.map(point)->framesptr->empty()) {
      delete world.map(point)->framesptr;
      world.map(point)->framesptr = NULL;
    }
  }
}

void School::update() {
  try {
    world.stats.expenses.school += SCHOOL_RUNNING_COST;

    if (commodityCount[STUFF_LABOR] >= LABOR_MAKE_TECH_SCHOOL
      &&  commodityCount[STUFF_GOODS] >= GOODS_MAKE_TECH_SCHOOL
      &&  commodityCount[STUFF_WASTE] + GOODS_MAKE_TECH_SCHOOL / 3 <= MAX_WASTE_AT_SCHOOL
    ) {
      consumeStuff(STUFF_LABOR, LABOR_MAKE_TECH_SCHOOL);
      consumeStuff(STUFF_GOODS, GOODS_MAKE_TECH_SCHOOL);
      produceStuff(STUFF_WASTE, GOODS_MAKE_TECH_SCHOOL / 3);
      ++working_days;
      world.tech_level += TECH_MADE_BY_SCHOOL;
      total_tech_made += TECH_MADE_BY_SCHOOL;
    }
  } catch(const OutOfMoneyMessage::Exception& ex) { }

  if(world.total_time % 100 == 0) {
    reset_prod_counters();
    busy = working_days;
    working_days = 0;
  }
}

void School::animate(unsigned long real_time) {
  if(real_time >= anim) {
    anim = real_time + SCHOOL_ANIMATION_SPEED;
    int& frame = frameIt->frame;
    int& swing = frit->frame;
    if(frame) {
      if(++swing >= 10) {
        // Do not include last swing frame because it is same as first.
        // anim = real_time + SCHOOL_ANIMATION_BREAK - 100 * busy;
        swing = 0;
      }

      // stop the swing in position 0, 5, or 10
      if((swing == 0 || swing == 5) && (real_time >= anim2 || busy == 0)) {
        anim = real_time + SCHOOL_ANIMATION_BREAK - 100 * busy;
        frame = 0;
        swing = -1;
      }
    }
    else if(busy >= 20) {
      frame = 1;
      swing = 0;
      anim2 = real_time + 100 * busy;
    }
  }
}

void School::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  mps.add_sfp(N_("busy"), (float)busy);
  mps.add_sfp(N_("Lessons learned"), total_tech_made * 100.0 / MAX_TECH_LEVEL);
  list_commodities(mps, production);
}

void School::init_resources() {
  Construction::init_resources();

  frit = world.map(point)->createframe();
  frit->resourceGroup = ResourceGroup::resMap["ChildOnSwing"]; //host of the swing
  frit->frame = -1; //hide the swing
}

void School::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"total_tech_made", xmlFormat<int>(total_tech_made));
  Construction::save(xmlWriter);
}

bool School::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string name = xmlReader.get_name();
  if(name == "total_tech_made") total_tech_made = xmlParse<int>(xmlReader.read_inner_xml());
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/school.cpp */
