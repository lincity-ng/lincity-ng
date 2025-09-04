/* ---------------------------------------------------------------------- *
 * src/lincity/modules/waterwell.cpp
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

#include "waterwell.hpp"

#include <string>                   // for basic_string

#include "lincity-ng/Mps.hpp"       // for Mps
#include "lincity/MapPoint.hpp"     // for MapPoint
#include "lincity/groups.hpp"         // for GROUP_WATERWELL
#include "lincity/lin-city.hpp"       // for FLAG_HAS_UNDERGROUND_WATER, FALSE
#include "lincity/world.hpp"          // for World, Map, MapTile
#include "util/gettextutil.hpp"

WaterwellConstructionGroup waterwellConstructionGroup(
  N_("Water Tower"),
  N_("Water Towers"),
  FALSE,                     /* need credit? */
  GROUP_WATERWELL,
  GROUP_WATERWELL_SIZE,
  GROUP_WATERWELL_COLOUR,
  GROUP_WATERWELL_COST_MUL,
  GROUP_WATERWELL_BUL_COST,
  GROUP_WATERWELL_FIREC,
  GROUP_WATERWELL_COST,
  GROUP_WATERWELL_TECH,
  GROUP_WATERWELL_RANGE
);

Construction *WaterwellConstructionGroup::createConstruction(World& world) {
  return new Waterwell(world, this);
}

bool
WaterwellConstructionGroup::can_build_here(const World& world,
  const MapPoint point, Message::ptr& message
) const {
  if(!ConstructionGroup::can_build_here(world, point, message)) return false;

  for(int i = 0; i < size; i++)
  for(int j = 0; j < size; j++)
    if(world.map(point.e(j).s(i))->flags & FLAG_HAS_UNDERGROUND_WATER)
      goto has_ugw;

  message = DesertHereMessage::create(point);
  return false;

  has_ugw:
  return true;
}

Waterwell::Waterwell(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->busy = 0;
  this->working_days = 0;
  initialize_commodities();

  int area = cstgrp->size * cstgrp->size;
  commodityMaxProd[STUFF_WATER] = 100 * WATER_PER_UGW * area;
}

void Waterwell::update() {
  if(commodityCount[STUFF_WATER] + water_output <= MAX_WATER_AT_WATERWELL) {
    working_days++;
    produceStuff(STUFF_WATER, water_output);
  }
  //monthly update
  if(world.total_time % 100 == 99) {
    reset_prod_counters();
    busy = working_days;
    working_days = 0;
  }
}

void Waterwell::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  mps.add_sddp(N_("Fertility"), ugwCount,
    constructionGroup->size * constructionGroup->size);
  mps.add_sfp(N_("busy"), busy);
  mps.add_sddp(N_("Air Pollution"), world.map(point)->pollution,
    MAX_POLLUTION_AT_WATERWELL);
  mps.add_ss(N_("Drinkable"),
    world.map(point)->pollution > MAX_POLLUTION_AT_WATERWELL
      ? N_("No") : N_("Yes"));
  list_commodities(mps, production);
}

void Waterwell::place(MapPoint point) {
  Construction::place(point);

  this->ugwCount = 0;
  for(MapPoint p(point); p.y < point.y + constructionGroup->size; p.y++)
  for(p.x = point.x; p.x < point.x + constructionGroup->size; p.x++)
    if(world.map(p)->flags & FLAG_HAS_UNDERGROUND_WATER)
      this->ugwCount++;
  this->water_output = this->ugwCount * WATER_PER_UGW;
}

/** @file lincity/modules/waterwell.cpp */
