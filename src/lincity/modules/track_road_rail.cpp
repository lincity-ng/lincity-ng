/* ---------------------------------------------------------------------- *
 * src/lincity/modules/track_road_rail.cpp
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

#include "track_road_rail.h"

#include <stdlib.h>                       // for rand
#include <cassert>                        // for assert
#include <map>                            // for map
#include <string>                         // for basic_string, char_traits
#include <vector>                         // for vector

#include "fire.h"                         // for FIRE_ANIMATION_SPEED
#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity-ng/Sound.hpp"           // for getSound, Sound
#include "lincity/ConstructionRequest.h"  // for ConstructionDeletionRequest
#include "lincity/all_buildings.h"        // for DAYS_PER_RAIL_POLLUTION
#include "lincity/lin-city.h"             // for FALSE, ANIM_THRESHOLD, FLAG...
#include "lincity/messages.hpp"           // for OutOfMoneyMessage
#include "lincity/resources.hpp"          // for ExtraFrame, ResourceGroup
#include "lincity/stats.h"                // for Stats
#include "lincity/world.h"                // for World, MapTile, Map
#include "tinygettext/gettext.hpp"        // for N_, _

// Track:
TransportConstructionGroup trackConstructionGroup(
    N_("Track"),
    FALSE,                     /* need credit? */
    GROUP_TRACK,
    GROUP_TRANSPORT_SIZE,
    GROUP_TRACK_COLOUR,
    GROUP_TRACK_COST_MUL,
    GROUP_TRACK_BUL_COST,
    GROUP_TRACK_FIREC,
    GROUP_TRACK_COST,
    GROUP_TRACK_TECH,
    GROUP_TRANSPORT_RANGE
);

// TrackBridge:
TransportConstructionGroup trackbridgeConstructionGroup(
    N_("Track Bridge"),
    FALSE,                     /* need credit? */
    GROUP_TRACK_BRIDGE,
    GROUP_TRANSPORT_SIZE,
    GROUP_TRACK_BRIDGE_COLOUR,
    GROUP_TRACK_BRIDGE_COST_MUL,
    GROUP_TRACK_BRIDGE_BUL_COST,
    GROUP_TRACK_BRIDGE_FIREC,
    GROUP_TRACK_BRIDGE_COST,
    GROUP_TRACK_BRIDGE_TECH,
    GROUP_TRANSPORT_RANGE
);

// Road:
TransportConstructionGroup roadConstructionGroup(
    N_("Road"),
    FALSE,                     /* need credit? */
    GROUP_ROAD,
    GROUP_TRANSPORT_SIZE,
    GROUP_ROAD_COLOUR,
    GROUP_ROAD_COST_MUL,
    GROUP_ROAD_BUL_COST,
    GROUP_ROAD_FIREC,
    GROUP_ROAD_COST,
    GROUP_ROAD_TECH,
    GROUP_TRANSPORT_RANGE
);
// RoadBridge:
TransportConstructionGroup roadbridgeConstructionGroup(
    N_("Road Bridge"),
    FALSE,                     /* need credit? */
    GROUP_ROAD_BRIDGE,
    GROUP_TRANSPORT_SIZE,
    GROUP_ROAD_BRIDGE_COLOUR,
    GROUP_ROAD_BRIDGE_COST_MUL,
    GROUP_ROAD_BRIDGE_BUL_COST,
    GROUP_ROAD_BRIDGE_FIREC,
    GROUP_ROAD_BRIDGE_COST,
    GROUP_ROAD_BRIDGE_TECH,
    GROUP_TRANSPORT_RANGE
);


// Rail:
TransportConstructionGroup railConstructionGroup(
    N_("Rail"),
    FALSE,                     /* need credit? */
    GROUP_RAIL,
    GROUP_TRANSPORT_SIZE,
    GROUP_RAIL_COLOUR,
    GROUP_RAIL_COST_MUL,
    GROUP_RAIL_BUL_COST,
    GROUP_RAIL_FIREC,
    GROUP_RAIL_COST,
    GROUP_RAIL_TECH,
    GROUP_TRANSPORT_RANGE
);
// RailBridge:
TransportConstructionGroup railbridgeConstructionGroup(
    N_("Rail Bridge"),
    FALSE,                     /* need credit? */
    GROUP_RAIL_BRIDGE,
    GROUP_TRANSPORT_SIZE,
    GROUP_RAIL_BRIDGE_COLOUR,
    GROUP_RAIL_BRIDGE_COST_MUL,
    GROUP_RAIL_BRIDGE_BUL_COST,
    GROUP_RAIL_BRIDGE_FIREC,
    GROUP_RAIL_BRIDGE_COST,
    GROUP_RAIL_BRIDGE_TECH,
    GROUP_TRANSPORT_RANGE
);

