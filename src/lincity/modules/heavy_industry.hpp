/* ---------------------------------------------------------------------- *
 * src/lincity/modules/heavy_industry.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_HEAVYINDUSTRY_HPP__
#define __LINCITYNG_LINCITY_MODULES_HEAVYINDUSTRY_HPP__

#include <array>                      // for array
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for LABOR_LOAD_COAL, LABOR_LOAD_ORE
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction

class World;

#define GROUP_INDUSTRY_H_COLOUR (7)     /* colour 7 is an orange, sort of */
#define GROUP_INDUSTRY_H_COST 50000
#define GROUP_INDUSTRY_H_COST_MUL 20
#define GROUP_INDUSTRY_H_BUL_COST 70000
#define GROUP_INDUSTRY_H_TECH (170 * (MAX_TECH_LEVEL / 1000))
#define GROUP_INDUSTRY_H_FIREC 80
#define GROUP_INDUSTRY_H_RANGE 0
#define GROUP_INDUSTRY_H_SIZE 4

#define LABOR_MAKE_STEEL 140
#define ORE_MAKE_STEEL 17
#define POWER_MAKE_STEEL 200
#define COAL_MAKE_STEEL 2
#define POL_PER_STEEL_MADE 0.25
#define MAX_ORE_USED 612
#define MAX_ORE_AT_INDUSTRY_H (20 * MAX_ORE_USED)
#define MAX_COAL_AT_INDUSTRY_H (2 * MAX_ORE_AT_INDUSTRY_H / ORE_MAKE_STEEL)
#define MAX_STEEL_AT_INDUSTRY_H (MAX_ORE_AT_INDUSTRY_H / ORE_MAKE_STEEL)
#define MAX_LABOR_AT_INDUSTRY_H (MAX_ORE_AT_INDUSTRY_H / LABOR_MAKE_STEEL + 20*(LABOR_LOAD_ORE + LABOR_LOAD_COAL + LABOR_LOAD_STEEL))

#define MAX_WASTE_AT_INDUSTRY_H (MAX_STEEL_AT_INDUSTRY_H * POL_PER_STEEL_MADE)
#define MAX_LOVOLT_AT_INDUSTY_H (MAX_STEEL_AT_INDUSTRY_H * POWER_MAKE_STEEL)
#define MAX_HIVOLT_AT_INDUSTY_H (MAX_STEEL_AT_INDUSTRY_H * POWER_MAKE_STEEL)

#define INDUSTRY_H_ANIM_SPEED  290
#define INDUSTRY_H_POLLUTION    10

#define MAX_MADE_AT_INDUSTRY_H 625

class IndustryHeavyConstructionGroup: public ConstructionGroup {
public:
    IndustryHeavyConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_INDUSTRY_H;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_AT_INDUSTRY_H;
        commodityRuleCount[STUFF_ORE].take = true;
        commodityRuleCount[STUFF_ORE].give = false;
        commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_AT_INDUSTRY_H;
        commodityRuleCount[STUFF_COAL].take = true;
        commodityRuleCount[STUFF_COAL].give = false;
        commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_AT_INDUSTRY_H;
        commodityRuleCount[STUFF_STEEL].take = false;
        commodityRuleCount[STUFF_STEEL].give = true;
        commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_AT_INDUSTRY_H;
        commodityRuleCount[STUFF_WASTE].take = false;
        commodityRuleCount[STUFF_WASTE].give = true;
        commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_AT_INDUSTY_H;
        commodityRuleCount[STUFF_LOVOLT].take = true;
        commodityRuleCount[STUFF_LOVOLT].give = false;
        commodityRuleCount[STUFF_HIVOLT].maxload = MAX_HIVOLT_AT_INDUSTY_H;
        commodityRuleCount[STUFF_HIVOLT].take = true;
        commodityRuleCount[STUFF_HIVOLT].give = false;
    };
    // overriding method that creates a HeavyIndustry
    virtual Construction *createConstruction(World& world);
};

extern IndustryHeavyConstructionGroup industryHeavyConstructionGroup;

class IndustryHeavy: public Construction {
public:
    IndustryHeavy(World& world, ConstructionGroup *cstgrp);

    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;
    virtual void place(MapPoint point) override;

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) override;

    int  tech;
    double bonus, extra_bonus;
    int  output_level;
    int  anim;
    int  steel_this_month;
};

#endif //__LINCITYNG_LINCITY_MODULES_HEAVYINDUSTRY_HPP__

/** @file lincity/modules/heavy_industry.h */
