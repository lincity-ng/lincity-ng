/* ---------------------------------------------------------------------- *
 * src/lincity/world.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2005      Matthias Braun <matze@braunis.de>
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

#include "world.h"

#include <stdlib.h>         // for NULL
#include <iostream>         // for basic_ostream, char_traits, operator<<
#include <iterator>         // for advance
#include <map>              // for map
#include <string>           // for basic_string, operator<

#include "all_buildings.h"  // for TileConstructionGroup, desertConstruction...
#include "engglobs.h"       // for world, dxo, dyo, world_id
#include "groups.h"         // for GROUP_WATER, GROUP_BARE, GROUP_DESERT
#include "init_game.h"      // for clear_game
#include "lin-city.h"       // for FLAG_TRANSPARENT, FLAG_IS_RIVER, FLAG_HAS...
#include "lintypes.h"       // for Construction, ConstructionGroup
#include "resources.hpp"    // for ExtraFrame, ResourceGroup
#include "modules/tile.hpp"

#ifdef DEBUG
#include <cassert>          // for assert
#endif

Ground::Ground() {
  altitude = 0;
  ecotable = 0;
  wastes = 0;
  pollution = 0;
  water_alt = 0;
  water_pol = 0;
  water_wast = 0;
  water_next = 0;
  int1 = 0;
  int2 = 0;
  int3 = 0;
  int4 = 0;
}
Ground::~Ground() {}

MapPoint::MapPoint(int x, int y)
  : x(x), y(y)
{}

bool
MapPoint::operator==(const MapPoint& other) const {
    return x == other.x && y == other.y;
}

MapTile::MapTile() :
  ground()
{
    construction = NULL;
    reportingConstruction = NULL;
    framesptr = NULL;
    flags = 0;
    type = CST_GREEN;
    group = GROUP_BARE;
    pollution = 0;
    ore_reserve = 0;
    coal_reserve = 0;
    pollution = 0;
}

MapTile::~MapTile()
{
    //Here the order matters
    if(construction)
    {   delete construction;}
    if (framesptr)
    {
        framesptr->clear();
        delete framesptr;
    }
}

void MapTile::setTerrain(unsigned short new_group)
{
    this->type = 0;
    this->group = new_group;
    if(new_group == GROUP_WATER)
    {   flags |= FLAG_HAS_UNDERGROUND_WATER;}
}

ConstructionGroup *
MapTile::getTileConstructionGroup() const {
    switch (group)
    {
        case GROUP_BARE:    return &bareConstructionGroup;      break;
        case GROUP_DESERT:  return &desertConstructionGroup;    break;
        case GROUP_WATER:   return &waterConstructionGroup;     break;
        case GROUP_TREE:    return &treeConstructionGroup;      break;
        case GROUP_TREE2:   return &tree2ConstructionGroup;     break;
        case GROUP_TREE3:   return &tree3ConstructionGroup;     break;
        default:
            std::cout << "invalid group of maptile at: (" << point.x <<"," << point.y << ")" << std::endl;
            return &desertConstructionGroup;
    }
}

ResourceGroup* MapTile::getTileResourceGroup() const {
    static bool initialized = false;
    static ResourceGroup* bare = 0;
    static ResourceGroup* desert = 0;
    static ResourceGroup* water = 0;
    static ResourceGroup* tree = 0;
    static ResourceGroup* tree2 = 0;
    static ResourceGroup* tree3 = 0;

    if (!initialized)
    {
        initialized = true;
        //These must match the Group names from all_modules.cpp NOT all_buldings.cpp
        bare = ResourceGroup::resMap["Green"];
        desert = ResourceGroup::resMap["Desert"];
        water = ResourceGroup::resMap["Water"];
        tree = ResourceGroup::resMap["Tree"];
        tree2 = ResourceGroup::resMap["Tree2"];
        tree3 = ResourceGroup::resMap["Tree3"];

    }
    switch (group)
    {
        case GROUP_BARE:    return bare;      break;
        case GROUP_DESERT:  return desert;    break;
        case GROUP_WATER:   return water;     break;
        case GROUP_TREE:    return tree;      break;
        case GROUP_TREE2:   return tree2;     break;
        case GROUP_TREE3:   return tree3;     break;
        default:
            std::cout << "invalid group of maptile at: (" << point.x <<"," << point.y << ")" << std::endl;
            return desert;
    }
}

ConstructionGroup* MapTile::getConstructionGroup() const //constructionGroup of bare land or the covering construction
{   return (reportingConstruction ? reportingConstruction->constructionGroup : getTileConstructionGroup());}

ConstructionGroup* MapTile::getTopConstructionGroup() const //constructionGroup of bare land or the actual construction
{   return (construction ? construction->constructionGroup : getTileConstructionGroup());}

ConstructionGroup* MapTile::getLowerstVisibleConstructionGroup() const
{
    if(!reportingConstruction || reportingConstruction->flags & FLAG_TRANSPARENT)
    {   return getTileConstructionGroup();}
    else
    {   return getConstructionGroup();}
}

unsigned short MapTile::getType() const //type of bare land or the covering construction
{   return (reportingConstruction ? reportingConstruction->frameIt->frame : type);}

unsigned short MapTile::getTopType() const //type of bare land or the actual construction
{   return (construction ? construction->frameIt->frame : type);}

unsigned short MapTile::getLowerstVisibleType() const {
    if(!reportingConstruction || reportingConstruction->flags & FLAG_TRANSPARENT)
    {   return type;}
    else
    {   return reportingConstruction->frameIt->frame;}
}

unsigned short MapTile::getGroup() const //group of bare land or the covering construction
{   return (reportingConstruction ? reportingConstruction->constructionGroup->group : group);}

unsigned short MapTile::getTransportGroup() const //group of bare land or the covering construction
{
    unsigned short grp = getGroup();
    if (is_transport())
    {
        switch(grp)
        {
            case GROUP_TRACK_BRIDGE:
                grp = GROUP_TRACK;
                break;
            case GROUP_ROAD_BRIDGE:
                grp = GROUP_ROAD;
                break;
            case GROUP_RAIL_BRIDGE:
                grp = GROUP_RAIL;
                break;
            default:
                break;
        }
    }
    return grp;
}

unsigned short MapTile::getTopGroup() const //group of bare land or the actual construction
{
    if(!construction) //simple case
    {   return group;}
    else if(construction == reportingConstruction) //regular construction
    {   return construction->constructionGroup->group;}
    else // in case a construction has a chile e.g. waste burning on transport
    {   return (reportingConstruction ? reportingConstruction->constructionGroup->group : group);}
}

unsigned short MapTile::getLowerstVisibleGroup() const
{
    if(!reportingConstruction || reportingConstruction->flags & FLAG_TRANSPARENT)
    {   return group;}
    else
    {   return reportingConstruction->constructionGroup->group;}
}


bool MapTile::is_bare() const //true if we there is neither a covering construction nor water
{   return (!reportingConstruction) && (group != GROUP_WATER);}

bool MapTile::is_water() const //true on bridges or lakes (also under bridges)
{   return (group == GROUP_WATER);}

bool MapTile::is_lake() const //true on lakes (also under bridges)
{   return (group == GROUP_WATER) && !(flags & FLAG_IS_RIVER);}

bool MapTile::is_river() const // true on rivers (also under bridges)
{   return (flags & FLAG_IS_RIVER);}

bool MapTile::is_visible() const // true if tile is not covered by another construction. Only useful for minimap Gameview is rotated to upperleft
{   return (construction || !reportingConstruction);}

bool MapTile::is_transport() const //true on tracks, road, rails and bridges
{   return (reportingConstruction && reportingConstruction->flags & FLAG_IS_TRANSPORT);}

bool MapTile::is_residence() const //true on residences
{
    return (reportingConstruction &&(
        (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_LL)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_ML)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_HL)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_LH)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_MH)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_HH) ) );
}

std::list<ExtraFrame>::iterator MapTile::createframe(void)
{
    if(!framesptr)
    {   framesptr = new std::list<ExtraFrame>;}
    framesptr->resize(framesptr->size() + 1);
    std::list<ExtraFrame>::iterator frit = framesptr->end();
    std::advance(frit, -1);
    return frit; //the last position
}

void MapTile::killframe(std::list<ExtraFrame>::iterator it)
{
    //what would actually happen if "it" belongs to another maptile?
    framesptr->erase(it);
    if (framesptr->empty())
    {
        delete framesptr;
        framesptr = NULL;
    }
}



Map::Map(int map_len) :
  side_len(map_len), maptile(map_len * map_len)
{}

Map::~Map() {
  maptile.clear();
}

MapTile* Map::operator()(int x, int y) {
  return &(maptile[x + y * side_len]);
}

const MapTile* Map::operator()(int x, int y) const {
  return &(maptile[x + y * side_len]);
}

MapTile* Map::operator()(int index) {
  return &(maptile[index]);
}

bool Map::is_inside(int x, int y) const {
    return (x >= 0 && y >= 0 && x < side_len && y < side_len);
}

bool Map::is_inside(int index) const {
    return (index >= 0 && index < side_len * side_len);
}

bool Map::is_border(int x, int y) const {
    return (x == 0 || y == 0 || x == side_len-1 || y == side_len -1);
}

bool Map::is_border(int index) const {
    return (index%side_len == side_len -1 || index%side_len == 0 || index/side_len == side_len-1 || index/side_len == 0);
}

bool Map::is_edge(int x, int y) const {
    return (x == 1 || y == 1 || x == side_len-2 || y == side_len -2);
}

bool Map::is_visible(int x, int y) const {
    return (x > 0 && y > 0 && x < side_len-1 && y < side_len -1);
}

int Map::map_x(MapTile * tile)
{
    return (tile-&maptile[0]) % side_len;
}

int Map::map_y(MapTile * tile)
{
    return (tile-&maptile[0]) / side_len;
}

int Map::map_index(MapTile * tile)
{
    return (tile-&maptile[0]);
}

int Map::len() const {
    return side_len;
}

bool Map::maximum(int x , int y)
{
    int alt = maptile[x + y * side_len].ground.altitude;
    bool is_max = true;
    for (int i=0; i<8; i++)
    {
        int tx = x + dxo[i];
        int ty = y + dyo[i];
        is_max &= (alt >= maptile[tx + ty * side_len].ground.altitude);
    }
    return is_max;
}

bool Map::minimum(int x , int y)
{
    int alt = maptile[x + y * side_len].ground.altitude;
    bool is_min = true;
    for (int i=0; i<8; i++)
    {
        int tx = x + dxo[i];
        int ty = y + dyo[i];
        is_min &= (alt <= maptile[tx + ty * side_len].ground.altitude);
    }
    return is_min;
}

bool Map::saddlepoint(int x , int y)
{
    int alt = maptile[x + y * side_len].ground.altitude;
    int dips = 0;
    bool dip_new = alt > maptile[x + dxo[7] + (y + dyo[7])*side_len ].ground.altitude;
    bool dip_old = dip_new;
    for (int i=0; i<8; i++)
    {
        dip_new = alt > maptile[x + dxo[i]+ (y + dyo[i])*side_len].ground.altitude;
        if (dip_new && !dip_old) //We just stepped into a valley
        {
                dips++;
        }
        dip_old = dip_new;
    }
    return dips > 1;
}

bool Map::checkEdgeMin(int x , int y)
{
    int alt = maptile[x + y * side_len].ground.altitude;
    if (x==1 || x == side_len-2)
    {
        return alt < maptile[x+1 + y * side_len].ground.altitude
            && alt < maptile[x-1 + y * side_len].ground.altitude;
    }
    else if (y==1 || y == side_len-2)
    {
        return alt < maptile[x + (y+1) * side_len].ground.altitude
            && alt < maptile[x + (y-1) * side_len].ground.altitude;
    }
    else
        return false;
}

World::World() :
  World(WORLD_SIDE_LEN)
{}

World::World(int mapSize) :
  map(mapSize)
{
  total_time = 0;
  coal_survey_done = 0;
  total_money = 0;
  tech_level = 0;
  rockets_launched = 0;
  rockets_launched_success = 0;

  people_pool = 100;

  stats.sustainability.old_population = people_pool;

  for(Commodity s = STUFF_INIT; s < STUFF_COUNT; s++) {
    tradeRule[s].take = true;
    tradeRule[s].give = true;
  }
}

World::~World() {
}

/** @file lincity/world.cpp */
