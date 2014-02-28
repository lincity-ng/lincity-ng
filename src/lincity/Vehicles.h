/* ---------------------------------------------------------------------- *
 * Vehicles.hpp
 * This file is part of lincity-NG.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __Vehicles_h__
#define __Vehicles_h__

#include "lintypes.h"
#include <list>

#define BLUE_CAR_SPEED 500;


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

    int x;
    int y;
    VehicleModel model; //different vehicles
    std::list<ExtraFrame>::iterator frameIt; //the particular extraframe at the host
    Construction::Commodities stuff_id;
    int anim; //speed of the car;
    void update();
    void move_to(int new_x, int new_y);

    static std::list<Vehicle*> vehicleList;

    static void clearVehicleList();
};

#endif

