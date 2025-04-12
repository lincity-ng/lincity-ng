/* ---------------------------------------------------------------------- *
 * src/lincity/Vehicles.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
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

#ifndef __Vehicles_h__
#define __Vehicles_h__

#include <list>  // for list

#include "MapPoint.hpp"

class World;
enum Commodity : int;
struct ExtraFrame;

#define BLUE_CAR_SPEED 1500
#define TRACK_BRIDGE_HEIGHT 22
#define ROAD_BRIDGE_HEIGHT 44

#define COMMUTER_TRAFFIC_RATE 1024

enum VehicleModel {
  VEHICLE_BLUECAR,
  VEHICLE_DEFAULT
};

enum VehicleStrategy {
  VEHICLE_STRATEGY_MAXIMIZE, //go towards more stuff eg. morning commute for STUFF_LABOR
  VEHICLE_STRATEGY_MINIMIZE, //go towards less stuff eg. evening commute for STUFF_LABOR
  VEHICLE_STRATEGY_RANDOM    //just do a random walk
};

class Vehicle {
public:
  Vehicle(World& world, MapPoint point, VehicleModel model0,
    VehicleStrategy vehicleStrategy = VEHICLE_STRATEGY_RANDOM);

  ~Vehicle(void);

  World& world;
  //location, heading and comming from
  MapPoint point, next, prev, old1, old2;
  float xr, yr;
  int death_counter;
  bool turn_left;
  unsigned int headings;
  int direction;

  VehicleModel model; //different vehicles
  Commodity stuff_id; //cargo
  int initial_cargo;
  VehicleStrategy strategy; // delivery, pickup, random
  std::list<ExtraFrame>::iterator frameIt; //the particular extraframe at the host
  MapPoint framePt; //index of the maptile with the frame, NOT necessarily the current position

  int speed0, speed, anim;
  void update(unsigned long real_time);


  static std::list<Vehicle*> vehicleList;

  static void cleanVehicleList(); //kill vehicles with deathcounter < 0
private:
  void getNewHeadings(); //plan ahead for 2 tiles
  bool acceptable_heading(MapPoint dest); //checks if a move would comply with the strategy
  void drive();          //advance position by 1 tile
  void walk(unsigned long real_time);           //change the offset of the sprite and evetually choose a tile to attach it to
  void move_frame(MapPoint newPoint); //place the frame on the map aka *world(idx)
};

#endif
