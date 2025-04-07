/* ---------------------------------------------------------------------- *
 * src/lincity/ConstructionRequest.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2025      David Bears <dbear4q@gmail.com>
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

#include "ConstructionRequest.hpp"

#include "MapPoint.hpp"             // for MapPoint
#include "all_buildings.hpp"        // for ORE_RESERVE
#include "groups.hpp"               // for GROUP_WATER
#include "lin-city.hpp"             // for FLAG_POWER_CABLES_0, FLAG_POWER_C...
#include "lintypes.hpp"             // for Construction, ConstructionGroup
#include "modules/all_modules.hpp"  // for FireConstructionGroup, Fire, Park...
#include "transport.hpp"            // for POWER_MODULUS
#include "world.hpp"                // for World, Map, MapTile

void
ConstructionDeletionRequest::execute() {
  //std::cout << "deleting: " << subject->constructionGroup->name
  //<< " (" << subject->x << "," << subject->y << ")" << std::endl;
  World& world = subject->world;
  unsigned short size = subject->constructionGroup->size;
  subject->detach();
  for (int i = 0; i < size; i++)
  for (int j = 0; j < size; j++) {
    //update mps display
    world.map(subject->point.s(i).e(j))->flags &=
      ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
    world.setUpdated(World::Updatable::MAP);
  }
  // update adjacencies
  int x = subject->point.x, y = subject->point.y;
  world.map.connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
  world.map.desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void
OreMineDeletionRequest::execute() {
  World& world = subject->world;
  int size = subject->constructionGroup->size;
  MapPoint point = subject->point;
  subject->detach();
  for(MapPoint p(point); p.y < point.y + size; p.y++)
  for(p.x = point.x; p.x < point.x + size; p.x++) {
    world.map(p)->flags &=
      ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
    if(world.map(p)->ore_reserve < ORE_RESERVE / 2) {
      world.map(p)->setTerrain(GROUP_WATER);
      world.map(p)->flags |=
        FLAG_HAS_UNDERGROUND_WATER;
      world.map.connect_rivers(p.x, p.y);
    }
    //update mps display
    world.setUpdated(World::Updatable::MAP);
  }

  // update adjacencies
  int x = subject->point.x, y = subject->point.y;
  world.map.connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
  world.map.desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void
CommuneDeletionRequest::execute() {
  World& world = subject->world;
  unsigned short size = subject->constructionGroup->size;
  MapPoint point = subject->point;
  subject->detach();
  for(unsigned short i = 0; i < size; ++i)
  for(unsigned short j = 0; j < size; ++j) {
    MapPoint p(point.s(i).e(j));
    world.map(p)->flags &=
      ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
    if(world.map(p)->flags & FLAG_HAS_UNDERGROUND_WATER)
      parklandConstructionGroup.placeItem(world, p);
    //update mps display
    world.setUpdated(World::Updatable::MAP);
  }
  // update adjacencies
  int x = subject->point.x, y = subject->point.y;
  world.map.connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
  world.map.desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void
BurnDownRequest::execute() {
  World& world = subject->world;
  unsigned short size = subject->constructionGroup->size;
  subject->detach();
  for(unsigned short i = 0; i < size; ++i)
  for(unsigned short j = 0; j < size; ++j) {
    MapPoint p(subject->point.s(i).e(j));
    world.map(p)->flags &=
      ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
    fireConstructionGroup.placeItem(world, p);
    static_cast<Fire*>(world.map(p)->construction)
      ->burning_days = FIRE_LENGTH - 25;
    //update mps display
    world.setUpdated(World::Updatable::MAP);
  }
  // update adjacencies
  int x = subject->point.x, y = subject->point.y;
  world.map.connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
  world.map.desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void
SetOnFire::execute() {
  World& world = subject->world;
  unsigned short size = subject->constructionGroup->size;
  subject->detach();
  for(unsigned short i = 0; i < size; ++i)
  for(unsigned short j = 0; j < size; ++j) {
    MapPoint p(subject->point.s(i).e(j));
    world.map(p)->flags &=
      ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
    fireConstructionGroup.placeItem(world, p);
    //update mps display
    world.setUpdated(World::Updatable::MAP);
  }
  // update adjacencies
  int x = subject->point.x, y = subject->point.y;
  world.map.connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
  world.map.desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void
PowerLineFlashRequest::execute() {
  int *anim_counter = &(dynamic_cast<Powerline*>(subject)->anim_counter);
  // 2/3 cooldown will prevent interlacing wave packets
  if(*anim_counter <= POWER_MODULUS / 3)
    *anim_counter = POWER_MODULUS;
}
