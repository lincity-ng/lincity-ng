/* ---------------------------------------------------------------------- *
 * src/lincity/modules/health_centre.cpp
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

#include "health_centre.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <algorithm>                      // for max, min
#include <string>                         // for basic_string, operator==

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"             // for GROUP_HEALTH
#include "lincity/lin-city.hpp"           // for FALSE, FLAG_HEALTH_COVER_CHECK
#include "lincity/messages.hpp"           // for OutOfMoneyMessage
#include "lincity/stats.hpp"              // for Stats
#include "lincity/world.hpp"              // for World, Map, MapTile
#include "util/gettextutil.hpp"           // for N_, _
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr

HealthCentreConstructionGroup healthCentreConstructionGroup(
     N_("Hospital"),
     N_("Hospitals"),
     FALSE,                     /* need credit? */
     GROUP_HEALTH,
     GROUP_HEALTH_SIZE,
     GROUP_HEALTH_COLOUR,
     GROUP_HEALTH_COST_MUL,
     GROUP_HEALTH_BUL_COST,
     GROUP_HEALTH_FIREC,
     GROUP_HEALTH_COST,
     GROUP_HEALTH_TECH,
     GROUP_HEALTH_RANGE
);

Construction *HealthCentreConstructionGroup::createConstruction(World& world) {
  return new HealthCentre(world, this);
}

HealthCentre::HealthCentre(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->active = false;
  this->busy = 0;
  this->daycount = 0;
  this->working_days = 0;
  this->covercount = 0;
  initialize_commodities();

  commodityMaxCons[STUFF_LABOR] = 100 * HEALTH_CENTRE_LABOR;
  commodityMaxCons[STUFF_GOODS] = 100 * HEALTH_CENTRE_GOODS;
  commodityMaxProd[STUFF_WASTE] = 100 * (HEALTH_CENTRE_GOODS / 3);
}

void HealthCentre::update() {
  ++daycount;

  try {
    world.expense(HEALTH_RUNNING_COST * (1 +
        HEALTH_RUNNING_COST_MUL * world.tech_level / MAX_TECH_LEVEL),
      world.stats.expenses.health);

    if (commodityCount[STUFF_LABOR] >= HEALTH_CENTRE_LABOR
      &&  commodityCount[STUFF_GOODS] >= HEALTH_CENTRE_GOODS
      &&  commodityCount[STUFF_WASTE] + (HEALTH_CENTRE_GOODS / 3) <= MAX_WASTE_AT_HEALTH_CENTRE
    ) {
      consumeStuff(STUFF_LABOR, HEALTH_CENTRE_LABOR);
      consumeStuff(STUFF_GOODS, HEALTH_CENTRE_GOODS);
      produceStuff(STUFF_WASTE, HEALTH_CENTRE_GOODS / 3);
      ++covercount;
      ++working_days;
    }
  } catch(const OutOfMoneyMessage::Exception& ex) { }

  //TODO implement animation once graphics exist
  if(world.total_time % DAYS_BETWEEN_COVER == 75)
    cover();

  //monthly update
  if(world.total_time % 100 == 99) {
    reset_prod_counters();
    busy = working_days;
    working_days = 0;
  }
}

void HealthCentre::cover()
{
    if(covercount + COVER_TOLERANCE_DAYS < daycount)
    {
        daycount = 0;
        active = false;
        return;
    }
    active = true;
    covercount -= daycount;
    daycount = 0;

    MapPoint nw(
      std::max(point.x - constructionGroup->range, 1),
      std::max(point.y - constructionGroup->range, 1)
    );
    MapPoint se(
      std::min(point.x + constructionGroup->range, world.map.len() - 1),
      std::min(point.y + constructionGroup->range, world.map.len() - 1)
    );
    for(MapPoint p(nw); p.y < se.y; p.y++)
    for(p.x = nw.x; p.x < se.x; p.x++)
      world.map(p)->flags |= FLAG_HEALTH_COVER_CHECK;
}

void HealthCentre::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.add_sfp(N_("busy"), (float) busy);
  list_commodities(mps, production);
  mps.add_ss(N_("Health Care"), active ? _("Yes") : _("No"));
}

void HealthCentre::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"active",     xmlFormat<int>(active));
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"daycount",   xmlFormat<int>(daycount));
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"covercount", xmlFormat<int>(covercount));
  Construction::save(xmlWriter);
}

bool HealthCentre::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  const xmlpp::ustring name = xmlReader.get_name();
  if     (name == "active")     active     = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "daycount")   daycount   = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "covercount") covercount = xmlParse<int>(xmlReader.read_inner_xml());
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/health_centre.cpp */
