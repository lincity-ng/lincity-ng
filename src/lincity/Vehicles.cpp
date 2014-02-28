/* ---------------------------------------------------------------------- *
 * Vehicles.cpp
 * This file is part of lincity-NG.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#include "Vehicles.h"
#include <stdlib.h>
#include "engine.h"

std::list<Vehicle*> Vehicle::vehicleList;

Vehicle::Vehicle(int x0, int y0, VehicleModel model0)
{
    this->x = x0;
    this->y = y0;
    anim = 0;
    //TODO make more vehicles for different types of stuff
    this->stuff_id = Construction::STUFF_JOBS;
    //TODO Choose a random model for suitable stuff
    this->model = model0;
    vehicleList.push_back(this);
    frameIt = world(x,y)->createframe();
    frameIt->frame = -1;
    switch(model)
    {
        case (VEHICLE_BLUECAR):
            frameIt->resourceGroup =  ResourceGroup::resMap["Bluecar"];
            break;
        default:
        std::cout << "default in new vehicle model = " << model << " at x= " << x << " y= " << y << std::endl;

    }
    std::cout << "new vehicle model = " << model << " at x= " << x << " y= " << y << std::endl;
}

Vehicle::~Vehicle()
{
#ifdef DEBUG
        bool found = false;
#endif
        std::list<Vehicle*>::iterator it;
        for(it = Vehicle::vehicleList.begin(); it != Vehicle::vehicleList.end() ; ++it)
        {
            if(*it == this)
            {
                Vehicle::vehicleList.erase(it);
#ifdef DEBUG
                found = true;
#endif
                break;
            }
        }
#ifdef DEBUG
        if (!found)
        {   std::cout << "Error in ~Vehile at x= " << x << " y= " << y << std::endl;}
        assert(found);
#endif
        world(x,y)->killframe(frameIt);
        std::cout << "kill vehicle model= " << model << " at x= " << x << " y= " << y << std::endl;
}

void Vehicle::clearVehicleList()
{
    while(! vehicleList.empty())
    {   delete vehicleList.front();}
}

void Vehicle::move_to(int new_x, int new_y)
{

    if(!world(new_x, new_y)->framesptr)
    {   world(new_x, new_y)->framesptr = new std::list<ExtraFrame>;}
    world(new_x, new_y)->framesptr->splice(world(new_x, new_y)->framesptr->end(), *(world(x,y)->framesptr), frameIt);

    this->x = new_x;
    this->y = new_y;
}


void Vehicle::update()
{

    int s = (int)frameIt->resourceGroup->graphicsInfoVector.size();
    if((frameIt->frame < 0) && s)
    {   frameIt->frame = rand() % s;}

    if( real_time > anim)
    {
        switch(model)
        {
            case (VEHICLE_BLUECAR):
                anim = real_time + BLUE_CAR_SPEED;
                break;
            default:
            std::cout << "default in update vehicle model = " << model << " at x= " << x << " y= " << y << std::endl;

        }

        if(++(frameIt->frame) >= s)
        {   frameIt->frame = 0;}

        int test_x = x + rand() % 3 - 1;
        int test_y = (test_x == x)? y + rand() % 3 - 1 : y;
        unsigned short g = world(test_x, test_y)->getTransportGroup();
        if( (g == GROUP_TRACK) || (g == GROUP_ROAD) )
        {   move_to(test_x, test_y);}


    }
}























