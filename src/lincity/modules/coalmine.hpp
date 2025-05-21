/* ---------------------------------------------------------------------- *
 * src/lincity/modules/coalmine.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_COALMINE_HPP__
#define __LINCITYNG_LINCITY_MODULES_COALMINE_HPP__

#include <array>                      // for array
#include <string>                     // for string

#include "lincity/MapPoint.hpp"       // for MapPoint
#include "lincity/all_buildings.hpp"  // for LABOR_LOAD_COAL
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction

class World;

#define GROUP_COALMINE_COLOUR 0
#define GROUP_COALMINE_COST   10000
#define GROUP_COALMINE_COST_MUL 25
#define GROUP_COALMINE_BUL_COST   10000
#define GROUP_COALMINE_TECH   (85 * (MAX_TECH_LEVEL / 1000))
#define GROUP_COALMINE_FIREC  85
#define GROUP_COALMINE_RANGE  6
#define GROUP_COALMINE_SIZE  4


#define COALMINE_POLLUTION      3
#define COAL_PER_RESERVE   1000
#define LABOR_DIG_COAL 900
#define COALMINE_LABOR (LABOR_DIG_COAL + LABOR_LOAD_COAL)
#define MAX_LABOR_AT_COALMINE (20 * COALMINE_LABOR)
#define MAX_COAL_AT_MINE (20 * COAL_PER_RESERVE)


#define TARGET_COAL_LEVEL 80

class CoalmineConstructionGroup: public ConstructionGroup {
public:
    CoalmineConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_COALMINE;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_AT_MINE;
        commodityRuleCount[STUFF_COAL].take = true;
        commodityRuleCount[STUFF_COAL].give = true;
    }
    // overriding method that creates an Coalmine
    virtual Construction *createConstruction(World& world);
};

extern CoalmineConstructionGroup coalmineConstructionGroup;

class Coalmine: public Construction {
public:
    Coalmine(World& world, ConstructionGroup *cstgrp);
    virtual ~Coalmine() { }
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;
    virtual void place(MapPoint point) override;

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader,
      unsigned int ldsv_version) override;

    MapPoint mine_nw, mine_se;
    MapPoint mine_cur;
    int initial_coal_reserve, current_coal_reserve;
    int working_days, busy;
};

#endif // __LINCITYNG_LINCITY_MODULES_COALMINE_HPP__

/** @file lincity/modules/coalmine.h */
