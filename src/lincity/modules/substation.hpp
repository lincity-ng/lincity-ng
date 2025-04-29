/* ---------------------------------------------------------------------- *
 * src/lincity/modules/substation.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_SUBSTATION_HPP__
#define __LINCITYNG_LINCITY_MODULES_SUBSTATION_HPP__

#include <array>                      // for array
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for yellow
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction

class World;

#define GROUP_SUBSTATION_COLOUR (yellow(18))
#define GROUP_SUBSTATION_COST 500
#define GROUP_SUBSTATION_COST_MUL 2
#define GROUP_SUBSTATION_BUL_COST 100
#define GROUP_SUBSTATION_TECH (200 * (MAX_TECH_LEVEL / 1000))
#define GROUP_SUBSTATION_FIREC 50
#define GROUP_SUBSTATION_RANGE 0
#define GROUP_SUBSTATION_SIZE 2

#define SUBSTATION_HIVOLT                          (1500)
#define MAX_HIVOLT_AT_SUBSTATION    (20 * SUBSTATION_HIVOLT)
#define SUBSTATION_LOVOLT            (2 * SUBSTATION_HIVOLT)
#define MAX_LOVOLT_AT_SUBSTATION    (20 * SUBSTATION_LOVOLT)


class SubstationConstructionGroup: public ConstructionGroup {
public:
    SubstationConstructionGroup(
        const std::string& name,
        const std::string& name_plural,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, name_plural, no_credit, group, size, colour, cost_mul, bul_cost,
        fire_chance,   cost, tech, range
    ) {
        commodityRuleCount[STUFF_HIVOLT].maxload = MAX_HIVOLT_AT_SUBSTATION;
        commodityRuleCount[STUFF_HIVOLT].take = true;
        commodityRuleCount[STUFF_HIVOLT].give = false;
        commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_AT_SUBSTATION;
        commodityRuleCount[STUFF_LOVOLT].take = false;
        commodityRuleCount[STUFF_LOVOLT].give = true;
    }
    // overriding method that creates a Substation
    virtual Construction *createConstruction(World& world);
};

extern SubstationConstructionGroup substationConstructionGroup;

class Substation: public Construction {
public:
    Substation(World& world, ConstructionGroup *cstgrp);
    virtual ~Substation() {}
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;

    int  working_days, busy;
};

#endif // __LINCITYNG_LINCITY_MODULES_SUBSTATION_HPP__

/** @file lincity/modules/substation.h */
