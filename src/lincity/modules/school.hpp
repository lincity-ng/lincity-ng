/* ---------------------------------------------------------------------- *
 * src/lincity/modules/school.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_SCHOOL_HPP__
#define __LINCITYNG_LINCITY_MODULES_SCHOOL_HPP__

#include <array>                      // for array
#include <list>                       // for list
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for white
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction

class World;
struct ExtraFrame;

#define GROUP_SCHOOL_COLOUR (white(15))
#define GROUP_SCHOOL_COST   10000
#define GROUP_SCHOOL_COST_MUL 25
#define GROUP_SCHOOL_BUL_COST   10000
#define GROUP_SCHOOL_TECH   (1 * (MAX_TECH_LEVEL / 1000))
#define GROUP_SCHOOL_FIREC 40
#define GROUP_SCHOOL_RANGE 0
#define GROUP_SCHOOL_SIZE 2

#define LABOR_MAKE_TECH_SCHOOL  200
#define GOODS_MAKE_TECH_SCHOOL  75
#define TECH_MADE_BY_SCHOOL    2
#define MAX_LABOR_AT_SCHOOL     (20 * LABOR_MAKE_TECH_SCHOOL)
#define MAX_GOODS_AT_SCHOOL    (20 * GOODS_MAKE_TECH_SCHOOL)
#define MAX_WASTE_AT_SCHOOL    (20 * GOODS_MAKE_TECH_SCHOOL / 3)
#define SCHOOL_RUNNING_COST    2

#define SCHOOL_ANIMATION_SPEED 80
#define SCHOOL_ANIMATION_BREAK 9500


class SchoolConstructionGroup: public ConstructionGroup {
public:
    SchoolConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_SCHOOL;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_AT_SCHOOL;
        commodityRuleCount[STUFF_GOODS].take = true;
        commodityRuleCount[STUFF_GOODS].give = false;
        commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_AT_SCHOOL;
        commodityRuleCount[STUFF_WASTE].take = false;
        commodityRuleCount[STUFF_WASTE].give = true;
    }
    // overriding method that creates a School
    virtual Construction *createConstruction(World& world);
};

extern SchoolConstructionGroup schoolConstructionGroup;

class School: public Construction {
public:
    School(World& world, ConstructionGroup *cstgrp);
    virtual ~School();
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;

    virtual void init_resources() override;

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) override;

    std::list<ExtraFrame>::iterator frit;
    int anim;
    int anim2;
    // bool animate_enable;
    int total_tech_made;
    int working_days, busy;
};

#endif // __LINCITYNG_LINCITY_MODULES_SCHOOL_HPP__

/** @file lincity/modules/school.h */
