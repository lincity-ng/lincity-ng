/* ---------------------------------------------------------------------- *
 * src/lincity/engine.cpp
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

#include <algorithm>         // for max
#include <cassert>           // for assert
#include <climits>           // for INT_MAX
#include <cstdlib>           // for rand
#include <deque>             // for deque
#include <initializer_list>  // for initializer_list
#include <optional>          // for optional, nullopt, nullopt_t
#include <unordered_set>     // for unordered_set
#include <vector>            // for vector

#include "MapPoint.hpp"      // for MapPoint, hash
#include "groups.hpp"        // for GROUP_BARE, GROUP_DESERT
#include "lin-city.hpp"      // for FLAG_FIRE_COVER, FLAG_IS_RIVER, FLAG_CRI...
#include "lintypes.hpp"      // for Construction, ConstructionGroup
#include "messages.hpp"      // for OutOfMoneyMessage, FireStartedMessage
#include "stats.hpp"         // for Stat, Stats
#include "world.hpp"         // for Map, World, MapTile

void
World::income(int amt, Stat<int>& account) {
  assert(amt >= 0);
  int newBal = total_money + amt;
  if(newBal < total_money)
    newBal = INT_MAX;
  total_money = newBal;
  account -= amt;
  setUpdated(Updatable::MONEY);
}

void
World::expense(int amt, Stat<int>& account, bool allowCredit) {
  assert(amt >= 0);
  int newBal = total_money - amt;
  if(newBal < (allowCredit ? -2000000000 : 0) || newBal > total_money)
    throw OutOfMoneyMessage::create(allowCredit)->exception();
  total_money = newBal;
  account += amt;
  setUpdated(Updatable::MONEY);
  // TODO: move to ng
  if(total_money > 0 && newBal <= 0)
    pushMessage(OutOfMoneyMessage::create(false));
}

void
World::do_pollution() {
  //kill pollution from edges of map
  //diffuse pollution inside the map

  for(auto it = map.polluted.begin();
    it != map.polluted.end();
    ++it
  ) {
    if(map.is_border(*it)) {
      map(*it)->pollution /= 64;
      continue;
    }

    MapPoint neighbor;
    switch(rand() % 11) {
    case 0:
    case 1:
    case 2:
      neighbor = it->n();
      break;
    case 3:
    case 4:
    case 5:
      neighbor = it->w();
      break;
    case 6:
    case 7:
      neighbor = it->s();
      break;
    case 8:
    case 9:
      neighbor = it->e();
      break;
    case 10:
      neighbor = *it;
      map(*it)->pollution -= 2;
      break;
    default:
      assert(false);
    }

    int pflow = map(*it)->pollution/16;
    map(*it)->pollution -= pflow;
    map(neighbor)->pollution += pflow;
  }
}

void
World::scan_pollution() {
  const int len = map.len();
  const int area = len * len;
  std::unordered_set<MapPoint>::iterator it;
  stats.total_pollution = 0;
  map.polluted.clear();
  for(int index = 0; index < area; ++index) {
    MapPoint p(index % len, index / len);
    if(map(p)->pollution > 10)
      map.polluted.push_back(p);
    stats.total_pollution += map(p)->pollution;
  }
}

void
World::do_fire_health_cricket_power_cover() {
  const int coverFlags[] = {
    FLAG_FIRE_COVER,
    FLAG_HEALTH_COVER,
    FLAG_CRICKET_COVER,
    FLAG_MARKET_COVER
  };
  const int checkFlags[] = {
    FLAG_FIRE_COVER_CHECK,
    FLAG_HEALTH_COVER_CHECK,
    FLAG_CRICKET_COVER_CHECK,
    FLAG_MARKET_COVER_CHECK
  };
  const int flagNum = 4;
  for(MapTile& tile : map) {
    unsigned int& tileFlags = tile.flags;
    for(int fi = 0; fi < flagNum; fi++) {
      tileFlags =
        (tileFlags & ~coverFlags[fi] & ~checkFlags[fi]) |  // clear
        (tileFlags & checkFlags[fi] ? coverFlags[fi] : 0); // cover = check
    }
  }
}

void
World::do_random_fire() {
  MapPoint loc(rand() % map.len(), rand() % map.len());\
  if(!map.is_visible(loc))
    return;

  Construction *cst = map(loc)->reportingConstruction;
  if(!cst) return; // no building to burn down
  const ConstructionGroup& cstGrp = *cst->constructionGroup;
  if(rand() % 100 >= cstGrp.fire_chance)
    return;
  if(map(loc)->flags & FLAG_FIRE_COVER)
    return;

  cst->torch();
  pushMessage(FireStartedMessage::create(loc, cstGrp));
}

void
World::do_daily_ecology() {
  for(MapTile& tile : map) {
    /* approximately 3 monthes needed to turn bulldoze area into green */
    if(tile.getLowerstVisibleGroup() == GROUP_DESERT
      && (tile.flags & FLAG_HAS_UNDERGROUND_WATER)
      && (rand() % 300 == 1)
    ) {
      tile.setTerrain(GROUP_BARE);
      map.desert_water_frontiers(tile.point.x - 1, tile.point.y - 1, 1 + 2, 1 + 2);
    }
  }

  //TODO: depending on water, green can become trees
  //      pollution can make desert
  //      etc ...
  /*TODO incorporate do_daily_ecology to simulate_mappoints. */
}

