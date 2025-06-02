/* ---------------------------------------------------------------------- *
 * src/lincity/modules/mill.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_MILL_HPP__
#define __LINCITYNG_LINCITY_MODULES_MILL_HPP__

#include <array>                      // for array
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for white
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction

class World;

#define GROUP_MILL_COLOUR (white(15))
#define GROUP_MILL_COST   10000
#define GROUP_MILL_COST_MUL 25
#define GROUP_MILL_BUL_COST   1000
#define GROUP_MILL_TECH   (25 * (MAX_TECH_LEVEL / 1000))
#define GROUP_MILL_FIREC  60
#define GROUP_MILL_RANGE  0
#define GROUP_MILL_SIZE 2

#define MILL_LABOR          35
#define GOODS_MADE_BY_MILL 75
#define FOOD_USED_BY_MILL  (GOODS_MADE_BY_MILL/2)
#define MILL_POWER_PER_COAL 60
#define COAL_USED_BY_MILL  1

#define MAX_LOVOLT_AT_MILL    (COAL_USED_BY_MILL * MILL_POWER_PER_COAL * 20)
#define MAX_LABOR_AT_MILL   (MILL_LABOR * 20)
#define MAX_FOOD_AT_MILL   (FOOD_USED_BY_MILL * 20)
#define MAX_COAL_AT_MILL   (COAL_USED_BY_MILL * 20)
#define MAX_GOODS_AT_MILL  (GOODS_MADE_BY_MILL * 20)

#define MILL_ANIM_SPEED    300

class MillConstructionGroup: public ConstructionGroup {
public:
    MillConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_MILL;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_FOOD].maxload = MAX_FOOD_AT_MILL;
        commodityRuleCount[STUFF_FOOD].take = true;
        commodityRuleCount[STUFF_FOOD].give = false;
        commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_AT_MILL;
        commodityRuleCount[STUFF_COAL].take = true;
        commodityRuleCount[STUFF_COAL].give = false;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_AT_MILL;
        commodityRuleCount[STUFF_GOODS].take = false;
        commodityRuleCount[STUFF_GOODS].give = true;
        commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_AT_MILL;
        commodityRuleCount[STUFF_LOVOLT].take = true;
        commodityRuleCount[STUFF_LOVOLT].give = false;
    }
    // overriding method that creates a mill
    virtual Construction *createConstruction(World& world);
};

extern MillConstructionGroup millConstructionGroup;

class Mill: public Construction {
public:
    Mill(World& world, ConstructionGroup *cstgrp);
    virtual ~Mill() { }
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;

    int  anim;
    int  pol_count;
    int  working_days, busy;
    bool animate_enable;
};

#endif // __LINCITYNG_LINCITY_MODULES_MILL_HPP__

/** @file lincity/modules/mill.h */