Construction *TransportConstructionGroup::createConstruction(World& world) {
  return new Transport(world, this);
}

void
TransportConstructionGroup::placeItem(World& world, int x, int y) {
  MapTile& tile = *world.map(x, y);
  unsigned short oldGrp = tile.getTransportGroup();
  if(tile.is_transport() && oldGrp != group || oldGrp == GROUP_POWER_LINE) {
    ConstructionDeletionRequest(tile.reportingConstruction).execute();
  }

  ConstructionGroup::placeItem(world, x, y);
}

Transport::Transport(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->anim = 0;
  this->start_burning_waste = false;
  this->waste_fire_anim = 0;
  // register the construction as transport tile
  // disable evacuation
  //transparency is set and updated in connect_transport
  this->flags |= (FLAG_IS_TRANSPORT | FLAG_NEVER_EVACUATE);

  initialize_commodities();
  this->trafficCount = this->commodityCount;

  switch (constructionGroup->group) {
  case GROUP_ROAD:
  case GROUP_ROAD_BRIDGE:
    commodityMaxCons[STUFF_GOODS] =
      (100 - 1) / (ROAD_GOODS_USED_MASK + 1) + 1;
    commodityMaxProd[STUFF_WASTE] =
      (100 - 1) / (ROAD_GOODS_USED_MASK + 1) + 1;
    break;
  case GROUP_RAIL:
  case GROUP_RAIL_BRIDGE:
    commodityMaxCons[STUFF_GOODS] =
      (100 - 1) / (RAIL_GOODS_USED_MASK + 1) + 1;
    commodityMaxCons[STUFF_STEEL] =
      (100 - 1) / (RAIL_STEEL_USED_MASK + 1) + 1;
    commodityMaxProd[STUFF_WASTE] =
      (100 - 1) / (RAIL_GOODS_USED_MASK + 1) + 1 +
      (100 - 1) / (RAIL_STEEL_USED_MASK + 1) + 1;
    break;
  }
  commodityMaxCons[STUFF_LOVOLT] = 100 * LOVOLT_LOSS_ON_TRANSPORT;
  commodityMaxCons[STUFF_WASTE] = 100 * WASTE_BURN_ON_TRANSPORT;
}
Transport::~Transport() {
  world.map(x,y)->killframe(waste_fire_frit);
}

void Transport::update()
{
  switch(constructionGroup->group) {
  case GROUP_TRACK:
  case GROUP_TRACK_BRIDGE:
    //tracks have no side effects
    break;
  case GROUP_ROAD:
  case GROUP_ROAD_BRIDGE:
    try {
      if(world.total_time % 100 == 0)
        world.expense(world.money_rates.transport_cost,
          world.stats.expenses.transport);
    } catch(const OutOfMoneyMessage::Exception& ex) {
      // TODO: reduce transport capacity
      // maybe have a chance of converting to track or deleting
    }
    if(world.total_time % DAYS_PER_ROAD_POLLUTION == 0)
      world.map(x,y)->pollution += ROAD_POLLUTION;
    if(!(world.total_time & ROAD_GOODS_USED_MASK)
      && commodityCount[STUFF_GOODS] > 0
    ) {
      consumeStuff(STUFF_GOODS, 1);
      produceStuff(STUFF_WASTE, 1);
    }
    break;
  case GROUP_RAIL:
  case GROUP_RAIL_BRIDGE:
    try {
      if(world.total_time % 100 == 0)
        world.expense(3 * world.money_rates.transport_cost,
          world.stats.expenses.transport);
    } catch(const OutOfMoneyMessage::Exception& ex) {
      // TODO: reduce transport capacity
    }
    if(world.total_time % DAYS_PER_RAIL_POLLUTION == 0)
      world.map(x,y)->pollution += RAIL_POLLUTION;
    if(!(world.total_time & RAIL_GOODS_USED_MASK)
      && commodityCount[STUFF_GOODS] > 0
    ) {
      consumeStuff(STUFF_GOODS, 1);
      produceStuff(STUFF_WASTE, 1);
    }
    if((world.total_time & RAIL_STEEL_USED_MASK) == 0
      && commodityCount[STUFF_STEEL] > 0
    ) {
      consumeStuff(STUFF_STEEL, 1);
      produceStuff(STUFF_WASTE, 1);
    }
    break;
  default:
    assert(false);
  }
  if (commodityCount[STUFF_LOVOLT] >= LOVOLT_LOSS_ON_TRANSPORT) {
    consumeStuff(STUFF_LOVOLT, LOVOLT_LOSS_ON_TRANSPORT);
  }
  else if (commodityCount[STUFF_LOVOLT] > 0) {
    consumeStuff(STUFF_LOVOLT, 1);
  }

  int wasteMax = constructionGroup->commodityRuleCount[STUFF_WASTE].maxload;
  if(commodityCount[STUFF_WASTE] > wasteMax * 9 / 10) {
    consumeStuff(STUFF_WASTE, WASTE_BURN_ON_TRANSPORT);
    world.map(x,y)->pollution += WASTE_BURN_ON_TRANSPORT_POLLUTE;
    start_burning_waste = true;
  }

  if(world.total_time % 100 == 99) {
    reset_prod_counters();
  }
}

