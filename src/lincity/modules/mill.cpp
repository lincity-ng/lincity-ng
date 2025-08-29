/* ---------------------------------------------------------------------- *
 * src/lincity/modules/mill.cpp
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

#include "mill.hpp"

#include <list>                     // for _List_iterator
#include <string>                   // for basic_string
#include <vector>                   // for vector

#include "lincity-ng/Mps.hpp"       // for Mps
#include "lincity/MapPoint.hpp"     // for MapPoint
#include "lincity/groups.hpp"       // for GROUP_MILL
#include "lincity/lin-city.hpp"     // for ANIM_THRESHOLD, FALSE
#include "lincity/resources.hpp"    // for ExtraFrame, ResourceGroup
#include "lincity/world.hpp"        // for World, Map, MapTile
#include "util/gettextutil.hpp"

MillConstructionGroup millConstructionGroup(
    N_("Textile Mill"),
    N_("Textile Mills"),
    FALSE,                     /* need credit? */
    GROUP_MILL,
    GROUP_MILL_SIZE,
    GROUP_MILL_COLOUR,
    GROUP_MILL_COST_MUL,
    GROUP_MILL_BUL_COST,
    GROUP_MILL_FIREC,
    GROUP_MILL_COST,
    GROUP_MILL_TECH,
    GROUP_MILL_RANGE
);

Construction *MillConstructionGroup::createConstruction(World& world) {
  return new Mill(world, this);
}

Mill::Mill(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->anim = 0;
  this->busy = 0;
  this->working_days = 0;
  this->animate_enable = false;
  this->pol_count = 0;
  initialize_commodities();

  commodityMaxCons[STUFF_COAL] = 100 * COAL_USED_BY_MILL;
  commodityMaxCons[STUFF_LOVOLT] = 100 *
    COAL_USED_BY_MILL * MILL_POWER_PER_COAL;
  commodityMaxCons[STUFF_FOOD] = 100 * FOOD_USED_BY_MILL;
  commodityMaxCons[STUFF_LABOR] = 100 * MILL_LABOR;
  commodityMaxProd[STUFF_GOODS] = 100 * GOODS_MADE_BY_MILL;
}

void Mill::update()
{
    bool use_coal = (commodityCount[STUFF_COAL]*MAX_LOVOLT_AT_MILL > commodityCount[STUFF_LOVOLT]*MAX_COAL_AT_MILL);
    if ((use_coal?commodityCount[STUFF_COAL]:commodityCount[STUFF_LOVOLT]) >= (use_coal?COAL_USED_BY_MILL:COAL_USED_BY_MILL * MILL_POWER_PER_COAL)
    && (commodityCount[STUFF_FOOD] >= FOOD_USED_BY_MILL)
    && (commodityCount[STUFF_LABOR] >= MILL_LABOR)
    && (commodityCount[STUFF_GOODS] <= MAX_GOODS_AT_MILL - GOODS_MADE_BY_MILL))
    {
        if(use_coal)
            consumeStuff(STUFF_COAL, COAL_USED_BY_MILL);
        else
            consumeStuff(STUFF_LOVOLT, COAL_USED_BY_MILL * MILL_POWER_PER_COAL);
        consumeStuff(STUFF_FOOD, FOOD_USED_BY_MILL);
        consumeStuff(STUFF_LABOR, MILL_LABOR);
        produceStuff(STUFF_GOODS, GOODS_MADE_BY_MILL);
        ++working_days;
        animate_enable = true;
        if((++pol_count %= 7) == 0)
          world.map(point)->pollution++;
    }

    //monthly update
    if(world.total_time % 100 == 99) {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }
}

void Mill::animate(unsigned long real_time) {
  int& frame = frameIt->frame;
  if(animate_enable && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(MILL_ANIM_SPEED);
    animate_enable = false;
    if(++frame >= (int)frameIt->resourceGroup->graphicsInfoVector.size())
      frame = 1;
  }
  else if(!busy) {
    frame = 0;
  }
}

void Mill::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.add_sfp(N_("busy"), (float) busy);
  list_commodities(mps, production);
}


/** @file lincity/modules/mill.cpp */
