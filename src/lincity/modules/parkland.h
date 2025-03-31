/* ---------------------------------------------------------------------- *
 * src/lincity/modules/parkland.h
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2025 David Bears <dbear4q@gmail.com>
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

#define GROUP_PARKLAND_COLOUR (green(31))
#define GROUP_PARKLAND_COST   1000
#define GROUP_PARKLAND_COST_MUL 25
#define GROUP_PARKLAND_BUL_COST   1000
#define GROUP_PARKLAND_TECH   2
#define GROUP_PARKLAND_FIREC 1
#define GROUP_PARKLAND_RANGE 0
#define GROUP_PARKLAND_SIZE 1

#include "modules.h"

class ParklandConstructionGroup: public ConstructionGroup {
public:
    ParklandConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range
    ) {

    };
    // overriding method that creates a Park
    virtual Construction *createConstruction(World& world);

    virtual bool can_build_here(const World& world, const MapPoint point,
      Message::ptr& message) const override;
};

extern ParklandConstructionGroup parklandConstructionGroup;
extern ParklandConstructionGroup parkpondConstructionGroup;

class Parkland: public Construction {
public:
    Parkland(World& world, ConstructionGroup *cstgrp);
    virtual void update();
    virtual void report(Mps& mps, bool production) const override;
};

/** @file lincity/modules/parkland.h */