void Transport::animate(unsigned long real_time) {
  if(start_burning_waste) { // start fire
    start_burning_waste = false;
    anim = real_time + ANIM_THRESHOLD(WASTE_BURN_TIME);
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

void Transport::list_traffic(Mps& mps) const {
  for(Commodity stuff = STUFF_INIT ; stuff < STUFF_COUNT ; stuff++) {
    if(!constructionGroup->commodityRuleCount[stuff].maxload)
      continue;
    mps.add_sfp(commodityNames[stuff],
      trafficCount[stuff] * 107.77 * TRANSPORT_RATE / TRANSPORT_QUANTA);
  }
}

void Transport::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  if(production) {
    mps.add_s(_("Traffic"));
    list_traffic(mps);
  }
  else {
    // mps.add_s(_("Commodities"));
    list_commodities(mps, false);
  }
}

void Transport::playSound()
{
    if(soundGroup->sounds_loaded)
    {
        unsigned short g = constructionGroup->group;
        if ((g == GROUP_ROAD) || (g == GROUP_ROAD_BRIDGE))
        {
            int avg = 0;
            int size = 0;
            for(Commodity stuff = STUFF_INIT ; stuff < STUFF_COUNT ; stuff++)
            {
              if(!constructionGroup->commodityRuleCount[stuff].maxload) continue;
              avg += (trafficCount[stuff] * 107 * TRANSPORT_RATE / TRANSPORT_QUANTA);
              size++;
            }
            if(avg > 0) //equiv to size > 0
            {   avg /= size;}
            int num_sounds = soundGroup->chunks.size()/2;
            if(avg > 5)
            {   getSound()->playASound(soundGroup->chunks[rand()%num_sounds]);}
            else
            {   getSound()->playASound(soundGroup->chunks[num_sounds+rand()%num_sounds]);}
        }
        else
        {
            int s = soundGroup->chunks.size();
            getSound()->playASound(soundGroup->chunks[rand()%s]);
        }
    }
}

bool Transport::canPlaceVehicle() {
  if(!world.map(x, y)->framesptr)
    return false;
  for(ExtraFrame& exfr : *world.map(x, y)->framesptr)
    if(exfr.resourceGroup->is_vehicle)
      return false;
  return true;
}

void Transport::init_resources() {
  Construction::init_resources();

  waste_fire_frit = world.map(x, y)->createframe();
  waste_fire_frit->resourceGroup = ResourceGroup::resMap["Fire"];
  waste_fire_frit->move_x = 0;
  waste_fire_frit->move_y = 0;
  waste_fire_frit->frame = -1;
}

void Transport::place(int x, int y) {
  // set the constructionGroup to build bridges iff over water
  if(world.map(x,y)->is_water()) {
    switch (constructionGroup->group) {
      case GROUP_TRACK:
        constructionGroup = &trackbridgeConstructionGroup;
      break;
      case GROUP_ROAD:
        constructionGroup = &roadbridgeConstructionGroup;
      break;
      case GROUP_RAIL:
        constructionGroup = &railbridgeConstructionGroup;
      break;
    }
  }
  else {
    switch (constructionGroup->group) {
      case GROUP_TRACK_BRIDGE:
        constructionGroup = &trackConstructionGroup;
      break;
      case GROUP_ROAD_BRIDGE:
        constructionGroup = &roadConstructionGroup;
      break;
      case GROUP_RAIL_BRIDGE:
        constructionGroup = &railConstructionGroup;
      break;
    }
  }

  Construction::place(x, y);
}

/** @file lincity/modules/track_road_rail_powerline.cpp */
