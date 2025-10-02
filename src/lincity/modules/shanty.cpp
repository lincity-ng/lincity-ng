/* ---------------------------------------------------------------------- *
 * src/lincity/modules/shanty.cpp
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

#include "shanty.hpp"

#include <stdio.h>                 // for fprintf, stderr, printf
#include <stdlib.h>                // for rand
#include <cassert>                 // for assert
#include <map>                     // for map
#include <optional>                // for optional, operator==
#include <random>                  // for bernoulli_distribution
#include <string>                  // for basic_string, operator<
#include <vector>                  // for vector

#include "commune.hpp"             // for CommuneConstructionGroup, COMMUNE_POP
#include "fire.hpp"                // for Fire, FireConstructionGroup, FIRE_...
#include "lincity-ng/Mps.hpp"      // for Mps
#include "lincity/MapPoint.hpp"    // for MapPoint
#include "lincity/groups.hpp"      // for GROUP_SHANTY, GROUP_MARKET
#include "lincity/lin-city.hpp"    // for ANIM_THRESHOLD, FALSE, FLAG_FIRE_C...
#include "lincity/messages.hpp"    // for FireStartedMessage
#include "lincity/resources.hpp"   // for ExtraFrame, GraphicsInfo, Resource...
#include "lincity/stats.hpp"       // for Stat, Stats
#include "lincity/world.hpp"       // for World, Map, MapTile
#include "modules_interfaces.hpp"  // for add_a_shanty, update_shanty
#include "util/gettextutil.hpp"    // for N_
#include "util/randutil.hpp"       // for BasicUrbg

ShantyConstructionGroup shantyConstructionGroup(
    N_("Shanty Town"),
    N_("Shanty Towns"),
     FALSE,                     /* need credit? */
     GROUP_SHANTY,
     GROUP_SHANTY_SIZE,
     GROUP_SHANTY_COLOUR,
     GROUP_SHANTY_COST_MUL,
     GROUP_SHANTY_BUL_COST,
     GROUP_SHANTY_FIREC,
     GROUP_SHANTY_COST,
     GROUP_SHANTY_TECH,
     GROUP_SHANTY_RANGE
);

Construction *ShantyConstructionGroup::createConstruction(World& world) {
  return new Shanty(world, this);
}

//TODO remove_a_shanty() and update_shanty() should go to ConstructionRequest

void add_a_shanty(World& world) {
    MapPoint p;
    std::optional<MapPoint> r;
    int numof_shanties = world.stats.groupCount[GROUP_SHANTY];
    const int len = world.map.len();
    p.x = rand() % len;
    p.y = rand() % len;
    if (numof_shanties > 0 && rand() % 8 != 0)
    {
        r = world.map.find_group(p, GROUP_SHANTY);
        if (!r) {
            printf("Looked for a shanty, without any! x=%d y=%d\n", p.x, p.y);
            return;
        }
        p = *r;
        r = world.map.find_bare_area(p, 2);
        if (r == -1)
        {
#ifdef DEBUG
            fprintf(stderr,"Adding a shanty (s), no space for it?!\n");
#endif
            return;
        }
        p = *r;
    }
    else
    {
        r = world.map.find_group(p, GROUP_MARKET);
        if (!r)
            return;             /* silently return, we havn't started yet. */

        p = *r;
        r = world.map.find_bare_area(p, 2);
        if (r == -1)
        {
#ifdef DEBUG
            fprintf(stderr,"Adding a shanty (r), no space for it?!\n");
#endif
            return;
        }
        p = *r;
    }
    shantyConstructionGroup.placeItem(world, p);
}

void update_shanty(World& world) {
  int numof_communes = world.stats.groupCount[GROUP_COMMUNE];
  int numof_shanties = world.stats.groupCount[GROUP_SHANTY];
  const int len = world.map.len();
  //Foersts make new people? Why not
  //people_pool += .3 * numof_communes;
  int pp = world.people_pool;
  world.people_pool -= 5 * numof_shanties;
  if (world.people_pool < 0)
  {   world.people_pool = 0;  }
  world.stats.population.deaths_m += (pp - world.people_pool);
  pp = world.people_pool - (COMMUNE_POP * numof_communes);
  int i = (pp - SHANTY_MIN_PP) / SHANTY_POP;
  if(i < 0)
    i = 0;
  if(i > numof_shanties) {
    for(int n = 0; n < 1 + (i - numof_shanties)/10; n++)
      add_a_shanty(world);
  }
  else if(numof_shanties > 0 && i < numof_shanties - 1) {
    for(int n = 0; n < 1 + (numof_shanties - i) / 10; n++) {
      MapPoint p(rand() % len, rand() % len);
      std::optional<MapPoint> r = world.map.find_group(p, GROUP_SHANTY);
      if(!r) {
        fprintf(stderr, "Can't find a shanty to remove!\n");
        return;
      }
      Shanty *shanty = dynamic_cast<Shanty *>(
        world.map(*r)->reportingConstruction);
      assert(shanty);
      shanty->detach();
      shanty->makeFire();
    }
  }
}

