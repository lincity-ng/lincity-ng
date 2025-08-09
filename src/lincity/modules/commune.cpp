/* ---------------------------------------------------------------------- *
 * src/lincity/modules/commune.cpp
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

#include "commune.hpp"

#include <cstdlib>                          // for rand
#include <list>                             // for _List_iterator
#include <string>                           // for basic_string
#include <vector>                           // for vector

#include "lincity-ng/Mps.hpp"               // for Mps
#include "lincity/MapPoint.hpp"             // for MapPoint
#include "lincity/groups.hpp"               // for GROUP_COMMUNE
#include "lincity/lin-city.hpp"             // for ANIM_THRESHOLD, FALSE
#include "lincity/modules/parkland.hpp"
#include "lincity/resources.hpp"            // for ExtraFrame, ResourceGroup
#include "lincity/world.hpp"                // for World, Map, MapTile
#include "util/gettextutil.hpp"

CommuneConstructionGroup communeConstructionGroup(
    N_("Forest"),
    N_("Forests"),
    FALSE,                     /* need credit? */
    GROUP_COMMUNE,
    GROUP_COMMUNE_SIZE,
    GROUP_COMMUNE_COLOUR,
    GROUP_COMMUNE_COST_MUL,
    GROUP_COMMUNE_BUL_COST,
    GROUP_COMMUNE_FIREC,
    GROUP_COMMUNE_COST,
    GROUP_COMMUNE_TECH,
    GROUP_COMMUNE_RANGE
);

Construction *CommuneConstructionGroup::createConstruction(World& world) {
  return new Commune(world, this);
}

Commune::Commune(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->anim = 0; // or real_time?
  this->animate_enable = false;
  this->steel_made = false;
  this->monthly_stuff_made = 0;
  this->last_month_output = 0;
  this->lazy_months = 0;
  initialize_commodities();

  commodityMaxCons[STUFF_WATER] = 100 *
    constructionGroup->size * constructionGroup->size * WATER_FOREST;
  commodityMaxProd[STUFF_COAL] = 100 * COMMUNE_COAL_MADE;
  commodityMaxProd[STUFF_ORE] = 100 *
    (COMMUNE_ORE_MADE + COMMUNE_ORE_FROM_WASTE);
  commodityMaxCons[STUFF_WASTE] = 100 * COMMUNE_WASTE_GET;
  commodityMaxProd[STUFF_STEEL] = 100 / 20 * COMMUNE_STEEL_MADE;
}

void
Commune::update() {
  int tmpUgwCount = ugwCount;
  int tmpCoalprod = coalprod;
  const unsigned short s = constructionGroup->size;
  const unsigned short a = s*s;
  if(commodityCount[STUFF_WATER]>= (a-ugwCount)*WATER_FOREST) {
    tmpUgwCount = a;
    tmpCoalprod = COMMUNE_COAL_MADE;
    consumeStuff(STUFF_WATER, (a-ugwCount)*WATER_FOREST);
  }
  if(/* (total_time & 1) && */ //make coal every second day
    tmpCoalprod > 0
    && commodityCount[STUFF_COAL] + tmpCoalprod <= MAX_COAL_AT_COMMUNE
  ) {
    produceStuff(STUFF_COAL, tmpCoalprod);
    monthly_stuff_made++;
    animate_enable = true;
  }
  if(commodityCount[STUFF_ORE] + COMMUNE_ORE_MADE <= MAX_ORE_AT_COMMUNE) {
    produceStuff(STUFF_ORE, COMMUNE_ORE_MADE);
    monthly_stuff_made++;
    animate_enable = true;
  }
  /* recycle a bit of waste if there is plenty*/
  if (commodityCount[STUFF_WASTE] >= 3 * COMMUNE_WASTE_GET) {
    consumeStuff(STUFF_WASTE, COMMUNE_WASTE_GET);
    monthly_stuff_made++;
    animate_enable = true;
    if(commodityCount[STUFF_ORE] + COMMUNE_ORE_FROM_WASTE <= MAX_ORE_AT_COMMUNE )
      produceStuff(STUFF_ORE, COMMUNE_ORE_FROM_WASTE);
  }
  if (world.total_time % 10 == 0) {
    int modulus = world.total_time % 20 >= 10 ? 1 : 0;
    for(MapPoint p(point); p.y < point.y + s; p.y++)
    for(p.x = point.x + (p.y + modulus) % 2; p.x < point.x + s; p.x++) {
      int& pol = world.map(p)->pollution;
      if(pol) --pol;
    }
    if(modulus && commodityCount[STUFF_STEEL] + COMMUNE_STEEL_MADE <= MAX_STEEL_AT_COMMUNE) {
      monthly_stuff_made++;
      animate_enable = true;
      steel_made = true;
      produceStuff(STUFF_STEEL, COMMUNE_STEEL_MADE);
    }
  }

  if(world.total_time % 100 == 99) { //each month
    reset_prod_counters();
    last_month_output = monthly_stuff_made;
    monthly_stuff_made = 0;

    if(last_month_output) { //we were busy
      if (lazy_months > 0)
        --lazy_months;
    }
    else { //we are lazy
      lazy_months++;
      /* Communes without production only last 10 years */
      if(lazy_months > 120) {
        turnIntoParks();
        return;
      }
    }
  }
}

void Commune::animate(unsigned long real_time) {
  int& frame = frameIt->frame;
  if(animate_enable && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(COMMUNE_ANIM_SPEED - 25 + (rand() % 50));
    animate_enable = false;

    frame++;
    if(frame == 6 || frame == 11) {
      // animate_enable = false;
      frame -= 5;
    }

    frame += (frame >= 6 ? -5 : 0) + (steel_made ? 5 : 0);
    steel_made = false;

    if(frame >= (int)frameIt->resourceGroup->graphicsInfoVector.size())
      // this should never happen
      frame = 1;
  }
  else if(!monthly_stuff_made && !last_month_output) {
    frame = 0;
  }
}

void Commune::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.add_sddp(N_("Fertility"), ugwCount, constructionGroup->size * constructionGroup->size);
  mps.add_sfp(N_("busy"), (float)last_month_output / 3.05);
  mps.add_sd(N_("Pollution"), world.map(point)->pollution);
  if(lazy_months)
    mps.add_sddp(N_("lazy months"), lazy_months, 120);
  else
    mps.addBlank();
  list_commodities(mps, production);
}

void
Commune::place(MapPoint point) {
  Construction::place(point);

  this->ugwCount = 0;
  for(int i = 0; i < constructionGroup->size; i++)
  for (int j = 0; j < constructionGroup->size; j++)
    if (world.map(point.s(i).e(j))->flags & FLAG_HAS_UNDERGROUND_WATER)
      this->ugwCount++;

  if (this->ugwCount < 16 / 3)
  {   this->coalprod = COMMUNE_COAL_MADE/3;}
  else if (this->ugwCount < (2 * 16) / 3)
  {   this->coalprod = COMMUNE_COAL_MADE/2;}
  else
  {   this->coalprod = COMMUNE_COAL_MADE;}
}

void
Commune::turnIntoParks() {
  detach();

  unsigned short size = constructionGroup->size;
  for(unsigned short i = 0; i < size; ++i)
  for(unsigned short j = 0; j < size; ++j) {
    MapPoint p(point.s(i).e(j));
    if(world.map(p)->flags & FLAG_HAS_UNDERGROUND_WATER)
      parklandConstructionGroup.placeItem(world, p);
  }
}

/** @file lincity/modules/commune.cpp */
