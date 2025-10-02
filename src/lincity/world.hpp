/* ---------------------------------------------------------------------- *
 * src/lincity/world.hpp
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

#ifndef __world_h__
#define __world_h__

#include <array>            // for array
#include <deque>            // for deque
#include <filesystem>       // for path
#include <iostream>         // for ostream
#include <list>             // for list
#include <memory>           // for unique_ptr
#include <optional>
#include <set>              // for set
#include <string>           // for basic_string, string
#include <unordered_set>    // for unordered_set
#include <vector>           // for vector

#include "MapPoint.hpp"     // for MapPoint
#include "all_buildings.hpp"  // for COAL_TAX_RATE, DOLE_RATE, GOODS_TAX_RATE
#include "commodities.hpp"  // for Commodity, CommodityRule (ptr only)
#include "messages.hpp"     // for Message
#include "resources.hpp"    // for ExtraFrame, ResourceGroup (ptr only)
#include "stats.hpp"          // for Stat, Stats

class Construction;
class ConstructionGroup;
class Vehicle;

#define WORLD_SIDE_LEN 100

class Ground {
public:
  Ground();
  ~Ground();
  int altitude;       //surface of ground. unused currently
  int ecotable;       //done at init time: pointer to the table for vegetation
  int wastes;         //wastes underground
  int pollution;      //pollution underground
  int water_alt;      //altitude of water (needed to know drainage basin)
  int water_pol;      //pollution of water
  int water_wast;     //wastes in water
  int water_next;     //next tile(s) where the water will go from here
  int int1;           //reserved for future (?) use
  int int2;
  int int3;
  int int4;
};

class MapTile {
public:
  MapTile(MapPoint point);
  ~MapTile();

  // TODO: make point const. Attention to map_len assignment in load/save.
  //       Option 1: refactor load/save so World::map needn't be assigned.
  //       Option 2: mess with the allocator for the Map::maptile vector.
  //       see https://en.cppreference.com/w/cpp/container/vector/operator=
  //       Option 3: convert Map::maptile to a vector<unique_ptr<MapTile>>
  MapPoint point;
  Ground ground;                        //the Ground associated to an instance of MapTile
  Construction *construction;           //the actual construction (e.g. for simulation)
  Construction *reportingConstruction;  //the construction covering the tile
  unsigned short type;                  //type of terrain (underneath constructions)
  unsigned short group;                 //group of the terrain (underneath constructions)
  unsigned int flags;                   //flags are defined in lin-city.h
  // TODO: prevent access to coal_reserve when coal_survey_done == false
  unsigned short coal_reserve;          //underground coal
  unsigned short ore_reserve;           //underground ore
  int pollution;                        //air pollution (under ground pollution is in ground[][])
  std::list<ExtraFrame> *framesptr;     //Overlays to be rendered on top of type, mostly NULL
                                        //use memberfunctions to add and remove sprites

  void setTerrain(unsigned short group); //places type & group at MapTile
  std::list<ExtraFrame>::iterator createframe(); //creates new empty ExtraFrames
                                                  //to be used by Contstructions and Vehicles
  void killframe(const std::list<ExtraFrame>::iterator& it); //kills an extraframe

  unsigned short getType() const;          //type of bare land or the covering construction
  unsigned short getTopType() const;       //type of bare land or the actual construction
  unsigned short getLowerstVisibleType() const ; //like getType but type of terrain underneath transparent constructions
  unsigned short getGroup() const;        //group of bare land or the covering construction
  unsigned short getTopGroup() const;     //group of bare land or the actual construction
  unsigned short getLowerstVisibleGroup() const; //like getGroup but group of terrain underneath transparent constructions
  unsigned short getTransportGroup() const; //like getGroup but bridges are reported normal transport tiles
  ConstructionGroup* getTileConstructionGroup() const; //constructionGroup of the maptile
  ResourceGroup*     getTileResourceGroup() const;     //resourceGroup of a tile
  ConstructionGroup* getConstructionGroup() const;     //constructionGroup of maptile or the covering construction
  ConstructionGroup* getTopConstructionGroup() const;  //constructionGroup of maptile or the actual construction
  ConstructionGroup* getLowerstVisibleConstructionGroup() const;

  bool is_bare() const;                    //true if we there is neither a covering construction nor water
  bool is_lake() const;                    //true on lakes (also under bridges)
  bool is_river() const;                   //true on rivers (also under bridges)
  bool is_water() const;                   //true on bridges or lakes (also under bridges)
  bool is_visible() const;                 //true if tile is not covered by another construction. Only useful for minimap Gameview is rotated to upperleft
  bool is_transport() const;               //true on tracks, road, rails and bridges
  bool is_residence() const;               //true if any residence covers the tile
  void writeTemplate();              //create maptile template
  void saveMembers(std::ostream *os);//write maptile AND ground members as XML to stram
};

class Map {
public:
  Map(int map_len);
  ~Map();
  const MapTile* operator()(MapPoint point) const;
  MapTile* operator()(MapPoint point);
  bool is_inside(MapPoint loc) const;
  bool is_border(MapPoint loc) const;
  bool is_edge(MapPoint point) const;
  bool is_visible(MapPoint loc) const;
  int len() const; //tells the actual world.side_len
  bool maximum(MapPoint point) const;
  bool minimum(MapPoint point) const;
  bool saddlepoint(MapPoint point) const;
  bool checkEdgeMin(MapPoint point) const;
  std::vector<MapPoint> polluted;
  using iterator = std::vector<MapTile>::iterator;
  using riterator = std::vector<MapTile>::reverse_iterator;
  using citerator = std::vector<MapTile>::const_iterator;
  using criterator = std::vector<MapTile>::const_reverse_iterator;
  iterator begin();
  iterator end();
  riterator rbegin();
  riterator rend();
  citerator cbegin() const;
  citerator cend() const;
  criterator crbegin() const;
  criterator crend() const;
  citerator begin() const;
  citerator end() const;
  criterator rbegin() const;
  criterator rend() const;

  int alt_min, alt_max, alt_step;

  // Using std::set instead of std::unordered_set so iterators remain valid
  // after insertion.
  std::set<Construction *> constructions;

  MapPoint recentPoint;


  std::optional<MapPoint> find_group(MapPoint p, unsigned short group);
  std::optional<MapPoint> find_bare_area(MapPoint p, int size);
  void connect_transport(int originx, int originy, int lastx, int lasty);
  void desert_water_frontiers(MapPoint p0, MapPoint p1);
  void desert_water_frontiers(int originx, int originy, int w, int h);
  void connect_rivers(int x, int y);

protected:
  int side_len;
  std::vector<MapTile> maptile;

private:

  bool is_bare_area(MapPoint point, int size);
};

class World {
public:
  World();
  World(int mapSize);
  ~World();

  void save(const std::filesystem::path& filename) const;
  static std::unique_ptr<World> load(const std::filesystem::path& filename);

  void do_time_step();
  void do_animate(unsigned long real_time);
  void buildConstruction(ConstructionGroup& cstGrp, MapPoint point);
  void bulldozeArea(MapPoint point);
  void evacuateArea(MapPoint point);
  void floodArea(MapPoint point);

  void pushMessage(Message::ptr message);
  Message::ptr popMessage();

  enum class Updatable {
    POPULATION,
    TECH,
    MONEY,
    FOOD,
    LABOR,
    GOODS,
    COAL,
    ORE,
    STEEL,
    POLLUTION,
    LOVOLT,
    HIVOLT,
    WATER,
    WASTE,
    TIME,
    MAP,
    SUSTAINABILITY,
  };
  void setUpdated(Updatable what);
  void clearUpdated(Updatable what);
  bool isUpdated(Updatable what);

// private: // planning to remove from public API

  Map map;

  std::string given_scene;

  int total_time;  // game time

  int people_pool;

  int total_money;

  struct {
    int income_tax = INCOME_TAX_RATE;
    int coal_tax = COAL_TAX_RATE;
    int dole = DOLE_RATE; // unemployment
    int transport_cost = TRANSPORT_COST_RATE;
    int goods_tax = GOODS_TAX_RATE;
    int export_tax = 0; // unused for now
    int import_cost = IM_PORT_COST_RATE;
  } money_rates;

  struct {
    int labor = 0;
    int coal = 0;
    int goods = 0;
    int trade_ex = 0;
    // int trade_im; // import is payed for directly
  } taxable;

  int tech_level;


  int rockets_launched, rockets_launched_success;
  int coal_survey_done;
  bool gameEnd;

  std::array<CommodityRule, STUFF_COUNT> tradeRule;

  Stats stats;

  std::list<Vehicle*> vehicleList;



  void income(int amt, Stat<int>& account);
  void expense(int amt, Stat<int>& account, bool allowCredit = true);
  void place_item(ConstructionGroup& cstGrp, MapPoint loc);
  void do_pollution();
  void scan_pollution();
  void do_fire_health_cricket_power_cover();
  void do_random_fire();
  void do_daily_ecology();
  void do_coal_survey();

private:
  std::deque<Message::ptr> messageQueue;
  std::unordered_set<Updatable> updatedSet;

  void do_periodic_events();
  void end_of_month_update();
  void start_of_year_update();
  void end_of_year_update();
  void simulate_mappoints();
  void sustainability_test();
  bool sust_fire_cover();
};



#endif /* __world_h__ */

/** @file lincity/world.h */
