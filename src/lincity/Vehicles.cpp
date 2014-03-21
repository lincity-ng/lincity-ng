/* ---------------------------------------------------------------------- *
 * Vehicles.cpp
 * This file is part of lincity-NG.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#include "Vehicles.h"
#include "lintypes.h"
#include <stdlib.h>
#include <vector>
#include "engine.h"

std::list<Vehicle*> Vehicle::vehicleList;

Vehicle::Vehicle(int x0, int y0, VehicleModel model0, VehicleStrategy vehicleStrategy)
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
    this->refresh_sprite = false;
    this->headings = 0;
    this->direction = 0;
    //this->xr = x;
    //this->yr = y;

    this->stuff_id = Construction::STUFF_JOBS;
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
            speed = BLUE_CAR_SPEED;
            break;
        default:
        std::cout << "default in new vehicle model = " << model << " at x= " << x << " y= " << y << std::endl;

    }
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

    int dx = xnext - xprev;
    int dy = ynext - yprev;
    if(dx == 2)
    {   direction = 2;}
    else if(dx == -2)
    {   direction = 6;}
    else if(dy == 2)
    {   direction = 0;}
    else if(dy == -2)
    {   direction = 4;}
    else if( (dx == 1) && (dy == 1) )
    {   direction = 1;}
    else if( (dx == 1) && (dy == -1) )
    {   direction = 3;}
    else if( (dx == -1) && (dy == 1) )
    {   direction = 7;}
    else if( (dx == -1) && (dy == -1) )
    {   direction = 5;}

    move_frame( x + y * world.len() );
    xprev = x;
    yprev = y;
    x = xnext;
    y = ynext;
    refresh_sprite = true; //signal to update sprite AFTER ajusting offsets
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

bool Vehicle::acceptable_heading(int k)
{
    //handle trivial case
    if(strategy == VEHICLE_STRATEGY_RANDOM)
    {   return true;}

    int xtrial = x;
    int ytrial = y;

    switch (k)
    {
        case 0: xtrial = x + 1; break;
        case 1: xtrial = x - 1; break;
        case 2: ytrial = y - 1; break;
        case 3: ytrial = y + 1; break;
    }

    //dont go nowhere
    if(!world(xtrial,ytrial)->reportingConstruction)
    {   return false;}
    //always leave from illegal area
    if(!world(x,y)->reportingConstruction)
    {   return true;}

    switch(strategy)
    {
        case VEHICLE_STRATEGY_MAXIMIZE:
        return world(x,y)->reportingConstruction->tellstuff(stuff_id, -2)*24/25 <
        world(xtrial,ytrial)->reportingConstruction->tellstuff(stuff_id, -2) &&
        initial_cargo*99/100 < world(xtrial,ytrial)->reportingConstruction->tellstuff(stuff_id, -2);
        case VEHICLE_STRATEGY_MINIMIZE:
        return world(x,y)->reportingConstruction->tellstuff(stuff_id, -2) >
        world(xtrial,ytrial)->reportingConstruction->tellstuff(stuff_id, -2)*24/25
        && initial_cargo > world(xtrial,ytrial)->reportingConstruction->tellstuff(stuff_id, -2)*99/100;
        default: //silence gcc warning
        return true;
    }
}


void Vehicle::getNewHeadings()
{
    headings = 0;
    unsigned short g;
    int sum = 0;

    g = world(x + 1, y)->getTransportGroup();
    if ( ( (g == GROUP_TRACK) || (g == GROUP_ROAD) ) && (x >= xprev))
    {   headings |= 1; ++sum;}
    g = world(x - 1, y)->getTransportGroup();
    if ( ( (g == GROUP_TRACK) || (g == GROUP_ROAD) ) && (x <= xprev))
    {   headings |= 2; ++sum;}
    g = world(x , y - 1)->getTransportGroup();
    if ( ( (g == GROUP_TRACK) || (g == GROUP_ROAD) ) && (y <= yprev))
    {   headings |= 4; ++sum;}
    g = world(x , y + 1)->getTransportGroup();
    if ( ( (g == GROUP_TRACK) || (g == GROUP_ROAD) ) && (y >= yprev))
    {   headings |= 8; ++sum;}

    //absolutely nowhere to go
    if (!sum)
    {   alive = false;  return;}

    //choose a random branch
    int k = -1;
    do
    {
        if(k != -1) //eliminate the previously failed possibility
        {
            headings &= ~(1 << k);
            --sum;
            if (!sum) //no more options
            {   alive = false;  return;}
        }
        k = 0;
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
    }
    while (sum && !acceptable_heading(k));

    // nowhere to go
    if (!sum)
    {   alive = false;  return;}

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
    bool do_drive = false;
    if( real_time > anim) //move to dest
    {
        if (frameIt->frame == -2)
        {   anim = real_time + 50;}
        else
        {   anim = real_time + speed;}
        //FIXME dirty workaround to avoid jumping sprites
        do_drive = true;
    }

    //move the sprite on the same tile
    int remaining = (anim-real_time)*1000/speed;
    int elapsed = 1000 - remaining;
    int mvx = 0;
    int mvy = 0;
    switch (direction) //pointing towards
    {
        case (0): //lower left
            mvx =  48 - 64 * elapsed / 1000;
            mvy = -40 + 32 * elapsed / 1000;
        break;
        case(4): //upper right
            mvx =       64 * elapsed / 1000;
            mvy =     - 32 * elapsed / 1000;
        break;
        case (2): //lower right
            mvx = -64 + 64 * elapsed / 1000;
            mvy = -32 + 32 * elapsed / 1000;
        break;
        case (6): //upper left
            mvx =  33 - 64 * elapsed / 1000;
            mvy =  -8 - 32 * elapsed / 1000;
        break;
        case (1): //vertically down
        {
            if(xprev != x) //turning left
            {
                mvx =   4 + 32 * remaining * remaining / 1000000;
                mvy =   4 - 32 * remaining / 1000;
            }
            else
            {
                mvx = -30 - 16 * remaining * remaining / 1000000;
                mvy =  -6 - 16 * remaining / 1000;
            }
        }
        break;
        case (5): //vertically up
        {
            if(xprev != x) //turning left
            {
                mvx = -32 + 16 * remaining /1000 + 24 * remaining * elapsed / 250000;
                mvy = -32 + 32 * remaining / 1000;
            }
            else
            {
                mvx =  60 - 16 + 16 * elapsed * elapsed / 1000000;
                mvy = -26 + 16 * remaining /1000;
            }
        }
        break;
        case (3): //horizontally right
        {
            if (xprev != x)
            {
                mvx = 0;
                mvy = 8;
                if( anim > (real_time + speed/4) )
                {   anim = real_time + speed/4;}
            }
            else
            {
                mvx =  50 - 92 * remaining / 1000;
                mvy = -20 + 12 - 12 * (remaining-500) * (remaining-500) / 250000;
            }
        }
        break;
        case (7): //horizontally left
        {
            if (xprev != x)
            {
                //std::cout << "turning right" << std::endl;
                mvx = -26 + 64 * remaining /1000;
                mvy = -32 + 8 - 8 * (remaining-500) * (remaining-500) / 250000;

            }
            else
            {
                mvx = -20 + 52 * remaining / 1000;
                mvy = -2 - 8 + 8 * (remaining-500) * (remaining-500) / 250000;;
            }
        }
        break;
    }

    //Check for bridges and scan for ramps
    bool adjust_uphill = false;
    bool adjust_downhill = false;
    switch(world(xprev,yprev)->getGroup())
    {
        case GROUP_TRACK_BRIDGE:
            mvy -= TRACK_BRIDGE_HEIGHT;
            break;
        case GROUP_ROAD_BRIDGE:
            mvy -= ROAD_BRIDGE_HEIGHT;
            break;
         case GROUP_ROAD:
            if( world(xnext,ynext)->getGroup() == GROUP_ROAD_BRIDGE
            ||  world(x,y)->getGroup() == GROUP_ROAD_BRIDGE)
            {
                mvy -= (elapsed * ROAD_BRIDGE_HEIGHT/ 2000);
                frameIt->frame = 8 + direction/2; //going up hill
                if(world(x,y)->getGroup() == GROUP_ROAD_BRIDGE)
                {mvy -= 24;}
                adjust_uphill = true;
            }
            else if(world(3*xprev - 2*x,3*yprev - 2*y)->getGroup() == GROUP_ROAD_BRIDGE
            ||      world(2*xprev - x,2*yprev - y)->getGroup() == GROUP_ROAD_BRIDGE)
            {
                mvy -= (remaining * ROAD_BRIDGE_HEIGHT/ 2000);
                frameIt->frame = 12 + direction/2; //going down hill
                if(world(2*xprev - x,2*yprev - y)->getGroup() == GROUP_ROAD_BRIDGE)
                {mvy -= 24;}
                adjust_downhill = true;
            }
            break;
        case GROUP_TRACK:
            if( world(x,y)->getGroup() == GROUP_TRACK_BRIDGE )
            {
                mvy -= (elapsed * TRACK_BRIDGE_HEIGHT / 1000);
                frameIt->frame = 8 + direction/2; //going up hill
                adjust_uphill = true;
            }
            else if(world(2*xprev - x,2*yprev - y)->getGroup() == GROUP_TRACK_BRIDGE)
            {
                mvy -= (remaining * TRACK_BRIDGE_HEIGHT / 1000);
                frameIt->frame = 12 + direction/2;// going down hill
                adjust_downhill = true;
            }
            break;
    }

    //Now aply offsets of ramps
    if(adjust_uphill)
    {
        switch (direction)
        {
            case 0: mvy +=  8;break;
            case 4: mvy -=  8;break;
            case 2: mvy +=  6;break;
        }
    }
    else if(adjust_downhill)
    {
        switch (direction)
        {
            case 0: mvy -= 12;break;
            case 4: mvy +=  8;break;
            case 2: mvy -=  2;break;
        }
    }

    //Apply the movement of the sprite
    frameIt->move_x = mvx;
    frameIt->move_y = mvy;

    //get a new sprite if moved previously
    if(refresh_sprite)
    {
        int s = (int)frameIt->resourceGroup->graphicsInfoVector.size();
        if(s)
        {   frameIt->frame = direction % s;}
        refresh_sprite = false;
    }
    // drive if need and sprite was already updated
    if(do_drive)
    {
        drive();
        frameIt->frame = -1; //hide the jumping frame
        //TODO resolve the jumping issue
        //maybe with a movement manager?
        //TODO manager should also sort the vehicles on a tile from back to front
    }

    //cars have limited lifespan
    if(death_counter < 0)
    {   alive = false;}

}



















