/* ---------------------------------------------------------------------- *
 * src/lincity/modules/light_industry.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_LIGHTINDUSTRY_HPP__
#define __LINCITYNG_LINCITY_MODULES_LIGHTINDUSTRY_HPP__

#include <array>                      // for array
#include <list>                       // for list
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for cyan
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction

class World;
struct ExtraFrame;

#define GROUP_INDUSTRY_L_COLOUR (cyan(18))
#define GROUP_INDUSTRY_L_COST 20000
#define GROUP_INDUSTRY_L_COST_MUL 25
#define GROUP_INDUSTRY_L_BUL_COST 20000
#define GROUP_INDUSTRY_L_TECH (160 * (MAX_TECH_LEVEL / 1000))
#define GROUP_INDUSTRY_L_FIREC 70
#define GROUP_INDUSTRY_L_RANGE 0
#define GROUP_INDUSTRY_L_SIZE 3

#define INDUSTRY_L_ORE_USED  125
#define INDUSTRY_L_STEEL_USED  12
#define INDUSTRY_L_LABOR_USED   30
#define INDUSTRY_L_LABOR_LOAD_ORE 1
#define INDUSTRY_L_LABOR_LOAD_STEEL 4
#define MIN_LABOR_AT_INDUSTRY_L (INDUSTRY_L_LABOR_LOAD_ORE + INDUSTRY_L_LABOR_LOAD_STEEL + INDUSTRY_L_LABOR_USED)
#define INDUSTRY_L_MAKE_GOODS 225
#define INDUSTRY_L_POWER_PER_GOOD 10

#define MAX_ORE_AT_INDUSTRY_L (20 * INDUSTRY_L_ORE_USED)
#define MAX_LABOR_AT_INDUSTRY_L (20 * MIN_LABOR_AT_INDUSTRY_L)
#define MAX_GOODS_AT_INDUSTRY_L (20*2*4 * INDUSTRY_L_MAKE_GOODS)
#define MAX_WASTE_AT_INDUSTRY_L (MAX_GOODS_AT_INDUSTRY_L / 20)
#define MAX_LOVOLT_AT_INDUSTY_L (INDUSTRY_L_POWER_PER_GOOD*MAX_GOODS_AT_INDUSTRY_L)
#define MAX_HIVOLT_AT_INDUSTY_L (INDUSTRY_L_POWER_PER_GOOD*MAX_GOODS_AT_INDUSTRY_L / 2)
#define MAX_STEEL_AT_INDUSTRY_L (20 * INDUSTRY_L_STEEL_USED)

#define INDUSTRY_L_ANIM_SPEED 290
#define INDUSTRY_L_POL_PER_GOOD 0.05

class IndustryLightConstructionGroup: public ConstructionGroup {
public:
    IndustryLightConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_INDUSTRY_L;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_AT_INDUSTRY_L;
        commodityRuleCount[STUFF_GOODS].take = false;
        commodityRuleCount[STUFF_GOODS].give = true;
        commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_AT_INDUSTRY_L;
        commodityRuleCount[STUFF_ORE].take = true;
        commodityRuleCount[STUFF_ORE].give = false;
        commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_AT_INDUSTRY_L;
        commodityRuleCount[STUFF_STEEL].take = true;
        commodityRuleCount[STUFF_STEEL].give = false;
        commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_AT_INDUSTRY_L;
        commodityRuleCount[STUFF_WASTE].take = false;
        commodityRuleCount[STUFF_WASTE].give = true;
        commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_AT_INDUSTY_L;
        commodityRuleCount[STUFF_LOVOLT].take = true;
        commodityRuleCount[STUFF_LOVOLT].give = false;
        commodityRuleCount[STUFF_HIVOLT].maxload = MAX_HIVOLT_AT_INDUSTY_L;
        commodityRuleCount[STUFF_HIVOLT].take = true;
        commodityRuleCount[STUFF_HIVOLT].give = false;
    };
    // overriding method that creates a LightIndustry
    virtual Construction *createConstruction(World& world);
};

extern IndustryLightConstructionGroup industryLightConstructionGroup;


class IndustryLight: public Construction {
public:
    IndustryLight(World& world, ConstructionGroup *cstgrp);

    virtual ~IndustryLight();

    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;

    virtual void init_resources() override;
    virtual void detach() override;
    virtual void place(MapPoint point) override;

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) override;

    std::array<std::list<ExtraFrame>::iterator, 2> frits;
    int  tech;
    double bonus, extra_bonus;
    int  working_days;
    int  busy;
    int  anim;
    int  goods_this_month;
    int  goods_today;
};

#endif // __LINCITYNG_LINCITY_MODULES_LIGHTINDUSTRY_HPP__

/** @file lincity/modules/light_industry.h */
