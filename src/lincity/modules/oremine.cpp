/* ---------------------------------------------------------------------- *
 * src/lincity/modules/oremine.cpp
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

#include "oremine.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <list>                           // for _List_iterator
#include <string>                         // for basic_string, allocator

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"             // for GROUP_OREMINE, GROUP_WATER
#include "lincity/lin-city.hpp"           // for ANIM_THRESHOLD, FALSE, FLAG...
#include "lincity/resources.hpp"          // for ExtraFrame
#include "lincity/stats.hpp"              // for Stats
#include "lincity/world.hpp"              // for World, Map, MapTile
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

OremineConstructionGroup oremineConstructionGroup(
     N_("Ore Mine"),
     N_("Ore Mines"),
     FALSE,                     /* need credit? */
     GROUP_OREMINE,
     GROUP_OREMINE_SIZE,
     GROUP_OREMINE_COLOUR,
     GROUP_OREMINE_COST_MUL,
     GROUP_OREMINE_BUL_COST,
     GROUP_OREMINE_FIREC,
     GROUP_OREMINE_COST,
     GROUP_OREMINE_TECH,
     GROUP_OREMINE_RANGE
);

Construction *OremineConstructionGroup::createConstruction(World& world) {
  return new Oremine(world, this);
}

bool
OremineConstructionGroup::can_build_here(const World& world,
  const MapPoint point, Message::ptr& message
) const {
  if(!ConstructionGroup::can_build_here(world, point, message)) return false;

  int total_ore = 0;
  for(int i = 0; i < size; i++)
  for(int j = 0; j < size; j++)
    total_ore += world.map(point.e(j).s(i))->ore_reserve;
  if(total_ore < MIN_ORE_RESERVE_FOR_MINE) {
    message = NoOreMessage::create();
    return false;
  }

  return true;
}

Oremine::Oremine(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->anim = 0;
  this->animate_enable = false;
  this->working_days = 0;
  this->busy = 0;
  this->anim_count = 0;
  // this->days_offset = 0;
  initialize_commodities();

  commodityMaxProd[STUFF_ORE] = 100 * ORE_PER_RESERVE;
  commodityMaxCons[STUFF_ORE] = 100 * ORE_PER_RESERVE;
  commodityMaxCons[STUFF_LABOR] = 100 * OREMINE_LABOR;
}

void Oremine::update()
{
    animate_enable = false;

    // see if we can/need to extract some underground ore
    if ((total_ore_reserve)
    && (commodityCount[STUFF_ORE] <= ORE_LEVEL_TARGET * (MAX_ORE_AT_MINE - ORE_PER_RESERVE)/100)
    && (commodityCount[STUFF_LABOR] >= OREMINE_LABOR))
    {
      for(MapPoint p(point); p.y < point.y + constructionGroup->size; p.y++)
      for(p.x = point.x; p.x < point.x + constructionGroup->size; p.x++) {
        if(world.map(p)->ore_reserve > 0) {
          world.map(p)->ore_reserve--;
          total_ore_reserve--;
          produceStuff(STUFF_ORE, ORE_PER_RESERVE);
          consumeStuff(STUFF_LABOR, OREMINE_LABOR);
          //FIXME ore_tax should be handled upon delivery
          //ore_made += ORE_PER_RESERVE;
          if(total_ore_reserve <
            (constructionGroup->size * constructionGroup->size * ORE_RESERVE)
          )
            world.stats.sustainability.mining_flag = false;
          animate_enable = true;
          working_days++;
          goto end_mining;
        }
      }
    }
    // return the ore to ore_reserve if there is enough sustainable ore available
    else if ((commodityCount[STUFF_ORE] - ORE_PER_RESERVE > ORE_LEVEL_TARGET * (MAX_ORE_AT_MINE )/100)
    && (commodityCount[STUFF_LABOR] >= LABOR_DIG_ORE))
    {
      for(MapPoint p(point); p.y < point.y + constructionGroup->size; p.y++)
      for(p.x = point.x; p.x < point.x + constructionGroup->size; p.x++) {
        if(world.map(p)->ore_reserve < 3 * ORE_RESERVE / 2) {
          world.map(p)->ore_reserve++;
          total_ore_reserve++;
          consumeStuff(STUFF_ORE, ORE_PER_RESERVE);
          consumeStuff(STUFF_LABOR, OREMINE_LABOR);
          animate_enable = true;
          working_days++;
          goto end_mining;
        }
      }
    }
    end_mining:

    //Monthly update of activity
    if (world.total_time % 100 == 99) {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }

    //Evacuate Mine if no more deposits
    if ( total_ore_reserve == 0 )
    {   flags |= FLAG_EVACUATE;}

    //Abandon the Oremine if it is really empty
    if(total_ore_reserve == 0
      && commodityCount[STUFF_LABOR] == 0
      && commodityCount[STUFF_ORE] == 0
    ) {
      detach();
      makeLake();
      return;
    }
}

void Oremine::animate(unsigned long real_time) {
  int& frame = frameIt->frame;

  if(animate_enable && real_time >= anim) {
    //faster animation for more active mines
    anim = real_time + ANIM_THRESHOLD((14 - busy/11) * OREMINE_ANIMATION_SPEED);
    if(anim_count < 8)
      frame = anim_count;
    else if (anim_count < 12)
      frame = 14 - anim_count;
    else
      frame = 16 - anim_count;
    if(++anim_count == 16)
      anim_count = 0;
  }
}

void Oremine::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.add_sfp(N_("busy"), busy);
  mps.add_sddp(N_("Deposits"), total_ore_reserve, (constructionGroup->size * constructionGroup->size * ORE_RESERVE));
  list_commodities(mps, production);
}

void
Oremine::place(MapPoint point) {
  Construction::place(point);

  int ore = 0;
  for(MapPoint p(point); p.y < point.y + constructionGroup->size; p.y++)
  for(p.x = point.x; p.x < point.x + constructionGroup->size; p.x++)
    ore += world.map(p)->ore_reserve;
  if(ore < 1)
    ore = 1;
  this->total_ore_reserve = ore;
}

void
Oremine::bulldoze() {
  Construction::bulldoze();
  makeLake();
}

void
Oremine::makeLake() {
  int size = constructionGroup->size;
  for(MapPoint p(point); p.y < point.y + size; p.y++)
  for(p.x = point.x; p.x < point.x + size; p.x++) {
    MapTile& tile = *world.map(p);
    if(tile.ore_reserve < ORE_RESERVE / 2) {
      tile.setTerrain(GROUP_WATER);
      tile.flags |= FLAG_HAS_UNDERGROUND_WATER;
      world.map.connect_rivers(p.x, p.y);
    }
  }
  world.map.desert_water_frontiers(point, point.e(size).s(size));
}

void Oremine::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"total_ore_reserve", xmlFormat<int>(total_ore_reserve));
  Construction::save(xmlWriter);
}

bool Oremine::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string tag = xmlReader.get_name();
  if(tag == "total_ore_reserve") total_ore_reserve = xmlParse<int>(xmlReader.read_inner_xml());
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/oremine.cpp */
