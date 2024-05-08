/* ---------------------------------------------------------------------- *
 * world.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __world_h__
#define __world_h__

#include <iostream>       // for ostream
#include <list>           // for list
#include <set>            // for set
#include <vector>         // for vector

#include "resources.hpp"  // for ExtraFrame, ResourceGroup (ptr only)

class Construction;
class ConstructionGroup;

#define WORLD_SIDE_LEN 100

//Array2D is used during map generation in initgame
template <class T>
class Array2D {
public:
  Array2D(int lenx, int leny) {
    this->lenx = lenx;
    this->leny = leny;
    matrix.resize(lenx * leny, (T)0);
  }
  ~Array2D() {
    matrix.clear();
  }
  void initialize(T init) {
    for(int index = 0; index < lenx * leny; index++) {
      matrix[index] = init;
    }
  }
  T* operator()(int x, int y) {
    return &(matrix[x + y * lenx]);
  }
  T* operator()(int index) {
    return &(matrix[index]);
  }
  bool is_inside(int x, int y) {
    return x >= 0 && x < lenx && y >= 0 && y < leny;
  }
  bool is_inside(int index) {
    return index >= 0 && index < lenx * leny;
  }

protected:
  int lenx, leny;
  std::vector<T> matrix;
};

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
  MapTile();
  ~MapTile();
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

  unsigned short getType();          //type of bare land or the covering construction
  unsigned short getTopType();       //type of bare land or the actual construction
  unsigned short getLowerstVisibleType(); //like getType but type of terrain underneath transparent constructions
  unsigned short getGroup();        //group of bare land or the covering construction
  unsigned short getTopGroup();     //group of bare land or the actual construction
  unsigned short getLowerstVisibleGroup(); //like getGroup but group of terrain underneath transparent constructions
  unsigned short getTransportGroup(); //like getGroup but bridges are reported normal transport tiles
  ConstructionGroup* getTileConstructionGroup(); //constructionGroup of the maptile
  ResourceGroup*     getTileResourceGroup();     //resourceGroup of a tile
  ConstructionGroup* getConstructionGroup();     //constructionGroup of maptile or the covering construction
  ConstructionGroup* getTopConstructionGroup();  //constructionGroup of maptile or the actual construction
  ConstructionGroup* getLowerstVisibleConstructionGroup();

  bool is_bare();                    //true if we there is neither a covering construction nor water
  bool is_lake();                    //true on lakes (also under bridges)
  bool is_river();                   //true on rivers (also under bridges)
  bool is_water();                   //true on bridges or lakes (also under bridges)
  bool is_visible();                 //true if tile is not covered by another construction. Only useful for minimap Gameview is rotated to upperleft
  bool is_transport();               //true on tracks, road, rails and bridges
  bool is_residence();               //true if any residence covers the tile
  void writeTemplate();              //create maptile template
  void saveMembers(std::ostream *os);//write maptile AND ground members as XML to stram
};

class World
{
public:
  World(int map_len);
  ~World();
  MapTile* operator()(int x, int y);
  MapTile* operator()(int index);
  bool is_inside(int x, int y);
  bool is_inside(int index);
  bool is_border(int x, int y);
  bool is_border(int index);
  bool is_edge(int x, int y);
  bool is_visible(int x, int y);
  int map_x(MapTile *tile);// returns x
  int map_y(MapTile *tile);// returns y
  int map_index(MapTile *tile);// returns index
  int len(void); //tells the actual world.side_len
  void len(int new_len); //resizes the world by edge
  bool maximum(int x , int y);
  bool minimum(int x , int y);
  bool saddlepoint(int x , int y);
  bool checkEdgeMin(int x , int y);
  int count_altered();
  bool dirty;
  int seed(void); //tells recreation seed
  void seed(int new_seed); //sets the seed
  int old_setup_ground;
  int climate;
  std::set<int> polluted;
  bool without_trees;

protected:
  int id;
  int side_len;
  std::vector<MapTile> maptile;
};





#endif /* __world_h__ */

/** @file lincity/world.h */
