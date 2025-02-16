/* ---------------------------------------------------------------------- *
 * src/lincity/modules/waterwell.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2024 David Bears <dbear4q@gmail.com>
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

#include "waterwell.h"

#include "modules.h"

WaterwellConstructionGroup waterwellConstructionGroup(
  N_("Water tower"),
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

void Waterwell::report() {
  int i = 0;

  const char *p;

  mps_store_title(i, constructionGroup->name);
  i++;
  mps_store_sddp(i++, N_("Fertility"), ugwCount,
    constructionGroup->size * constructionGroup->size);
  mps_store_sfp(i++, N_("busy"), busy);
  mps_store_sddp(i++, N_("Air Pollution"), world.map(x,y)->pollution,
    MAX_POLLUTION_AT_WATERWELL);
  p = world.map(x,y)->pollution>MAX_POLLUTION_AT_WATERWELL?N_("No"):N_("Yes");
  mps_store_ss(i++, N_("Drinkable"), p);
  list_commodities(&i);
}

void Waterwell::place(int x, int y) {
  Construction::place(x, y);

  this->ugwCount = 0;
  for(int yy = y; yy < y + constructionGroup->size; yy++)
  for(int xx = x; xx < x + constructionGroup->size; xx++)
    if(world.map(xx, yy)->flags & FLAG_HAS_UNDERGROUND_WATER)
      this->ugwCount++;
  this->water_output = this->ugwCount * WATER_PER_UGW;
}

/** @file lincity/modules/waterwell.cpp */
