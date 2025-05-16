/* ---------------------------------------------------------------------- *
 * src/lincity/modules/recycle.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_RECYCLE_HPP__
#define __LINCITYNG_LINCITY_MODULES_RECYCLE_HPP__

#include <array>                      // for array
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for LABOR_LOAD_ORE, LABOR_LOAD_STEEL
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction

class World;

#define GROUP_RECYCLE_COLOUR   (green(28))
#define GROUP_RECYCLE_COST    100000
#define GROUP_RECYCLE_COST_MUL 5
#define GROUP_RECYCLE_BUL_COST    1000
#define GROUP_RECYCLE_TECH    (232 * (MAX_TECH_LEVEL / 1000))
#define GROUP_RECYCLE_FIREC 10
#define GROUP_RECYCLE_RANGE 0
#define GROUP_RECYCLE_SIZE 2

#define WASTE_RECYCLED       500
#define RECYCLE_LABOR   (WASTE_RECYCLED/50 + LABOR_LOAD_ORE + LABOR_LOAD_STEEL)
#define RECYCLE_RUNNING_COST 3
#define LOVOLT_RECYCLE_WASTE (WASTE_RECYCLED/2)

#define MAX_LABOR_AT_RECYCLE (20 * RECYCLE_LABOR)
#define MAX_WASTE_AT_RECYCLE (20 * WASTE_RECYCLED)
#define MAX_ORE_AT_RECYCLE (16 * WASTE_RECYCLED)
#define MAX_LOVOLT_AT_RECYCLE   (20 * LOVOLT_RECYCLE_WASTE)
#define MAX_STEEL_AT_RECYCLE (16 * WASTE_RECYCLED/50)

#define BURN_WASTE_AT_RECYCLE (MAX_WASTE_AT_RECYCLE/200)


class RecycleConstructionGroup: public ConstructionGroup {
public:
    RecycleConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_RECYCLE;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_AT_RECYCLE;
        commodityRuleCount[STUFF_WASTE].take = true;
        commodityRuleCount[STUFF_WASTE].give = false;
        commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_AT_RECYCLE;
        commodityRuleCount[STUFF_LOVOLT].take = true;
        commodityRuleCount[STUFF_LOVOLT].give = false;
        commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_AT_RECYCLE;
        commodityRuleCount[STUFF_STEEL].take = false;
        commodityRuleCount[STUFF_STEEL].give = true;
        commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_AT_RECYCLE;
        commodityRuleCount[STUFF_ORE].take = false;
        commodityRuleCount[STUFF_ORE].give = true;
    }
    // overriding method that creates a recyle
    virtual Construction *createConstruction(World& world);
};

extern RecycleConstructionGroup recycleConstructionGroup;

class Recycle: public Construction {
public:
    Recycle(World& world, ConstructionGroup *cstgrp);

    virtual ~Recycle() { }
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void place(MapPoint point) override;

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) override;

    int  tech;
    int  make_ore;
    int  make_steel;
    int  working_days, busy;
};

#endif // __LINCITYNG_LINCITY_MODULES_RECYCLE_HPP__

/** @file lincity/modules/recycle.h */
