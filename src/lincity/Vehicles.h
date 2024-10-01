/* ---------------------------------------------------------------------- *
 * src/lincity/Vehicles.h
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

#include <list>           // for list

#include "resources.hpp"  // for ExtraFrame

enum Commodity : int;

#define BLUE_CAR_SPEED 1500
#define TRACK_BRIDGE_HEIGHT 22
#define ROAD_BRIDGE_HEIGHT 44

#define COMMUTER_TRAFFIC_RATE 1024

enum VehicleModel
{
    VEHICLE_BLUECAR,
    VEHICLE_DEFAULT
};

enum VehicleStrategy
{
    VEHICLE_STRATEGY_MAXIMIZE, //go towards more stuff eg. morning commute for STUFF_LABOR
    VEHICLE_STRATEGY_MINIMIZE, //go towards less stuff eg. evening commute for STUFF_LABOR
    VEHICLE_STRATEGY_RANDOM    //just do a random walk
};

class Vehicle
{
public:

    Vehicle(int x0, int y0, VehicleModel model0, VehicleStrategy vehicleStrategy = VEHICLE_STRATEGY_RANDOM );

    ~Vehicle(void);

    //location, heading and comming from
    int x, xnext, xprev, xold1, xold2;
    int y, ynext, yprev, yold1, yold2;
    float xr, yr;
    int death_counter;
    bool alive, turn_left;
    unsigned int headings;
    int direction;

    VehicleModel model; //different vehicles
    Commodity stuff_id; //cargo
    int initial_cargo;
    VehicleStrategy strategy; // delivery, pickup, random
    std::list<ExtraFrame>::iterator frameIt; //the particular extraframe at the host
    int map_idx;    //index of the maptile with the frame, NOT necessarily the current position

    int speed0, speed, anim;
    void update();


    static std::list<Vehicle*> vehicleList;

    static void clearVehicleList(); //kill all vehicles
    static void cleanVehicleList(); //kill vehicles with deathcounter < 0
private:
    void getNewHeadings(); //plan ahead for 2 tiles
    bool acceptable_heading(int idx); //checks if a move would comply with the strategy
    void drive();          //advance position by 1 tile
    void walk();           //change the offset of the sprite and evetually choose a tile to attach it to
    void move_frame(int idx); //place the frame on the map aka *world(idx)


};

#endif
