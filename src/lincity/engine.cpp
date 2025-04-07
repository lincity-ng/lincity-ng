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

#include <cassert>                // for assert
#include <deque>                  // for deque
#include <limits.h>               // for INT_MAX
#include <memory>                 // for __shared_ptr_access
#include <stdlib.h>               // for rand
#include <unordered_set>          // for unordered_set
#include <vector>                 // for vector

#include "ConstructionRequest.hpp"  // for SetOnFire
#include "MapPoint.hpp"           // for MapPoint
#include "engglobs.hpp"             // for dx, dy
#include "groups.hpp"               // for GROUP_DESERT, GROUP_PORT, GROUP_WATER
#include "lctypes.hpp"              // for CST_GREEN
#include "lin-city.hpp"             // for FLAG_FIRE_COVER, FLAG_CRICKET_COVER
#include "lintypes.hpp"             // for Construction, ConstructionGroup
#include "messages.hpp"           // for OutOfMoneyMessage, FireStartedMessage
#include "modules/all_modules.hpp"  // for Residence
#include "stats.hpp"                // for Stat, Stats
#include "world.hpp"                // for Map, World, MapTile

extern void ok_dial_box(const char *, int, const char *);
extern void print_total_money(void);

/****** Private functions prototypes *****/
//static void bulldoze_mappoint(short fill, int x, int y);
//static int is_real_river(int x, int y);

/*************** Global functions ********************/
void
World::fire_area(MapPoint loc) {
  /* this happens when a rocket crashes or on random_fire. */
  MapTile& tile = *map(loc);
  if(tile.getGroup() == GROUP_WATER || tile.getGroup() == GROUP_FIRE)
    return;
  Construction *cst = tile.reportingConstruction;
  if(cst) {
    //fire is an unatural death for one in two
    if(tile.is_residence()) {
      Residence *residence = dynamic_cast<Residence*>(cst);
      int casualities = residence->local_population/2;
      residence->local_population -= casualities;
      stats.population.unnat_deaths_m += casualities;
      stats.population.deaths_m += casualities;
    }
    SetOnFire(cst).execute();
  }
}

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
  if(allowCredit ? newBal < -2000000000 || newBal > total_money : newBal < 0) {
    throw OutOfMoneyMessage::create(allowCredit)->exception();
  }
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
      neighbor = it->n();
      break;
    case 6:
    case 7:
      neighbor = it->n();
      break;
    case 8:
    case 9:
      neighbor = it->n();
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
    int& tileFlags = tile.flags;
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

  const ConstructionGroup& cstGrp = *map(loc)->getConstructionGroup();
  if(rand() % 100 >= cstGrp.fire_chance)
    return;
  if(map(loc)->flags & FLAG_FIRE_COVER)
    return;

  fire_area(loc);
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
      tile.setTerrain(CST_GREEN);
      map.desert_water_frontiers(tile.point.x - 1, tile.point.y - 1, 1 + 2, 1 + 2);
    }
  }

  //TODO: depending on water, green can become trees
  //      pollution can make desert
  //      etc ...
  /*TODO incorporate do_daily_ecology to simulate_mappoints. */
}

int
Map::check_group(int x, int y) {
  if(!is_inside(MapPoint(x,y)))
    return -1;
  return (*this)(MapPoint(x,y))->getGroup();
}

int
Map::check_topgroup(int x, int y) {
  if(!is_inside(MapPoint(x,y)))
    return -1;
  return (*this)(MapPoint(x,y))->getTopGroup();
}

void
Map::connect_rivers(int x, int y) {
  Map& map = (*this);
  std::deque<int> line;
  const int len = map.len();

  line.clear();
  //only act on lakes
  if(map(x,y)->is_lake())
    line.push_back(y*len+x);

  while(line.size()>0) {
    int x = line.front() % len;
    int y = line.front() / len;
    line.pop_front();
    //check for close by river
    for(unsigned int i = 0;i<4;++i) {
      int xx = x + dx[i];
      int yy = y + dy[i];
      if(map(xx,yy)->is_river()) {
        map(x, y)->flags |= FLAG_IS_RIVER;
        i = 4;
        //now check for more close by lakes
        for(unsigned int j = 0;j<4;++j) {
          int x3 = x + dx[j];
          int y3 = y + dy[j];
          if(map(x3,y3)->is_lake())
            line.push_back(y3*len+x3);
        }
      }
    }
  }
}

/* Feature: coal survey should vary in price and accuracy with technology */
void
World::do_coal_survey() {
  if(coal_survey_done == 0)
    return;
  expense(1000000, stats.expenses.coalSurvey);
  coal_survey_done = 1;
}

