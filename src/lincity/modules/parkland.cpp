/* ---------------------------------------------------------------------- *
 * src/lincity/modules/parkland.cpp
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

#include "parkland.h"

#include "modules.h"

// Parkland:
ParklandConstructionGroup parklandConstructionGroup(
     N_("Park"),
     TRUE,                     /* need credit? */
     GROUP_PARKLAND,
     GROUP_PARKLAND_SIZE,
     GROUP_PARKLAND_COLOUR,
     GROUP_PARKLAND_COST_MUL,
     GROUP_PARKLAND_BUL_COST,
     GROUP_PARKLAND_FIREC,
     GROUP_PARKLAND_COST,
     GROUP_PARKLAND_TECH,
     GROUP_PARKLAND_RANGE
);

ParklandConstructionGroup parkpondConstructionGroup(
     N_("Park (Pond)"),
     TRUE,                     /* need credit? */
     GROUP_PARKPOND,
     GROUP_PARKLAND_SIZE,
     GROUP_PARKLAND_COLOUR,
     GROUP_PARKLAND_COST_MUL,
     GROUP_PARKLAND_BUL_COST,
     GROUP_PARKLAND_FIREC,
     GROUP_PARKLAND_COST,
     GROUP_PARKLAND_TECH,
     GROUP_PARKLAND_RANGE
);

Construction *ParklandConstructionGroup::createConstruction(World& world) {
  return new Parkland(world, this);
}

bool
ParklandConstructionGroup::can_build_here(const World& world,
  const MapPoint point, Message::ptr& message
) const {
  if(!(world.map(point)->flags & FLAG_HAS_UNDERGROUND_WATER)) {
    message = DesertHereMessage::create(point);
    return false;
  }

  return ConstructionGroup::can_build_here(world, point, message);
}

Parkland::Parkland(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->flags |= FLAG_NEVER_EVACUATE;
}

void Parkland::update()
{
    if (world.map(x,y)->pollution > 10 && (world.total_time & 1) == 0)
        world.map(x,y)->pollution --;
}

void Parkland::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  mps.add_sd(N_("Air Pollution"), world.map(x,y)->pollution);
}

/** @file lincity/modules/parkland.cpp */
