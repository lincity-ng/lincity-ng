/* ---------------------------------------------------------------------- *
 * src/lincity/modules/university.cpp
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

#include "university.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <string>                         // for basic_string, allocator

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/groups.hpp"             // for GROUP_UNIVERSITY, GROUP_SCHOOL
#include "lincity/lin-city.hpp"           // for MAX_TECH_LEVEL, TRUE
#include "lincity/stats.hpp"              // for Stats
#include "lincity/world.hpp"              // for World
#include "util/gettextutil.hpp"           // for N_
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStrF

UniversityConstructionGroup universityConstructionGroup(
    N_("University"),
    N_("Universities"),
     TRUE,                     /* need credit? */
     GROUP_UNIVERSITY,
     GROUP_UNIVERSITY_SIZE,
     GROUP_UNIVERSITY_COLOUR,
     GROUP_UNIVERSITY_COST_MUL,
     GROUP_UNIVERSITY_BUL_COST,
     GROUP_UNIVERSITY_FIREC,
     GROUP_UNIVERSITY_COST,
     GROUP_UNIVERSITY_TECH,
     GROUP_UNIVERSITY_RANGE
);

Construction *UniversityConstructionGroup::createConstruction(World& world) {
  return new University(world, this);
}

bool
UniversityConstructionGroup::can_build(const World& world, Message::ptr& message
) const {
  if(world.stats.groupCount[GROUP_SCHOOL]/4
    - world.stats.groupCount[GROUP_UNIVERSITY] < 1
  ) {
    message = NotEnoughStudentsMessage::create();
    return false;
  }

  return ConstructionGroup::can_build(world, message);
}

University::University(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->working_days = 0;
  this->busy = 0;
  this->total_tech_made = 0;
  initialize_commodities();

  commodityMaxCons[STUFF_LABOR] = 100 * UNIVERSITY_LABOR;
  commodityMaxCons[STUFF_GOODS] = 100 * UNIVERSITY_GOODS;
  commodityMaxProd[STUFF_WASTE] = 100 * (UNIVERSITY_GOODS/3);
}

void University::update() {
  try {
    world.expense(UNIVERSITY_RUNNING_COST, world.stats.expenses.university);
    //do the teaching
    if (commodityCount[STUFF_LABOR] >= UNIVERSITY_LABOR
      &&  commodityCount[STUFF_GOODS] >= UNIVERSITY_GOODS
      &&  commodityCount[STUFF_WASTE] + UNIVERSITY_GOODS / 3 <= MAX_WASTE_AT_UNIVERSITY
    ) {
      consumeStuff(STUFF_LABOR, UNIVERSITY_LABOR);
      consumeStuff(STUFF_GOODS, UNIVERSITY_GOODS);
      produceStuff(STUFF_WASTE, UNIVERSITY_GOODS / 3);
      ++working_days;
      world.tech_level += UNIVERSITY_TECH_MADE;
      total_tech_made += UNIVERSITY_TECH_MADE;
    }
  } catch(const OutOfMoneyMessage::Exception& ex) { }

  //monthly update
  if(world.total_time % 100 == 99) {
    reset_prod_counters();
    busy = working_days;
    working_days = 0;
  }
}

void University::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  mps.add_sfp(N_("busy"), busy);
  mps.add_sfp(N_("Tech researched"), total_tech_made * 100.0 / MAX_TECH_LEVEL);
  list_commodities(mps, production);
}

void University::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"total_tech_made", xmlFormat<int>(total_tech_made));
  Construction::save(xmlWriter);
}

bool University::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string name = xmlReader.get_name();
  if(name == "total_tech_made") total_tech_made = xmlParse<int>(xmlReader.read_inner_xml());
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/university.cpp */
