/* ---------------------------------------------------------------------- *
 * src/lincity/modules/pottery.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_POTTERY_HPP__
#define __LINCITYNG_LINCITY_MODULES_POTTERY_HPP__

#include <array>                      // for array
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for red
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction

class World;

#define GROUP_POTTERY_COLOUR (red(12))
#define GROUP_POTTERY_COST   1000
#define GROUP_POTTERY_COST_MUL 25
#define GROUP_POTTERY_BUL_COST 1000
#define GROUP_POTTERY_TECH  (0 * (MAX_TECH_LEVEL / 1000))
#define GROUP_POTTERY_FIREC 50
#define GROUP_POTTERY_RANGE 0
#define GROUP_POTTERY_SIZE 2

#define POTTERY_ORE_MAKE_GOODS    11
#define POTTERY_COAL_MAKE_GOODS    2
#define POTTERY_LABOR              35
#define POTTERY_MADE_GOODS        35
#define POTTERY_ANIM_SPEED        280
#define POTTERY_CLOSE_TIME        25
#define MAX_ORE_AT_POTTERY        (POTTERY_ORE_MAKE_GOODS*20)
#define MAX_COAL_AT_POTTERY       (POTTERY_COAL_MAKE_GOODS*20)
#define MAX_LABOR_AT_POTTERY       (POTTERY_LABOR*20)
#define MAX_GOODS_AT_POTTERY      (POTTERY_MADE_GOODS*20)

class PotteryConstructionGroup: public ConstructionGroup {
public:
    PotteryConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_POTTERY;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_AT_POTTERY;
        commodityRuleCount[STUFF_COAL].take = true;
        commodityRuleCount[STUFF_COAL].give = false;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_AT_POTTERY;
        commodityRuleCount[STUFF_GOODS].take = false;
        commodityRuleCount[STUFF_GOODS].give = true;
        commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_AT_POTTERY;
        commodityRuleCount[STUFF_ORE].take = true;
        commodityRuleCount[STUFF_ORE].give = false;
    }
    // overriding method that creates a pottery
    virtual Construction *createConstruction(World& world);
};

extern PotteryConstructionGroup potteryConstructionGroup;

class Pottery: public Construction {
public:
    Pottery(World& world, ConstructionGroup *cstgrp);
    virtual ~Pottery() { }
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;

    int  anim;
    int  pauseCounter;
    int  working_days, busy;
    bool animate_enable;
};

#endif // __LINCITYNG_LINCITY_MODULES_POTTERY_HPP__
