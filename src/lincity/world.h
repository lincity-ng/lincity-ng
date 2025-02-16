/* ---------------------------------------------------------------------- *
 * src/lincity/world.h
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

#include <iostream>       // for ostream
#include <list>           // for list
#include <set>            // for set
#include <vector>         // for vector
#include <unordered_set>
#include <libxml++/parsers/textreader.h>
#include <libxml/xmlwriter.h>
#include <filesystem>
#include <memory>

#include "resources.hpp"  // for ExtraFrame, ResourceGroup (ptr only)
#include "stats.h"
#include "ConstructionCount.h"
#include "events.hpp"
#include "lctypes.h"
#include "all_buildings.h"

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

class MapPoint {
public:
  MapPoint(int x = 0, int y = 0);

  bool operator==(const MapPoint& other) const;

  MapPoint n(int dist = 1) const;
  MapPoint s(int dist = 1) const;
  MapPoint e(int dist = 1) const;
  MapPoint w(int dist = 1) const;
  MapPoint ne() const;
  MapPoint nw() const;
  MapPoint se() const;
  MapPoint sw() const;

  int x, y;
};

class MapTile {
public:
  MapTile();
  ~MapTile();
  MapPoint point;
  Ground ground;                        //the Ground associated to an instance of MapTile
  Construction *construction;           //the actual construction (e.g. for simulation)
  Construction *reportingConstruction;  //the construction covering the tile
  unsigned short type;                  //type of terrain (underneath constructions)
  unsigned short group;                 //group of the terrain (underneath constructions)
  int flags;                            //flags are defined in lin-city.h
  unsigned short coal_reserve;          //underground coal
  unsigned short ore_reserve;           //underground ore
  int pollution;                        //air pollution (under ground pollution is in ground[][])
  std::list<ExtraFrame> *framesptr;     //Overlays to be rendered on top of type, mostly NULL
                                        //use memberfunctions to add and remove sprites

  void setTerrain(unsigned short group); //places type & group at MapTile
  std::list<ExtraFrame>::iterator createframe(); //creates new empty ExtraFrames
                                                  //to be used by Contstructions and Vehicles
  void killframe(std::list<ExtraFrame>::iterator it); //kills an extraframe

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

class Map
{
public:
  Map(int map_len);
  ~Map();
  MapTile* operator()(int x, int y);
  const MapTile* operator()(int x, int y) const;
  MapTile* operator()(int index);
  MapTile* operator()(MapPoint loc);
  bool is_inside(int x, int y) const;
  bool is_inside(int index) const;
  bool is_inside(MapPoint loc) const;
  bool is_border(int x, int y) const;
  bool is_border(int index) const;
  bool is_border(MapPoint loc) const;
  bool is_edge(int x, int y) const;
  bool is_visible(int x, int y) const;
  bool is_visible(MapPoint loc) const;
  int map_x(MapTile *tile);// returns x
  int map_y(MapTile *tile);// returns y
  int map_index(MapTile *tile);// returns index
  int len() const; //tells the actual world.side_len
  bool maximum(int x, int y);
  bool minimum(int x, int y);
  bool saddlepoint(int x, int y);
  bool checkEdgeMin(int x, int y);
  int count_altered();
  std::vector<MapPoint> polluted;

  int alt_min, alt_max, alt_step;

  ConstructionCount constructionCount;

  MapPoint recentPoint;

protected:
  int side_len;
  std::vector<MapTile> maptile;
};

class World {
public:
  World();
  World(int mapSize);
  ~World();

  void buildConstruction(ConstructionGroup& cstGrp, MapPoint loc);

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

  int tech_level;


  int rockets_launched, rockets_launched_success;
  int coal_survey_done;

  std::array<CommodityRule, STUFF_COUNT> tradeRule;

  Stats stats;

  std::list<Vehicle*> vehicleList;

  void addEvent(LincityEvent *event);

  void do_time_step();
  void do_animate();

  void fire_area(MapPoint loc);
  void income(int amt, Stat<int>& account);
  void expense(int amt, Stat<int>& account, bool allowCredit = true);
  void place_item(ConstructionGroup& cstGrp, MapPoint loc);
  void bulldoze_item(MapPoint loc);
  void do_pollution();
  void scan_pollution();
  void do_fire_health_cricket_power_cover();
  void do_random_fire();
  void do_daily_ecology();
  int check_group(int x, int y);
  int check_topgroup(int x, int y);
  int check_lvgroup(int x, int y);
  bool check_water(int x, int y);
  void connect_rivers(int x, int y);
  void do_coal_survey();
  void desert_water_frontiers(int originx, int originy, int w, int h);
  int find_group(int x, int y, unsigned short group);
  bool is_bare_area(int x, int y, int size);
  int find_bare_area(int x, int y, int size);
  void connect_transport(int originx, int originy, int lastx, int lasty);

  void save(const std::filesystem::path& filename);
  static std::unique_ptr<World> load(const std::filesystem::path& filename);

private:
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