void
Map::desert_water_frontiers(int originx, int originy, int w, int h) {
    Map& map = (*this);
    /* copied from connect_transport */
    // sets the correct TYPE depending on neighbours, => gives the correct tile to display
    int mask;
/*
    static const short desert_table[16] = {
        CST_DESERT_0, CST_DESERT_1D, CST_DESERT_1R, CST_DESERT_2RD,
        CST_DESERT_1L, CST_DESERT_2LD, CST_DESERT_2LR, CST_DESERT_3LRD,
        CST_DESERT_1U, CST_DESERT_2UD, CST_DESERT_2RU, CST_DESERT_3RUD,
        CST_DESERT_2LU, CST_DESERT_3LUD, CST_DESERT_3LRU, CST_DESERT
    };

#if FLAG_LEFT != 1 || FLAG_UP != 2 || FLAG_RIGHT != 4 || FLAG_DOWN != 8
#error  desert_frontier(): you loose
#error  the algorithm depends on proper flag settings -- (ThMO)
#endif
*/
    /* Adjust originx,originy,w,h to proper range */
    if (originx <= 0)
    {
        w -= 1 - originx;
        originx = 1;
    }
    if (originy <= 0)
    {
        h -= 1 - originy;
        originy = 1;
    }
    if(originx + w >= map.len())
      w = map.len() - originx;
    if(originy + h >= map.len())
      h = map.len() - originy;

    for (int x = originx; x < originx + w; x++)
    {
        for (int y = originy; y < originy + h; y++)
        {
            if(map(x, y)->getLowerstVisibleGroup() == GROUP_DESERT) {
                mask = 0;
                if(map.is_visible(x,y-1) && map(x,y-1)->getLowerstVisibleGroup() == GROUP_DESERT)
                {   mask |= 8;}
                if(map.is_visible(x-1,y) && map(x-1,y)->getLowerstVisibleGroup() == GROUP_DESERT)
                {   mask |= 4;}
                if(map.is_visible(x+1,y) && map(x+1,y)->getLowerstVisibleGroup() == GROUP_DESERT)
                {   mask |= 2;}
                if(map.is_visible(x,y+1) && map(x,y+1)->getLowerstVisibleGroup() == GROUP_DESERT)
                {   ++mask;}
                map(x, y)->type = mask;
            }
            else if(map(x, y)->getLowerstVisibleGroup() == GROUP_WATER) {
                mask = 0;
                if(map.is_visible(x,y-1) && (map(x,y-1)->is_water() || check_group(x,y-1) == GROUP_PORT))
                {   mask |= 8;}
                if(map.is_visible(x-1,y) && (map(x-1,y)->is_water() || check_group(x-1,y) == GROUP_PORT))
                {   mask |= 4;}
                if(map.is_visible(x+1,y) && map(x+1,y)->is_water())
                {   mask |= 2;}
                if(map.is_visible(x,y+1) && map(x,y+1)->is_water())
                {   ++mask;}
                map(x, y)->type = mask;
            }
        }
    }
}

/*
   // spiral arounf mapTile[x][y] until we hit something of group group.
   // return the x y coords encoded as x+y*world.len()
   // return -1 if we don't find one.
 */
int
World::find_group(int x, int y, unsigned short group) {
    int i, j;
    for(i = 1; i < (2 * map.len()); i++) {
        for (j = 0; j < i; j++)
        {
            x--;
            if(map.is_visible(x, y) && map(x, y)->getTopGroup() == group)
              return (x + y * map.len());
        }
        for (j = 0; j < i; j++) {
            y--;
            if(map.is_visible(x, y) && map(x, y)->getTopGroup() == group)
              return (x + y * map.len());
        }
        i++;
        for (j = 0; j < i; j++)
        {
            x++;
            if(map.is_visible(x, y) && map(x, y)->getTopGroup() == group)
              return (x + y * map.len());
        }
        for (j = 0; j < i; j++)
        {
            y++;
            if(map.is_visible(x, y) && map(x, y)->getTopGroup() == group)
              return (x + y * map.len());
        }
    }
    return (-1);
}

/*
   // spiral round from startx,starty until we hit a 2x2 space.
   // return the x y coords encoded as x+y*world.len()
   // return -1 if we don't find one.
 */
bool
World::is_bare_area(int x, int y, int size) {
    for(int j = 0; j<size; j++)
    {
        for(int i = 0; i<size; i++)
        {
            if(!map.is_visible(x+i, y+j) || !map(x+i, y+j)->is_bare())
              return false;
        }
    }
    return true;
}

int
World::find_bare_area(int x, int y, int size) {
    int i, j;
    for(i = 1; i < (2 * map.len()); i++) {
        for (j = 0; j < i; j++)
        {
            x--;
            if(map.is_visible(x, y) && is_bare_area(x, y, size))
              return x + y * map.len();
        }
        for (j = 0; j < i; j++) {
            y--;
            if(map.is_visible(x, y) && is_bare_area(x, y, size))
              return x + y * map.len();
        }
        i++;
        for (j = 0; j < i; j++) {
            x++;
            if(map.is_visible(x, y) && is_bare_area(x, y, size))
              return x + y * map.len();
        }
        for (j = 0; j < i; j++) {
            y++;
            if(map.is_visible(x, y) && is_bare_area(x, y, size))
              return x + y * map.len();
        }
    }
    return (-1);
}

/** @file lincity/engine.cpp */
