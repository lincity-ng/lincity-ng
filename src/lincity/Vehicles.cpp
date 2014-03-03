/* ---------------------------------------------------------------------- *
 * Vehicles.cpp
 * This file is part of lincity-NG.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#include "Vehicles.h"
#include <stdlib.h>
#include <vector>
#include "engine.h"

std::list<Vehicle*> Vehicle::vehicleList;

Vehicle::Vehicle(int x0, int y0, VehicleModel model0)
{
    this->x = x0;
    this->y = y0;
    this->xnext = x0;
    this->ynext = y0;
    this->xprev = x0;
    this->yprev = y0;
    this->anim = 0;
    this->death_counter = 100;
    this->alive = true;
    this->step_forward = false;
    this->headings = 0;

    //TODO make more vehicles for different types of stuff
    this->stuff_id = Construction::STUFF_JOBS;
    //TODO Choose a random model for suitable stuff
    this->model = model0;
    vehicleList.push_back(this);
    frameIt = world(x,y)->createframe();
    frameIt->frame = -1;
    map_idx = x + y * world.len();
    switch(model)
    {
        case (VEHICLE_BLUECAR):
            frameIt->resourceGroup =  ResourceGroup::resMap["Bluecar"];
            break;
        default:
        std::cout << "default in new vehicle model = " << model << " at x= " << x << " y= " << y << std::endl;

    }
    //std::cout << "new vehicle model = " << model << " at x= " << x << " y= " << y << std::endl;
}

Vehicle::~Vehicle()
{
    world(x,y)->killframe(frameIt);
    //std::cout << "kill vehicle model= " << model << " at x= " << x << " y= " << y << std::endl;
}

void Vehicle::clearVehicleList()
{
    while(! vehicleList.empty())
    {
        delete vehicleList.front();
        vehicleList.pop_front();
    }
}

void Vehicle::cleanVehicleList()
{
    for(std::list<Vehicle*>::iterator it = vehicleList.begin();
        it != vehicleList.end();
        std::advance(it,1))
    {
        if(! (*it)->alive)
        {
            delete (*it);
            *it = NULL;
        }
    }
    vehicleList.remove(NULL);
}

void Vehicle::drive(void)
{
    --death_counter;

    int s = (int)frameIt->resourceGroup->graphicsInfoVector.size();
    if(s)
    {
        int dx = xnext - xprev;
        int dy = ynext - yprev;
        if(dx == 2)
        {
            frameIt->frame = 2;
            frameIt->move_x = -28;
            frameIt->move_y = -14;
        }
        else if(dx == -2)
        {
            frameIt->frame = 6;
            frameIt->move_x = 20;
            frameIt->move_y = -10;
        }
        else if(dy == 2)
        {
            frameIt->frame = 0;
            frameIt->move_x = -2;
            frameIt->move_y = -15;
        }
        else if(dy == -2)
        {
            frameIt->frame = 4;
            frameIt->move_x = 0;
            frameIt->move_y = 0;
        }
        else if( (dx == 1) && (dy == 1) )
        {
            frameIt->frame = 1;
            if (xnext == x)
            {
                frameIt->move_x = -34;
            }
            else
            {
                 frameIt->move_x = 5;
            }
            frameIt->move_y = -10;
        }
        else if( (dx == 1) && (dy == -1) )
        {
             frameIt->frame = 3;
             frameIt->move_x = 0;
             if (ynext == y)
             {  frameIt->move_y = 6;}
             else
             {  frameIt->move_y = -12;}

         }
        else if( (dx == -1) && (dy == 1) )
        {
            frameIt->frame = 7;
            frameIt->move_x = 0;
            if (xnext == x)
            {   frameIt->move_y = -5;}
            else
            {   frameIt->move_y = -22;}
        }
        else if( (dx == -1) && (dy == -1) )
        {
            frameIt->frame = 5;
            if (xnext == x)
            {   frameIt->move_x = 30;}
            else
            {   frameIt->move_x = 0;}
            frameIt->move_y = -8;
        }
        frameIt->frame =  frameIt->frame % s;
    }
    move_frame( x + y * world.len() );
    xprev = x;
    yprev = y;
    x = xnext;
    y = ynext;
}

void Vehicle::move_frame(int new_idx)
{
    if(!world(new_idx)->framesptr)
    {   world(new_idx)->framesptr = new std::list<ExtraFrame>;}
    world(new_idx)->framesptr->splice(world(new_idx)->framesptr->end(), *(world(map_idx)->framesptr), frameIt);
    if(world(map_idx)->framesptr->empty())
    {
        delete world(map_idx)->framesptr;
        world(map_idx)->framesptr = NULL;
    }
    map_idx = new_idx; //remember where the frame was put
}


void Vehicle::update()
{

    if(x == xnext && y == ynext) //get a new heading
    {
        //std::cout << "seeking path" << std::endl;

        headings = 0;
        unsigned short g;

        g = world(x + 1, y)->getTransportGroup();
        if ( ( (g == GROUP_TRACK) || (g == GROUP_ROAD) ) && (x >= xprev))
        {   headings |= 1;}
        g = world(x - 1, y)->getTransportGroup();
        if ( ( (g == GROUP_TRACK) || (g == GROUP_ROAD) ) && (x <= xprev))
        {   headings |= 2;}
        g = world(x , y - 1)->getTransportGroup();
        if ( ( (g == GROUP_TRACK) || (g == GROUP_ROAD) ) && (y <= yprev))
        {   headings |= 4;}
        g = world(x , y + 1)->getTransportGroup();
        if ( ( (g == GROUP_TRACK) || (g == GROUP_ROAD) ) && (y >= yprev))
        {   headings |= 8;}


        int sum = (headings&1?1:0)+(headings&2?1:0)+(headings&4?1:0)+(headings&8?1:0);
        if (!sum)
        {
            alive = false;
            return;
        }

        //choose a random branch
        int choice = rand() % sum;
        int j = 0;
        int k = 0;
        for(int i = 1; i < 16; i*=2)
        {
            if(headings&i)
            {
                if(choice == j)
                {   break;}
                ++j;
            }
            ++k;
        }
        //std::cout << "headings= " << headings << " sum= " << sum << " choice= " << choice << std::endl;
        //set the next destination
        switch (k)
        {
            case 0:
                xnext = x + 1;
            break;
            case 1:
                xnext = x - 1;
            break;
            case 2:
                ynext = y - 1;
            break;
            case 3:
                ynext = y + 1;
            break;
            default:
            assert(false);
        }
        //std::cout << "heading to x= " << xnext << " y= " << ynext << std::endl;
        //now choose graphic depending on path

    }
    //Choose a sprite and an offset


    if( real_time > anim) //move to dest
    {
        switch(model)
        {
            case (VEHICLE_BLUECAR):
                anim = real_time + BLUE_CAR_SPEED;
                break;
            default:
            std::cout << "default in update vehicle model = " << model << " at x= " << x << " y= " << y << std::endl;
        }
        if(x != xnext || y != ynext)
        {   drive();}
    }

    if(death_counter < 0)
    {   alive = false;}
}























