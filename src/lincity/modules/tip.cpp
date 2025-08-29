/* ---------------------------------------------------------------------- *
 * src/lincity/modules/tip.cpp
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

#include "tip.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <list>                           // for _List_iterator
#include <string>                         // for basic_string, operator==

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/groups.hpp"             // for GROUP_TIP
#include "lincity/lin-city.hpp"           // for FALSE, FLAG_NEVER_EVACUATE
#include "lincity/resources.hpp"          // for ExtraFrame
#include "lincity/stats.hpp"              // for Stats
#include "lincity/world.hpp"              // for World
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

TipConstructionGroup tipConstructionGroup(
    N_("Land Fill"),
    N_("Land Fills"),
     FALSE,                     /* need credit? */
     GROUP_TIP,
     GROUP_TIP_SIZE,
     GROUP_TIP_COLOUR,
     GROUP_TIP_COST_MUL,
     GROUP_TIP_BUL_COST,
     GROUP_TIP_FIREC,
     GROUP_TIP_COST,
     GROUP_TIP_TECH,
     GROUP_TIP_RANGE
);

Construction *TipConstructionGroup::createConstruction(World& world) {
  return new Tip(world, this);
}

Tip::Tip(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->flags |= FLAG_NEVER_EVACUATE;
  this->total_waste = 0;
  this->working_days = 0;
  this->busy = 0;
  this->degration_days = 0;
  initialize_commodities();

  commodityMaxCons[STUFF_WASTE] = 100 * WASTE_BURRIED;
  commodityMaxProd[STUFF_WASTE] = 100 * WASTE_BURRIED;
}

void Tip::update()
{
    //the waste is always slowly degrading
    //max degradiation per day is about 42 (10M/240k)
    degration_days += total_waste;
    total_waste -= degration_days / TIP_DEGRADE_TIME;
    degration_days %= TIP_DEGRADE_TIME;

    if ((commodityCount[STUFF_WASTE] >= WASTE_BURRIED)
    && (commodityCount[STUFF_WASTE]*100/TIP_TAKES_WASTE > CRITICAL_WASTE_LEVEL)
    && (total_waste + WASTE_BURRIED <= MAX_WASTE_AT_TIP))
    {
        consumeStuff(STUFF_WASTE, WASTE_BURRIED);
        total_waste += WASTE_BURRIED;
        working_days++;
        world.stats.sustainability.mining_flag = false;
    }
    else if ((commodityCount[STUFF_WASTE] + WASTE_BURRIED <= TIP_TAKES_WASTE)
    && (commodityCount[STUFF_WASTE]*100/TIP_TAKES_WASTE < CRITICAL_WASTE_LEVEL)
    && (total_waste > 0))
    {
        int waste_dug = (WASTE_BURRIED < total_waste)?WASTE_BURRIED:total_waste;
        produceStuff(STUFF_WASTE, waste_dug);
        total_waste -= waste_dug;
        working_days++;
    }
    if(world.total_time % 100 == 99) {
      reset_prod_counters();
      busy = working_days;
      working_days = 0;
    }
}

void Tip::animate(unsigned long real_time) {
  int i = (total_waste /3 * 22) / MAX_WASTE_AT_TIP;
  if (total_waste > 0 && i < 8)
  {   i++;}
  frameIt->frame = i;
}

void Tip::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  mps.add_sfp(N_("busy"), busy);
  mps.add_sd(N_("Waste"), total_waste);
  mps.add_sfp(N_("Filled"), (float)total_waste*100/MAX_WASTE_AT_TIP);
  list_commodities(mps, production);
}

bool
Tip::can_bulldoze(Message::ptr& message) const {
  if(total_waste > 0) {
    message = CannotBulldozeNonemptyTipMessage::create(point);
    return false;
  }
  return Construction::can_bulldoze(message);
}

void Tip::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"total_waste",    xmlFormat<int>(total_waste));
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"degration_days", xmlFormat<int>(degration_days));
  Construction::save(xmlWriter);
}

bool Tip::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string name = xmlReader.get_name();
  if     (name == "total_waste")    total_waste    = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "degration_days") degration_days = xmlParse<int>(xmlReader.read_inner_xml());
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/tip.cpp */
