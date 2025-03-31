/* ---------------------------------------------------------------------- *
 * src/lincity/ConstructionRequest.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2025      David Bears <dbear4q@gmail.com>
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

#include "all_buildings.h"        // for ORE_RESERVE
#include "groups.h"               // for GROUP_WATER
#include "lin-city.h"             // for FLAG_POWER_CABLES_0, FLAG_POWER_CAB...
#include "lintypes.h"             // for Construction, ConstructionGroup
#include "modules/all_modules.h"  // for FireConstructionGroup, Fire, Parkla...
#include "transport.h"            // for POWER_MODULUS
#include "world.h"                // for World, Map, MapTile

//#include "../lincity-ng/Mps.hpp"
//FIXME cannot include mps.h because of differing paths for further dependencies
//TODO eliminate duplicated code

extern int mps_x, mps_y;

void ConstructionDeletionRequest::execute()
{
    //std::cout << "deleting: " << subject->constructionGroup->name
    //<< " (" << subject->x << "," << subject->y << ")" << std::endl;
    World& world = subject->world;
    unsigned short size = subject->constructionGroup->size;
    int x = subject->x;
    int y = subject->y;
    subject->detach();
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            //update mps display
            world.map(x + j, y + i)->flags &= ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
            world.setUpdated(World::Updatable::MAP);
        }
    }
    // update adjacencies
    world.map.connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
    world.map.desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void OreMineDeletionRequest::execute()
{
    World& world = subject->world;
    int size = subject->constructionGroup->size;
    int x = subject->x;
    int y = subject->y;
    subject->detach();
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            world.map(x + j, y + i)->flags &=
              ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
            if (world.map(x+j,y+i)->ore_reserve < ORE_RESERVE / 2) {
                world.map(x+j,y+i)->setTerrain(GROUP_WATER);
                world.map(x+j,y+i)->flags |=
                  FLAG_HAS_UNDERGROUND_WATER;
                world.map.connect_rivers(x+j,y+i);
            }
            //update mps display
            world.setUpdated(World::Updatable::MAP);
        }
    }

    // update adjacencies
    world.map.connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
    world.map.desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void CommuneDeletionRequest::execute()
{
    World& world = subject->world;
    unsigned short size = subject->constructionGroup->size;
    int x = subject->x;
    int y = subject->y;
    subject->detach();
    for (unsigned short i = 0; i < size; ++i)
    {
        for (unsigned short j = 0; j < size; ++j)
        {
            world.map(x + j, y + i)->flags &=
              ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
            if(world.map(x+j,y+i)->flags & FLAG_HAS_UNDERGROUND_WATER)
              parklandConstructionGroup.placeItem(world, x+j, y+i);
            //update mps display
            world.setUpdated(World::Updatable::MAP);
        }
    }
    // update adjacencies
    world.map.connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
    world.map.desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void BurnDownRequest::execute()
{
    World& world = subject->world;
    unsigned short size = subject->constructionGroup->size;
    int x = subject->x;
    int y = subject->y;
    subject->detach();
    for (unsigned short i = 0; i < size; ++i)
    {
        for (unsigned short j = 0; j < size; ++j)
        {
            world.map(x + j, y + i)->flags &=
              ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
            fireConstructionGroup.placeItem(world, x+j, y+i);
            static_cast<Fire*>(world.map(x+j,y+i)->construction)
              ->burning_days = FIRE_LENGTH - 25;
            //update mps display
            world.setUpdated(World::Updatable::MAP);
        }
    }
    // update adjacencies
    world.map.connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
    world.map.desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void SetOnFire::execute()
{
    World& world = subject->world;
    unsigned short size = subject->constructionGroup->size;
    int x = subject->x;
    int y = subject->y;
    subject->detach();
    for (unsigned short i = 0; i < size; ++i)
    {
        for (unsigned short j = 0; j < size; ++j)
        {
            world.map(x + j, y + i)->flags &=
              ~(FLAG_POWER_CABLES_0 | FLAG_POWER_CABLES_90);
            fireConstructionGroup.placeItem(world, x+j, y+i);
            //update mps display
            world.setUpdated(World::Updatable::MAP);
        }
    }
    // update adjacencies
    world.map.connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
    world.map.desert_water_frontiers(x - 1, y - 1, size + 2, size + 2);
}

void PowerLineFlashRequest::execute()
{
    int *anim_counter = &(dynamic_cast<Powerline*>(subject)->anim_counter);
    // 2/3 cooldown will prevent interlacing wave packets
    if (*anim_counter <= POWER_MODULUS/3)
    {   *anim_counter = POWER_MODULUS;}
}
