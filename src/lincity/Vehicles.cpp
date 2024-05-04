/* ---------------------------------------------------------------------- *
 * Vehicles.cpp
 * This file is part of lincity-NG.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#include "Vehicles.h"

#include <stdlib.h>    // for NULL, rand
#include <cmath>       // for ceil
#include <iostream>    // for basic_ostream, basic_ostream::operator<<, oper...
#include <iterator>    // for advance
#include <map>         // for map
#include <string>      // for char_traits, basic_string, operator<
#include <vector>      // for vector

#include "engine.h"    // for real_time
#include "groups.h"    // for GROUP_ROAD_BRIDGE, GROUP_TRACK_BRIDGE, GROUP_ROAD
#include "lintypes.h"  // for world, ExtraFrame, MapTile, Construction, Reso...
#include "world.h"     // for World

std::list<Vehicle*> Vehicle::vehicleList;

Vehicle::Vehicle(int x0, int y0, VehicleModel model0, VehicleStrategy vehicleStrategy)
{
    this->x = x0;
    this->y = y0;
    this->xnext = x0;
    this->ynext = y0;
    this->xprev = x0;
    this->yprev = y0;
    this->xold1 = x0;
    this->yold1 = y0;
    this->xold2 = x0;
    this->yold2 = y0;
    this->anim = 0;
    this->death_counter = 100;
    this->alive = true;
    this->turn_left = true;
    this->headings = 0;
    this->direction = 0;
    this->xr = x;
    this->yr = y;

    this->stuff_id = STUFF_JOBS;
    //TODO Choose a random model for suitable stuff
    this->model = model0;
    this->strategy = vehicleStrategy;
    if (world(x,y)->reportingConstruction)
    {   this->initial_cargo = world(x,y)->reportingConstruction->tellstuff(stuff_id,-2);}
    else
    {   this->initial_cargo = -1;}

    vehicleList.push_back(this);
    frameIt = world(x,y)->createframe();
    frameIt->frame = -2; //special value to indicate fresh fast forward car
    map_idx = x + y * world.len();
    switch(model)
    {
        case (VEHICLE_BLUECAR):
            frameIt->resourceGroup =  ResourceGroup::resMap["Bluecar"];
            speed0 = BLUE_CAR_SPEED;
            break;
        default:
        std::cout << "default in new vehicle model = " << model << " at x= " << x << " y= " << y << std::endl;

    }
    speed = speed0;
/*
#ifdef DEBUG
    std::cout << "new vehicle model = " << model << " at x= " << x << " y= " << y
    << " transporting " << commodityNames[stuff_id] << std::endl;
#endif
*/
}

Vehicle::~Vehicle()
{
    world(map_idx)->killframe(frameIt);
/*
#ifdef DEBUG
    std::cout << "kill vehicle model= " << model << " at x= " << x << " y= " << y
    << " traveled distance " << (100-death_counter) << std::endl;
#endif
*/
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
    speed = speed0;
    int xstep = xnext - xprev;
    int ystep = ynext - yprev;
    if(xstep == 2)
    {   direction = 2;}
    else if(xstep == -2)
    {   direction = 6;}
    else if(ystep == 2)
    {   direction = 0;}
    else if(ystep == -2)
    {   direction = 4;}
    else if( (xstep == 1) && (ystep == 1) )
    {
        direction = 1;            //vertically down
        turn_left = (xprev == x); //turning left
    }
    else if( (xstep == 1) && (ystep == -1) )
    {
        direction = 3; //horizontally right
        turn_left = (xprev != x); //turning left and YES == is correct
    }
    else if( (xstep == -1) && (ystep == 1) )
    {
        direction = 7;
        turn_left = (xprev != x); //turning left and YES == is correct

    }
    else if( (xstep == -1) && (ystep == -1) )
    {
        direction = 5;            //vertically up
        turn_left = (xprev == x); //turning left
    }

    if(direction&1)
    {   speed = (turn_left?3:1)*speed0/2;}

    //now advance to position of the vehicle
    xold2=xold1;
    yold2=yold1;
    xold1=xprev;
    yold1=yprev;
    xprev = x;
    yprev = y;
    x = xnext;
    y = ynext;

    if (frameIt->frame < -1)
    {   ++frameIt->frame;}
    else
    {
        int s = (int)frameIt->resourceGroup->graphicsInfoVector.size();
        if(s)
        {   frameIt->frame = direction % s;}
    }
}

