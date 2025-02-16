/* ---------------------------------------------------------------------- *
 * src/lincity/modules/firestation.h
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2024 David Bears <dbear4q@gmail.com>
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

#define GROUP_FIRESTATION_COLOUR (red(20))
#define GROUP_FIRESTATION_COST   20000
#define GROUP_FIRESTATION_COST_MUL 10
#define GROUP_FIRESTATION_BUL_COST 1000
#define GROUP_FIRESTATION_TECH     22
#define GROUP_FIRESTATION_FIREC 0
#define GROUP_FIRESTATION_RANGE 17
#define GROUP_FIRESTATION_SIZE 2

#define FIRESTATION_LABOR   6
#define MAX_LABOR_AT_FIRESTATION (20 * FIRESTATION_LABOR)
#define FIRESTATION_GOODS  3
#define MAX_GOODS_AT_FIRESTATION (20 * FIRESTATION_GOODS)
#define MAX_WASTE_AT_FIRESTATION (20 * FIRESTATION_GOODS / 3)
#define FIRESTATION_RUNNING_COST 1
#define FIRESTATION_RUNNING_COST_MUL 6
#define FIRESTATION_ANIMATION_SPEED 250

#include <array>                    // for array

#include "modules.h"

class FireStationConstructionGroup: public ConstructionGroup {
public:
    FireStationConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    )
    {
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_FIRESTATION;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_AT_FIRESTATION;
        commodityRuleCount[STUFF_GOODS].take = true;
        commodityRuleCount[STUFF_GOODS].give = false;
        commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_AT_FIRESTATION;
        commodityRuleCount[STUFF_WASTE].take = false;
        commodityRuleCount[STUFF_WASTE].give = true;
    }
    // overriding method that creates a firestation
    virtual Construction *createConstruction(World& world);
};

extern FireStationConstructionGroup fireStationConstructionGroup;

class FireStation: public Construction {
public:
    FireStation(World& world, ConstructionGroup *cstgrp);
    virtual ~FireStation() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;
    void cover();

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader) override;

    int xs, ys, xe, ye;
    int daycount, covercount;
    int anim;
    bool animate_enable, active;
    int working_days, busy;
};

/** @file lincity/modules/firestation.h */
