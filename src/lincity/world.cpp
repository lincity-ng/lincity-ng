/* ---------------------------------------------------------------------- *
 * src/lincity/world.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2005      Matthias Braun <matze@braunis.de>
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

#include "world.hpp"

#include <stdlib.h>               // for NULL
#include <iostream>               // for basic_ostream, operator<<, basic_os...
#include <iterator>               // for advance
#include <map>                    // for map
#include <string>                 // for basic_string, char_traits, operator<

#include "ConstructionRequest.hpp"  // for BurnDownRequest, ConstructionDeleti...
#include "all_buildings.hpp"        // for GROUP_WATER_COST
#include "engglobs.hpp"             // for dxo, dyo
#include "groups.hpp"               // for GROUP_WATER, GROUP_BARE, GROUP_DESERT
#include "lctypes.hpp"              // for CST_GREEN, CST_DESERT
#include "lin-city.hpp"             // for FLAG_IS_RIVER, FLAG_TRANSPARENT
#include "lintypes.hpp"             // for Construction, ConstructionGroup
#include "modules/fire.hpp"         // for fireConstructionGroup
#include "modules/market.hpp"       // for Market
#include "modules/monument.hpp"     // for Monument
#include "modules/tile.hpp"       // for TileConstructionGroup, desertConstr...
#include "modules/tip.hpp"          // for Tip
#include "resources.hpp"          // for ExtraFrame, ResourceGroup

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
  side_len(map_len), maptile(map_len * map_len),
  recentPoint(map_len / 2, map_len / 2)
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

const MapTile *Map::operator()(MapPoint point) const {
  return &(maptile[point.x + point.y * side_len]);
}

MapTile *Map::operator()(MapPoint point) {
  return &(maptile[point.x + point.y * side_len]);
}

bool Map::is_inside(int x, int y) const {
    return (x >= 0 && y >= 0 && x < side_len && y < side_len);
}

bool Map::is_inside(int index) const {
    return (index >= 0 && index < side_len * side_len);
}

bool Map::is_inside(MapPoint point) const {
  return point.x >= 0 && point.y >= 0
    && point.x < side_len && point.y < side_len;
}

bool Map::is_border(int x, int y) const {
    return (x == 0 || y == 0 || x == side_len-1 || y == side_len -1);
}

bool Map::is_border(int index) const {
    return (index%side_len == side_len -1 || index%side_len == 0 || index/side_len == side_len-1 || index/side_len == 0);
}

bool Map::is_border(MapPoint point) const {
  return (point.x == 0 || point.y == 0
    || point.x == side_len-1 || point.y == side_len-1);
}

bool Map::is_edge(int x, int y) const {
  return is_edge(MapPoint(x, y));
}

bool Map::is_edge(MapPoint point) const {
  return (point.x == 1 || point.y == 1
    || point.x == side_len-2 || point.y == side_len -2);
}

bool
Map::is_visible(int x, int y) const {
  return is_visible(MapPoint(x, y));
}

bool
Map::is_visible(MapPoint point) const {
  return (point.x > 0 && point.y > 0
    && point.x < side_len-1 && point.y < side_len-1);
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

bool Map::maximum(int x , int y) const {
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
bool Map::maximum(MapPoint point) const {
  return maximum(point.x, point.y);
}

bool Map::minimum(int x , int y) const {
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
bool Map::minimum(MapPoint point) const {
  return minimum(point.x, point.y);
}

bool Map::saddlepoint(int x , int y) const {
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
bool Map::saddlepoint(MapPoint point) const {
  return saddlepoint(point.x, point.y);
}

bool Map::checkEdgeMin(int x , int y) const {
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
bool Map::checkEdgeMin(MapPoint point) const {
  return checkEdgeMin(point.x, point.y);
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

void
World::buildConstruction(ConstructionGroup& cstGrp, MapPoint point) {
  Message::ptr message;
  if(!cstGrp.can_build(*this, message))
    CannotBuildMessage::create(cstGrp, message)->throwEx();
  if(!cstGrp.can_build_here(*this, point, message))
    CannotBuildHereMessage::create(cstGrp, point, message)->throwEx();
  expense(cstGrp.getCosts(*this), stats.expenses.construction,
    !cstGrp.no_credit);
  cstGrp.placeItem(*this, point.x, point.y);

  map.connect_transport(point.x - 2, point.y - 2,
    point.x + cstGrp.size + 1, point.y + cstGrp.size + 1);
  map.desert_water_frontiers(point.x - 1, point.y - 1,
    cstGrp.size + 2, cstGrp.size + 2);
}

void
World::bulldozeArea(MapPoint point) {
  if(!map.is_visible(point))
    OutsideMapMessage::create(point)->throwEx();

  Construction *cst = map(point)->reportingConstruction;
  unsigned short g = map(point)->getGroup();

  // TODO: delegate check to the construction
  if(g == GROUP_DESERT)
    return; // nothing to do
  else if (g == GROUP_FIRE)
    CannotBulldozeThisMessage::create(point, fireConstructionGroup)->throwEx();
  else if (g == GROUP_MONUMENT
    && !dynamic_cast<Monument *>(cst)->completed
  )
    CannotBulldozeIncompleteMonumentMessage::create(point)->throwEx();
  else if (g == GROUP_TIP
    && !dynamic_cast<Tip *>(cst)->total_waste > 0
  )
    CannotBulldozeNonemptyTipMessage::create(point)->throwEx();

  expense(map(point)->getConstructionGroup()->bul_cost,
    stats.expenses.construction);

  // TODO: delegate special cases to the construction
  if(g == GROUP_SHANTY)
    BurnDownRequest(cst).execute();
  else if (g == GROUP_OREMINE)
    OreMineDeletionRequest(cst).execute();
  else if(cst)
    ConstructionDeletionRequest(cst).execute();
  else {
    map(point)->flags &= ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
    if (map(point)->is_water()) {
      map(point)->type = CST_GREEN;
      map(point)->group = GROUP_BARE;
      map(point)->flags &= ~(FLAG_IS_RIVER);
    }
    else {
      map(point)->type = CST_DESERT;
      map(point)->group = GROUP_DESERT;
    }
    //Here size is always 1
    map.connect_transport(point.x - 2, point.y - 2, point.x + 2, point.y + 2);
    map.connect_rivers(point.x, point.y);
    map.desert_water_frontiers(point.x - 1, point.y - 1, 1 + 2, 1 + 2);
  }

  setUpdated(Updatable::MAP);
}

void
World::evacuateArea(MapPoint point) {
  if(!map.is_visible(point))
    OutsideMapMessage::create(point)->throwEx();
  Construction *cst = map(point)->reportingConstruction;
  if(!cst)
    NothingHereMessage::create(point)->throwEx();
  if(cst->flags & FLAG_NEVER_EVACUATE)
    CannotEvacuateThisMessage::create(point,
      *cst->constructionGroup)->throwEx();

  if(cst->constructionGroup->group == GROUP_MARKET) {
    dynamic_cast<Market*>(cst)->toggleEvacuation();
    return;
  }
  cst->flags ^= FLAG_EVACUATE;
}

void
World::floodArea(MapPoint point) {
  if(!map.is_visible(point))
    OutsideMapMessage::create(point)->throwEx();
  if(!map(point)->is_bare())
    SpaceOccupiedMessage::create(point)->throwEx();
  map(point)->setTerrain(GROUP_WATER);
  expense(GROUP_WATER_COST, stats.expenses.construction);
  map.connect_transport(point.x - 2, point.y - 2, point.x + 2, point.y + 2);
  map.desert_water_frontiers(point.x - 1, point.y - 1, 3, 3);
  map.connect_rivers(point.x, point.y);
  setUpdated(Updatable::MAP);
}

void
World::pushMessage(Message::ptr message) {
  messageQueue.push_back(message);
}

Message::ptr
World::popMessage() {
  if(messageQueue.empty())
    return Message::ptr();
  Message::ptr message = messageQueue.front();
  messageQueue.pop_front();
  return message;
}

void
World::setUpdated(Updatable what) {
  updatedSet.insert(what);
}

void
World::clearUpdated(Updatable what) {
  updatedSet.erase(what);
}

bool
World::isUpdated(Updatable what) {
  return updatedSet.find(what) != updatedSet.end();
}

/** @file lincity/world.cpp */