void
Map::connect_rivers(int x, int y) {
  MapPoint point(x,y);
  Map& map = (*this);
  assert(map.is_visible(point));

  std::deque<MapPoint> line;

  if(map(point)->is_lake() && (
    map(point.n())->is_river() ||
    map(point.s())->is_river() ||
    map(point.e())->is_river() ||
    map(point.w())->is_river()
  )) {
    map(point)->flags |= FLAG_IS_RIVER;
    line.push_back(point);
  }

  while(!line.empty()) {
    MapPoint p = line.front();
    line.pop_front();
    // TODO: make sure we don't go outside the map
    for(MapPoint p1 : {p.n(), p.s(), p.e(), p.w()})
      if(map(p1)->is_lake()) {
        map(p1)->flags |= FLAG_IS_RIVER;
        line.push_back(p1);
      }
  }
}

/* Feature: coal survey should vary in price and accuracy with technology */
void
World::do_coal_survey() {
  if(coal_survey_done)
    return;
  expense(1000000, stats.expenses.coalSurvey);
  coal_survey_done = 1;
}

void
Map::desert_water_frontiers(int x, int y, int w, int h) {
  desert_water_frontiers(MapPoint(x+1,y+1), MapPoint(x+w-1,y+h-1));
}

void
Map::desert_water_frontiers(MapPoint p0, MapPoint p1) {
  Map& map = *this;
  assert(p0.x <= p1.x && p0.y <= p1.y);
  assert(is_visible(p0) && is_inside(p1));

  for(MapPoint p(0, std::max(p0.y-1, 1)); p.y < p1.y; p.y++)
  for(p.x = std::max(p0.x-1, 1); p.x < p1.x; p.x++) {
    unsigned short grp = map(p)->getLowerstVisibleGroup();
    // if(grp != GROUP_DESERT && grp != GROUP_WATER) continue;

    // south = 1, east = 2, west = 4, north = 8
    MapPoint q = p.e();
    if(p.y >= p0.y && q.x < map.len()-1) {
      if(map(q)->getLowerstVisibleGroup() == grp) {
        map(p)->type |= 2;
        map(q)->type |= 4;
      }
      else {
        map(p)->type &= ~2;
        map(q)->type &= ~4;
      }
    }

    q = p.s();
    if(p.x >= p0.x && q.y < map.len()-1) {
      if(map(q)->getLowerstVisibleGroup() == grp) {
        map(p)->type |= 1;
        map(q)->type |= 8;
      }
      else {
        map(p)->type &= ~1;
        map(q)->type &= ~8;
      }
    }
  }
}

/*
   // spiral arounf mapTile[x][y] until we hit something of group group.
   // return the x y coords encoded as x+y*world.len()
   // return -1 if we don't find one.
 */
std::optional<MapPoint>
Map::find_group(MapPoint p, unsigned short group) {
  for(unsigned int i = 2; i < len() * 4; i++) {
    int& c = i & 1 ? p.y : p.x;
    int d = i & 2 ? -1 : 1;
    for(int j = i / 2; j > 0; j--) {
      if(is_visible(p) && operator()(p)->getTopGroup() == group)
        return p;
      c += d;
    }
  }
  return std::nullopt;
}

/*
   // spiral round from startx,starty until we hit a 2x2 space.
   // return the x y coords encoded as x+y*world.len()
   // return -1 if we don't find one.
 */
bool
Map::is_bare_area(MapPoint point, int size) {
  for(int i = 0; i < size; i++)
  for(int j = 0; j < size; j++) {
    MapPoint p = point.s(i).e(j);
    if(!is_visible(p) || !operator()(p)->is_bare())
      return false;
  }
  return true;
}

std::optional<MapPoint>
Map::find_bare_area(MapPoint p, int size) {
  for(int i = 2; i < len() * 4; i++) {
    int& c = i & 1 ? p.y : p.x;
    int d = i & 2 ? -1 : 1;
    for(int j = i / 2; j > 0; j--) {
      if(is_visible(p) && is_bare_area(p, size))
        return p;
      c += d;
    }
  }
  return std::nullopt;
}

/** @file lincity/engine.cpp */
