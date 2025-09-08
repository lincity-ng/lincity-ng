/* ---------------------------------------------------------------------- *
 * src/lincity/modules/pottery.cpp
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

#include "pottery.hpp"

#include <list>                     // for _List_iterator
#include <string>                   // for basic_string
#include <vector>                   // for vector

#include "lincity-ng/Mps.hpp"       // for Mps
#include "lincity/MapPoint.hpp"     // for MapPoint
#include "lincity/groups.hpp"       // for GROUP_POTTERY
#include "lincity/lin-city.hpp"     // for ANIM_THRESHOLD, FALSE
#include "lincity/resources.hpp"    // for ExtraFrame, ResourceGroup
#include "lincity/world.hpp"        // for World, Map, MapTile
#include "util/gettextutil.hpp"

PotteryConstructionGroup potteryConstructionGroup(
    N_("Pottery"),
    N_("Potteries"),
    FALSE,                     /* need credit? */
    GROUP_POTTERY,
    GROUP_POTTERY_SIZE,
    GROUP_POTTERY_COLOUR,
    GROUP_POTTERY_COST_MUL,
    GROUP_POTTERY_BUL_COST,
    GROUP_POTTERY_FIREC,
    GROUP_POTTERY_COST,
    GROUP_POTTERY_TECH,
    GROUP_POTTERY_RANGE
);

Construction *PotteryConstructionGroup::createConstruction(World& world) {
  return new Pottery(world, this);
}

Pottery::Pottery(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->anim = 0; // or real_time?
  this->pauseCounter = 0;
  this->busy = 0;
  this->working_days = 0;
  this->animate_enable = false;
  initialize_commodities();

  commodityMaxProd[STUFF_GOODS] = 100 * POTTERY_MADE_GOODS;
  commodityMaxCons[STUFF_ORE] = 100 * POTTERY_ORE_MAKE_GOODS;
  commodityMaxCons[STUFF_COAL] = 100 * POTTERY_COAL_MAKE_GOODS;
  commodityMaxCons[STUFF_LABOR] = 100 * POTTERY_LABOR;
}

void Pottery::update()
{
    if(world.total_time % 100 == 0) {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }

    if (pauseCounter++ < 0)
    {   return;}

    if ((commodityCount[STUFF_GOODS] + POTTERY_MADE_GOODS <= MAX_GOODS_AT_POTTERY)
     && (commodityCount[STUFF_ORE] >= POTTERY_ORE_MAKE_GOODS)
     && (commodityCount[STUFF_COAL] >= POTTERY_COAL_MAKE_GOODS)
     && (commodityCount[STUFF_LABOR] >= POTTERY_LABOR))
    {
        produceStuff(STUFF_GOODS, POTTERY_MADE_GOODS);
        consumeStuff(STUFF_ORE, POTTERY_ORE_MAKE_GOODS);
        consumeStuff(STUFF_COAL, POTTERY_COAL_MAKE_GOODS);
        consumeStuff(STUFF_LABOR, POTTERY_LABOR);

        animate_enable = true;
        if(!((working_days++)%10))
          world.map(point)->pollution++;
    }
    else
    {
        animate_enable = false;
        pauseCounter = -POTTERY_CLOSE_TIME;
        return;
    }
}

void Pottery::animate(unsigned long real_time) {
  int& frame = frameIt->frame;
  if (animate_enable && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(POTTERY_ANIM_SPEED);
    if(++frame >= (int)frameIt->resourceGroup->graphicsInfoVector.size()) {
      frame = 1;
      animate_enable = false;
    }
  }
  else if(!busy) {
    frame = 0;
  }
}

void Pottery::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  mps.add_sfp(N_("busy"), (float) busy);
  list_commodities(mps, production);
}

/** @file lincity/modules/pottery.cpp */