void Vehicle::walk()
{
    double remaining = double(anim-real_time)/speed;
    double elapsed = 1 - remaining;
    //mx,my are like continuos coordinates for the map
    //(0,0) is always the southern corner of the tiles
    double mx = 0;
    double my = 0;
    switch (direction) //pointing towards
    {
        case (0): //lower left
            mx = -0.65;
            my = -remaining;
        break;
        case(4): //upper right
            mx = -0.27;
            my = -elapsed;
        break;
        case (2): //lower right
            mx = -remaining;
            my = -0.27;
        break;
        case (6): //upper left
            mx = -elapsed;
            my = -0.65;
        break;
        case (1): //vertically down
        {
            if(turn_left)
            {
                mx = -0.65 + elapsed * elapsed;
                my = -1.13 + 1 * elapsed;
            }
            else
            {
                mx =  -1 + 0.35 * elapsed;
                my = -0.27 + 0.27 * elapsed * elapsed;
            }
        }
        break;
        case (5): //vertically up
        {
            if(turn_left)
            {
                mx = -0.27 - 0.73 * elapsed * elapsed;
                my =  -0.65 * elapsed;
            }
            else
            {
                mx = 0 - 0.23 * elapsed;
                my = -0.65 - 0.35 * elapsed * elapsed;
            }
        }
        break;
        case (3): //horizontally right
        {
            if(turn_left)
            {
                if(elapsed < 0.5)
                {
                    mx = -1 + elapsed;
                    my =  -0.27 - 0.5 * elapsed * elapsed;
                }
                else
                {
                    mx = -0.27 - 0.5 * remaining * remaining;
                    my = -1 + remaining;
                }
            }
            else
            {
                mx = -0.27 * remaining * remaining;
                my = -0.27 * elapsed;
            }

        }
        break;
        case (7): //horizontally left
        {
            if (turn_left)
            {
                mx = -0.65 * elapsed;
                my = -0.65 + 0.65 * elapsed * elapsed;
            }
            else
            {
                mx = -0.65 - 0.35 * elapsed * elapsed;
                my = -1 + 0.35 * elapsed;
            }

        }
        break;
    }

    // a car with (mx,my)==(0,0) has its center in the southern corner
    mx += 0.25;
    my += 0.25;
    //update absolute floating positions
    xr = (double)xprev + mx;
    yr = (double)yprev + my;
    //choose tile for placing the frame
    int xtile = ceil(xr);
    int ytile = ceil(yr);
    //no need to go up or left
    if( xtile < xprev ) {   xtile = xprev;  }
    if( ytile < yprev ) {   ytile = yprev;  }
    //align animation to placement of frame on map
    mx = xr - (double)xtile;
    my = yr - (double)ytile;
    if( xtile +  ytile * world.len() != map_idx)
    {   move_frame(xtile +  ytile * world.len());}

    //Apply the animation of the sprite
    //dx, dy are on screen coordinates
    int dx = (mx - my) * 64;
    int dy = (mx + my) * 32;

    //Check for bridges and ramps
    switch(world(xprev,yprev)->getGroup())
    {
        case GROUP_TRACK_BRIDGE:
            dy -= TRACK_BRIDGE_HEIGHT;
            break;
        case GROUP_ROAD_BRIDGE:
            dy -= ROAD_BRIDGE_HEIGHT;
            break;
         case GROUP_ROAD:
            if( world(xnext,ynext)->getGroup() == GROUP_ROAD_BRIDGE
            ||  world(x,y)->getGroup() == GROUP_ROAD_BRIDGE)
            {
                dy -= (elapsed * ROAD_BRIDGE_HEIGHT/ 2);
                frameIt->frame = 8 + direction/2; //going up hill
                if(world(x,y)->getGroup() == GROUP_ROAD_BRIDGE)
                {dy -= ROAD_BRIDGE_HEIGHT/ 2;}
            }
            else if(world(xold2,yold2)->getGroup() == GROUP_ROAD_BRIDGE
            ||      world(xold1,yold1)->getGroup() == GROUP_ROAD_BRIDGE)
            {
                dy -= (remaining * ROAD_BRIDGE_HEIGHT/ 2);
                frameIt->frame = 12 + direction/2; //going down hill
                if(world(xold1,yold1)->getGroup() == GROUP_ROAD_BRIDGE)
                {dy -= ROAD_BRIDGE_HEIGHT/ 2;}
            }
            break;
        case GROUP_TRACK:
            if( world(x,y)->getGroup() == GROUP_TRACK_BRIDGE )
            {
                dy -= (elapsed * TRACK_BRIDGE_HEIGHT);
                frameIt->frame = 8 + direction/2; //going up hill
            }
            else if(world(xold1,yold1)->getGroup() == GROUP_TRACK_BRIDGE)
            {
                dy -= (remaining * TRACK_BRIDGE_HEIGHT );
                frameIt->frame = 12 + direction/2;// going down hill
            }
            break;
    }

    frameIt->move_x = dx;
    frameIt->move_y = dy;

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

bool Vehicle::acceptable_heading(int idx)
{

    unsigned short g = world(idx)->getTransportGroup();

    if( !( (g == GROUP_TRACK) || (g == GROUP_ROAD) ) )
    {
        if(g != GROUP_RAIL)
        {   return false;}
        int x_trial = idx % world.len();
        int y_trial = idx / world.len();
        if(!world.is_visible(x_trial, y_trial))
        {   return false;}
        unsigned short g2 = world(xprev, yprev)->getTransportGroup();
        unsigned short g3 = world(2*x_trial-x, 2*y_trial-y)->getTransportGroup();
        if (g2 != g3)
        {   return false;}
    }

    //handle trivial case
    if(strategy == VEHICLE_STRATEGY_RANDOM)
    {   return true;}

    //dont go nowhere
    if(!world(idx)->reportingConstruction)
    {   return false;}
    //always leave from illegal area
    if(!world(x,y)->reportingConstruction)
    {   return true;}

    switch(strategy)
    {
        case VEHICLE_STRATEGY_MAXIMIZE:
            return world(x,y)->reportingConstruction->tellstuff(stuff_id, -2)*24/25 <
            world(idx)->reportingConstruction->tellstuff(stuff_id, -2) &&
            initial_cargo*99/100 < world(idx)->reportingConstruction->tellstuff(stuff_id, -2);
        case VEHICLE_STRATEGY_MINIMIZE:
            return world(x,y)->reportingConstruction->tellstuff(stuff_id, -2) >
            world(idx)->reportingConstruction->tellstuff(stuff_id, -2)*24/25
            && initial_cargo > world(idx)->reportingConstruction->tellstuff(stuff_id, -2)*99/100;
        default: //silence warning
        return true;
    }
    return false;
}


void Vehicle::getNewHeadings()
{
    headings = 0;

    int sum = 0;
    const int len = world.len();

    //never turn back the car
    if  (  (x >= xprev) && acceptable_heading( (x+1) + y*len ) )
    {   headings |= 1; ++sum;}
    if  (  (x <= xprev) && acceptable_heading( (x-1) + y*len ) )
    {   headings |= 2; ++sum;}
    if ( (y <= yprev) && acceptable_heading( x + (y-1)*len ) )
    {   headings |= 4; ++sum;}
    if ( (y >= yprev) && acceptable_heading( x + (y+1)*len ) )
    {   headings |= 8; ++sum;}

    //absolutely nowhere to go
    if (!sum)
    {   alive = false;  return;}

    //choose a random branch
    int k = 0;
    int choice = rand() % sum;
    int j = 0;
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

    //set the next destination
    switch (k)
    {
        case 0: xnext = x + 1; break;
        case 1: xnext = x - 1; break;
        case 2: ynext = y - 1; break;
        case 3: ynext = y + 1; break;
    }
}


void Vehicle::update()
{
    //get a new heading
    if(x == xnext && y == ynext)
    {   getNewHeadings();}
    // check if it is time to make a step
    if( real_time > anim) //move to dest
    {
        drive();
        if (frameIt->frame < 0)
        {   anim = real_time + 50;}
        else
        {   anim = real_time + speed;}
    }
    //animate the sprite
    walk();
    //cars have limited lifespan
    if(death_counter < 0)
    {   alive = false;}

}



















