/* ---------------------------------------------------------------------- *
 * Vehicles.hpp
 * This file is part of lincity-NG.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __Vehicles_h__
#define __Vehicles_h__

#include "lintypes.h"
#include <list>

#define BLUE_CAR_SPEED 1500
#define TRACK_BRIDGE_HEIGHT 22
#define ROAD_BRIDGE_HEIGHT 44

#define COMMUTER_TRAFFIC_RATE 1024


class ExtraFrame;

enum VehicleModel
{
    VEHICLE_BLUECAR,
    VEHICLE_DEFAULT
};

enum VehicleStrategy
{
    VEHICLE_STRATEGY_MAXIMIZE, //go towards more stuff eg. morning commute for STUFF_JOBS
    VEHICLE_STRATEGY_MINIMIZE, //go towards less stuff eg. evening commute for STUFF_JOBS
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
    Construction::Commodities stuff_id; //cargo
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