Shanty::Shanty(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  initialize_commodities();
  this->flags |= FLAG_NEVER_EVACUATE;
  this->anim = 0;
  this->start_burning_waste = false;
  this->waste_fire_anim = 0;

  commodityMaxProd[STUFF_WASTE] = 100 *
    (SHANTY_PUT_WASTE * 2 + SHANTY_GET_GOODS / 3);
  commodityMaxCons[STUFF_FOOD] = 100 * SHANTY_GET_FOOD;
  commodityMaxCons[STUFF_LABOR] = 100 * SHANTY_GET_LABOR;
  commodityMaxCons[STUFF_GOODS] = 100 * SHANTY_GET_GOODS;
  commodityMaxCons[STUFF_COAL] = 100 * SHANTY_GET_COAL;
  commodityMaxCons[STUFF_ORE] = 100 * SHANTY_GET_ORE;
  commodityMaxCons[STUFF_STEEL] = 100 * SHANTY_GET_STEEL;
  commodityMaxCons[STUFF_WASTE] = 100 *
    (MAX_WASTE_AT_SHANTY /*+ SHANTY_PUT_WASTE*2 + SHANTY_GET_GOODS/3*/);
}

Shanty::~Shanty() {
  world.map(point)->killframe(waste_fire_frit);
}

void Shanty::update()
{
    //steal stuff and make waste
    produceStuff(STUFF_WASTE, SHANTY_PUT_WASTE);
    if (commodityCount[STUFF_FOOD] >= SHANTY_GET_FOOD)
    {   consumeStuff(STUFF_FOOD, SHANTY_GET_FOOD);}
    if (commodityCount[STUFF_LABOR] >= SHANTY_GET_LABOR)
    {
        consumeStuff(STUFF_LABOR, SHANTY_GET_LABOR);
        if ((world.taxable.labor -= SHANTY_GET_LABOR * 2) < 0)
        {   world.taxable.labor = 0;}
    }
    if (commodityCount[STUFF_GOODS] >= SHANTY_GET_GOODS)
    {
        consumeStuff(STUFF_GOODS, SHANTY_GET_GOODS);
        produceStuff(STUFF_WASTE, SHANTY_GET_GOODS / 3);
        if ((world.taxable.goods -= SHANTY_GET_GOODS * 2) < 0)
        {   world.taxable.goods = 0;}
    }
    if (commodityCount[STUFF_COAL] >= SHANTY_GET_COAL)
    {
        consumeStuff(STUFF_COAL, SHANTY_GET_COAL);
        if ((world.taxable.coal -= SHANTY_GET_COAL * 2) < 0)
        {   world.taxable.coal = 0;}
    }
    if (commodityCount[STUFF_ORE] >= SHANTY_GET_ORE)
    {   consumeStuff(STUFF_ORE, SHANTY_GET_ORE);}
    if (commodityCount[STUFF_STEEL] >= SHANTY_GET_STEEL)
    {   consumeStuff(STUFF_STEEL, SHANTY_GET_STEEL);}
    produceStuff(STUFF_WASTE, SHANTY_PUT_WASTE);
    if (commodityCount[STUFF_WASTE] >= MAX_WASTE_AT_SHANTY) {
        world.map(point.s().e())->pollution += commodityCount[STUFF_WASTE];
        levelStuff(STUFF_WASTE, 0);
        start_burning_waste = true;

        double uncontrolledP = WASTE_BURN_UNCONTROLLED
          * constructionGroup->fire_chance / 100;
        if(std::bernoulli_distribution(uncontrolledP)(BasicUrbg::get())
          && !(world.map(point)->flags & FLAG_FIRE_COVER)
        ) {
          torch();
          world.pushMessage(
            FireStartedMessage::create(point, *constructionGroup));
        }
    }

    if(world.total_time % 100 == 99) {
        reset_prod_counters();
    }
}

void Shanty::animate(unsigned long real_time) {
  if(start_burning_waste) { // start fire
    start_burning_waste = false;
    anim = real_time + ANIM_THRESHOLD(3 * WASTE_BURN_TIME);
  }
  if(real_time >= anim) { // stop fire
    waste_fire_frit->frame = -1;
  }
  else if(real_time >= waste_fire_anim) { // continue fire
    waste_fire_anim = real_time + ANIM_THRESHOLD(FIRE_ANIMATION_SPEED);
    int num_frames = waste_fire_frit->resourceGroup->graphicsInfoVector.size();
    if(++waste_fire_frit->frame >= num_frames)
      waste_fire_frit->frame = 0;
  }
}

void Shanty::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.add_sd(N_("Air Pollution"), world.map(point)->pollution);
  list_commodities(mps, production);
}

void Shanty::init_resources() {
  Construction::init_resources();

  waste_fire_frit = world.map(point)->createframe();
  waste_fire_frit->resourceGroup = ResourceGroup::resMap["Fire"];
  waste_fire_frit->move_x = 0;
  waste_fire_frit->move_y = 0;
  waste_fire_frit->frame = -1;
}

void
Shanty::bulldoze() {
  Construction::bulldoze();
  makeFire();
}

void
Shanty::makeFire() {
  int size = constructionGroup->size;
  for(MapPoint p(point); p.y < point.y + size; p.y++)
  for(p.x = point.x; p.x < point.x + size; p.x++) {
    fireConstructionGroup.placeItem(world, p);
    Fire *fire = dynamic_cast<Fire *>(world.map(p)->construction);
    assert(fire); if(!fire) continue;
    fire->burning_days = FIRE_LENGTH - 25;
  }
}

/** @file lincity/modules/shanty.cpp */
