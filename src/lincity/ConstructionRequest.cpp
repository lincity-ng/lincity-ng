/* ---------------------------------------------------------------------- *
 * src/lincity/ConstructionRequest.cpp
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

#include "ConstructionRequest.h"

#include "../gui_interface/mps.h"  // for mps_set, MPS_MAP
#include "all_buildings.h"         // for ORE_RESERVE
#include "engglobs.h"              // for world
#include "engine.h"                // for desert_water_frontiers, connect_ri...
#include "groups.h"                // for GROUP_WATER
#include "lin-city.h"              // for FLAG_POWER_CABLES_0, FLAG_POWER_CA...
#include "lintypes.h"              // for Construction, MapTile, Constructio...
#include "modules/all_modules.h"   // for FireConstructionGroup, Fire, Parkl...
#include "transport.h"             // for connect_transport, POWER_MODULUS
#include "world.h"                 // for World

//#include "../lincity-ng/Mps.hpp"
//FIXME cannot include mps.h because of differing paths for further dependencies
//TODO eliminate duplicated code

extern int mps_x, mps_y;

void ConstructionDeletionRequest::execute()
{
    //std::cout << "deleting: " << subject->constructionGroup->name
    //<< " (" << subject->x << "," << subject->y << ")" << std::endl;
    unsigned short size = subject->constructionGroup->size;
    int x = subject->x;
    int y = subject->y;
    subject->detach();
    delete subject;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            //update mps display
            world(x + j, y + i)->flags &= ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
            if (mps_x == x + j && mps_y == y + i)
            {   mps_set(x + j, y + i, MPS_MAP);}
        }
    }
    // update adjacencies
    connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
    desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void OreMineDeletionRequest::execute()
{
    int size = subject->constructionGroup->size;
    int x = subject->x;
    int y = subject->y;
    subject->detach();
    delete subject;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            world(x + j, y + i)->flags &= ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
            if (world(x+j,y+i)->ore_reserve < ORE_RESERVE / 2)
            {
                world(x+j,y+i)->setTerrain(GROUP_WATER);
                world(x+j,y+i)->flags |= (FLAG_HAS_UNDERGROUND_WATER | FLAG_ALTERED);
                connect_rivers(x+j,y+i);
            }
            //update mps display
            if (mps_x == x + j && mps_y == y + i)
            {   mps_set(x + j, y + i, MPS_MAP);}
        }
    }

    // update adjacencies
    connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
    desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void CommuneDeletionRequest::execute()
{
    unsigned short size = subject->constructionGroup->size;
    int x = subject->x;
    int y = subject->y;
    subject->detach();
    delete subject;
    for (unsigned short i = 0; i < size; ++i)
    {
        for (unsigned short j = 0; j < size; ++j)
        {
            world(x + j, y + i)->flags &= ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
            if (world(x+j,y+i)->flags & FLAG_HAS_UNDERGROUND_WATER)
            {    parklandConstructionGroup.placeItem(x+j, y+i);}
            //update mps display
            if (mps_x == x + j && mps_y == y + i)
            {   mps_set(x + j, y + i, MPS_MAP);}
        }
    }
    // update adjacencies
    connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
    desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void BurnDownRequest::execute()
{
    unsigned short size = subject->constructionGroup->size;
    int x = subject->x;
    int y = subject->y;
    subject->detach();
    delete subject;
    for (unsigned short i = 0; i < size; ++i)
    {
        for (unsigned short j = 0; j < size; ++j)
        {
            world(x + j, y + i)->flags &= ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
            fireConstructionGroup.placeItem(x+j, y+i);
            static_cast<Fire*> (world(x+j,y+i)->construction)->burning_days = FIRE_LENGTH - 25;
            //update mps display
            if (mps_x == x + j && mps_y == y + i)
            {   mps_set(x + j, y + i, MPS_MAP);}
        }
    }
    // update adjacencies
    connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
    desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void SetOnFire::execute()
{
    unsigned short size = subject->constructionGroup->size;
    int x = subject->x;
    int y = subject->y;
    subject->detach();
    delete subject;
    for (unsigned short i = 0; i < size; ++i)
    {
        for (unsigned short j = 0; j < size; ++j)
        {
            world(x + j, y + i)->flags &= ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
            fireConstructionGroup.placeItem(x+j, y+i);
            //update mps display
            if (mps_x == x + j && mps_y == y + i)
            {   mps_set(x + j, y + i, MPS_MAP);}
        }
    }
    // update adjacencies
    connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
    desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void PowerLineFlashRequest::execute()
{
    int *anim_counter = &(dynamic_cast<Powerline*>(subject)->anim_counter);
    // 2/3 cooldown will prevent interlacing wave packets
    if (*anim_counter <= POWER_MODULUS/3)
    {   *anim_counter = POWER_MODULUS;}
}
