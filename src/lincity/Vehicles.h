/* ---------------------------------------------------------------------- *
 * Vehicles.hpp
 * This file is part of lincity-NG.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __Vehicles_h__
#define __Vehicles_h__

#include "lintypes.h"
#include <list>

#define BLUE_CAR_SPEED 1000;


class ExtraFrame;

enum VehicleModel
{
    VEHICLE_BLUECAR,
    VEHICLE_DEFAULT
};

class Vehicle
{
public:

    Vehicle(int x0, int y0, VehicleModel model0);

    ~Vehicle(void);

    //location, heading and comming from
    int x, xnext, xprev;
    int y, ynext, yprev;
    int death_counter;
    bool alive;
    bool step_forward;
    unsigned int headings;

    VehicleModel model; //different vehicles
    std::list<ExtraFrame>::iterator frameIt; //the particular extraframe at the host
    int map_idx;    //index of the maptile with the frame, NOT necessarily the current position
    Construction::Commodities stuff_id;
    int anim; //speed of the car;
    void update();
    void drive();
    void move_frame(int idx);

    static std::list<Vehicle*> vehicleList;

    static void clearVehicleList(); //kill all vehicles
    static void cleanVehicleList(); //kill vehicles with deathcounter < 0
};

#endif

