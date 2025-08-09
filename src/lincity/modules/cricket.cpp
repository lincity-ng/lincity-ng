/* ---------------------------------------------------------------------- *
 * src/lincity/modules/cricket.cpp
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

#include "cricket.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <algorithm>                      // for max, min
#include <list>                           // for _List_iterator
#include <string>                         // for basic_string, operator==
#include <vector>                         // for vector

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"             // for GROUP_CRICKET
#include "lincity/lin-city.hpp"           // for ANIM_THRESHOLD, FALSE, FLAG...
#include "lincity/messages.hpp"           // for OutOfMoneyMessage
#include "lincity/resources.hpp"          // for GraphicsInfo, ExtraFrame
#include "lincity/stats.hpp"              // for Stats
#include "lincity/world.hpp"              // for World, Map, MapTile
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

// cricket place:
CricketConstructionGroup cricketConstructionGroup(
    N_("Basketball Court"),
    N_("Basketball Courts"),
     FALSE,                     /* need credit? */
     GROUP_CRICKET,
     GROUP_CRICKET_SIZE,
     GROUP_CRICKET_COLOUR,
     GROUP_CRICKET_COST_MUL,
     GROUP_CRICKET_BUL_COST,
     GROUP_CRICKET_FIREC,
     GROUP_CRICKET_COST,
     GROUP_CRICKET_TECH,
     GROUP_CRICKET_RANGE
);

Construction *CricketConstructionGroup::createConstruction(World& world) {
  return new Cricket(world, this);
}

Cricket::Cricket(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->anim = 0;
  this->animate_enable = false;
  this->active = false;
  this->busy = 0;
  this->daycount = 0;
  this->working_days = 0;
  this->covercount = 0;
  initialize_commodities();

  commodityMaxCons[STUFF_LABOR] = 100 * CRICKET_LABOR;
  commodityMaxCons[STUFF_GOODS] = 100 * CRICKET_GOODS;
  commodityMaxProd[STUFF_WASTE] = 100 * (CRICKET_GOODS / 3);
}

void Cricket::update()
{
    ++daycount;
    try {
      world.expense(CRICKET_RUNNING_COST, world.stats.expenses.cricket);

      if(commodityCount[STUFF_LABOR] >= CRICKET_LABOR
        &&  commodityCount[STUFF_GOODS] >= CRICKET_GOODS
        &&  commodityCount[STUFF_WASTE] + (CRICKET_GOODS / 3) <= MAX_WASTE_AT_CRICKET
      ) {
        consumeStuff(STUFF_LABOR, CRICKET_LABOR);
        consumeStuff(STUFF_GOODS, CRICKET_GOODS);
        produceStuff(STUFF_WASTE, CRICKET_GOODS / 3);
        ++covercount;
        ++working_days;
      }
    } catch(OutOfMoneyMessage::Exception& ex) { }

    if(world.total_time % DAYS_BETWEEN_COVER == 75)
      cover();

    //monthly update
    if(world.total_time % 100 == 99) {
      reset_prod_counters();
      busy = working_days;
      working_days = 0;
    }
}

void Cricket::cover()
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
    animate_enable = true;

    int xs = std::max(point.x - constructionGroup->range, 1);
    int xe = std::min(point.x + constructionGroup->range, world.map.len() - 1);
    int ys = std::max(point.y - constructionGroup->range, 1);
    int ye = std::min(point.y + constructionGroup->range, world.map.len() - 1);
    for(int yy = ys; yy < ye; ++yy)
      for(int xx = xs; xx < xe; ++xx)
        world.map(MapPoint(xx,yy))->flags |= FLAG_CRICKET_COVER_CHECK;
}

void Cricket::animate(unsigned long real_time) {
  int& frame = frameIt->frame;
  if(animate_enable && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(CRICKET_ANIMATION_SPEED);
    if(++frame >= (int)frameIt->resourceGroup->graphicsInfoVector.size())
    {
      frame = 0;
      animate_enable = false;
    }
  }
}

void Cricket::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.add_sfp(N_("busy"), busy);
  list_commodities(mps, production);
  mps.add_ss(N_("Public sports"), active ? N_("Yes") : N_("No"));
}

void Cricket::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"active",     xmlFormat<int>(active));
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"daycount",   xmlFormat<int>(daycount));
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"covercount", xmlFormat<int>(covercount));
  Construction::save(xmlWriter);
}

bool
Cricket::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string name = xmlReader.get_name();
  if(name == "active") active = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "daycount") daycount = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "covercount") covercount = xmlParse<int>(xmlReader.read_inner_xml());
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/cricket.cpp */
