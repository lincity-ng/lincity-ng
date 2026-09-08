/* ---------------------------------------------------------------------- *
 * src/lincity-ng/UserOperation.cpp
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

#include "UserOperation.hpp"

#include <assert.h>                       // for assert
#include <stddef.h>                       // for NULL

#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/lin-city.hpp"           // for FLAG_NEVER_EVACUATE
#include "lincity/lintypes.hpp"           // for ConstructionGroup, Construc...
#include "lincity/modules/windmill.hpp"   // for WindmillConstructionGroup
#include "lincity/modules/windpower.hpp"  // for WindpowerConstructionGroup
#include "lincity/world.hpp"              // for World, Map, MapTile
#include "lincity/modules/track_road_rail.hpp"

UserOperation::UserOperation() {
  constructionGroup = NULL;
  action = ACTION_QUERY;
}

UserOperation::~UserOperation() { }

UserOperation&
UserOperation::operator=(const UserOperation& other) {
  constructionGroup = other.constructionGroup;
  action = other.action;
  return *this;
}

bool
UserOperation::isAllowed(World& world, Message::ptr& message) const {
  switch(action) {
  case ACTION_BUILD:
    return actualConstructionGroup(world)->can_build(world, message);
  case ACTION_QUERY:
  case ACTION_BULLDOZE:
  case ACTION_EVACUATE:
  case ACTION_FLOOD:
    return true;
  default:
    assert(false);
    return false;
  }
}

bool
UserOperation::isAllowedHere(World& world, MapPoint point, Message::ptr& message
) const {
  if(!world.map.is_visible(point)) {
    message = OutsideMapMessage::create(point);
    return false;
  }
  switch(action) {
  case ACTION_QUERY:
    return true;
  case ACTION_BUILD: {
    ConstructionGroup *actual = actualConstructionGroup(world, point);
    return actual->can_build(world, message)
      && actual->can_build_here(world, point, message);
  }
  case ACTION_BULLDOZE: {
    if(Construction *cst = world.map(point)->reportingConstruction) {
      return cst->can_bulldoze(message);
    }
    else {
      // tiles are always bulldozable (given sufficient funds)
      return true;
    }
  }
  case ACTION_EVACUATE:
    if(!world.map(point)->reportingConstruction) {
      message = NothingHereMessage::create(point);
      return false;
    }
    if(world.map(point)->reportingConstruction->flags & FLAG_NEVER_EVACUATE) {
      message = CannotEvacuateThisMessage::create(point,
        *world.map(point)->reportingConstruction->constructionGroup);
      return false;
    }
    return true;
  case ACTION_FLOOD: {
    bool bare = world.map(point)->is_bare();
    if(!bare) message = SpaceOccupiedMessage::create(point);
    return bare;
  }
  default:
    assert(false);
    return false;
  }
}

unsigned short UserOperation::cursorSize(void) const {
    if( action == ACTION_QUERY )
    {   return 0;}
    else if(action == ACTION_BUILD)
    {   return constructionGroup->size;}
    else
    {   return 1;}
}

void
UserOperation::execute(World& world, MapPoint point) {
  assert(world.map.is_visible(point));

  switch(action) {
  case UserOperation::ACTION_QUERY: {
    return; // query handled entirely by Game
  }

  case UserOperation::ACTION_BUILD: {
    assert(constructionGroup);
    world.buildConstruction(*actualConstructionGroup(world, point), point);
    break;
  }

  case UserOperation::ACTION_BULLDOZE: {
    world.bulldozeArea(point);
    break;
  }

  case UserOperation::ACTION_EVACUATE: {
    world.evacuateArea(point);
    break;
  }

  case UserOperation::ACTION_FLOOD: {
    world.floodArea(point);
    break;
  }

  default: {
    assert(false);
  }
  }
}

ConstructionGroup *
UserOperation::actualConstructionGroup(
  const World& world, MapPoint point
) const {
  if(constructionGroup == &windmillConstructionGroup
    || constructionGroup == &windpowerConstructionGroup
  ) {
    if(world.tech_level >= windpowerConstructionGroup.tech)
      return &windpowerConstructionGroup;
    else
      return &windmillConstructionGroup;
  }
  else if(TransportConstructionGroup *transport =
    dynamic_cast<TransportConstructionGroup *>(constructionGroup)
  ) {
    if(world.map.is_visible(point) &&
      transport->isBridge() != world.map(point)->is_water()
    ) {
      switch(transport->group) {
      case GROUP_TRACK: return &trackbridgeConstructionGroup;
      case GROUP_ROAD: return &roadbridgeConstructionGroup;
      case GROUP_RAIL: return &railbridgeConstructionGroup;
      case GROUP_TRACK_BRIDGE: return &trackConstructionGroup;
      case GROUP_ROAD_BRIDGE: return &roadConstructionGroup;
      case GROUP_RAIL_BRIDGE: return &railConstructionGroup;
      default: assert(false);
      }
    }
  }
  return constructionGroup;
}

int
UserOperation::cost(const World& world, MapPoint point) const {
  switch(action) {
  case ACTION_BUILD:
    return actualConstructionGroup(world, point)->getCosts(world);
  case ACTION_BULLDOZE:
    return world.map(point)->getTileConstructionGroup()->bul_cost;
  case ACTION_FLOOD:
    return GROUP_WATER_COST;
  case ACTION_QUERY:
  case ACTION_EVACUATE:
    return 0;
  default:
    assert(false);
    return 0;
  }
}
