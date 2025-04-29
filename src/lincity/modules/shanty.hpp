/* ---------------------------------------------------------------------- *
 * src/lincity/modules/shanty.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_SHANTY_HPP__
#define __LINCITYNG_LINCITY_MODULES_SHANTY_HPP__

#include <array>                      // for array
#include <list>                       // for list
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for red
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction

class World;
struct ExtraFrame;

#define GROUP_SHANTY_COLOUR    (red(22))
#define GROUP_SHANTY_COST      0   /* Unused */
#define GROUP_SHANTY_COST_MUL  1   /* Unused */
#define GROUP_SHANTY_BUL_COST  100000
#define GROUP_SHANTY_TECH      0   /* Unused */
#define GROUP_SHANTY_FIREC     25
#define GROUP_SHANTY_RANGE     0
#define GROUP_SHANTY_SIZE      2


#define SHANTY_MIN_PP     150
#define SHANTY_POP        50
#define DAYS_BETWEEN_SHANTY (NUMOF_DAYS_IN_MONTH * 1)
#define SHANTY_WASTE_BURN_DAYS 10;

#define SHANTY_GET_FOOD   50
#define SHANTY_GET_LABOR   5
#define SHANTY_GET_GOODS  50
#define SHANTY_GET_COAL   1
#define SHANTY_GET_ORE    10
#define SHANTY_GET_STEEL  1
#define SHANTY_PUT_WASTE  1
#define SHANTY_GET_LOVOLT    50

#define MAX_FOOD_AT_SHANTY  (SHANTY_GET_FOOD * 20)
#define MAX_LABOR_AT_SHANTY  (SHANTY_GET_LABOR * 20)
#define MAX_GOODS_AT_SHANTY (SHANTY_GET_GOODS * 20)
#define MAX_COAL_AT_SHANTY  (SHANTY_GET_COAL * 20)
#define MAX_ORE_AT_SHANTY   (SHANTY_GET_ORE * 20)
#define MAX_STEEL_AT_SHANTY (SHANTY_GET_STEEL * 20)
#define MAX_WASTE_AT_SHANTY (SHANTY_PUT_WASTE * 20 + MAX_GOODS_AT_SHANTY / 3)
#define MAX_LOVOLT_AT_SHANTY   (SHANTY_GET_LOVOLT * 20)

class ShantyConstructionGroup: public ConstructionGroup {
public:
    ShantyConstructionGroup(
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
        commodityRuleCount[STUFF_FOOD].maxload = MAX_FOOD_AT_SHANTY;
        commodityRuleCount[STUFF_FOOD].take = true;
        commodityRuleCount[STUFF_FOOD].give = false;
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_SHANTY;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_AT_SHANTY;
        commodityRuleCount[STUFF_GOODS].take = true;
        commodityRuleCount[STUFF_GOODS].give = false;
        commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_AT_SHANTY;
        commodityRuleCount[STUFF_COAL].take = true;
        commodityRuleCount[STUFF_COAL].give = false;
        commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_AT_SHANTY;
        commodityRuleCount[STUFF_ORE].take = true;
        commodityRuleCount[STUFF_ORE].give = false;
        commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_AT_SHANTY;
        commodityRuleCount[STUFF_STEEL].take = true;
        commodityRuleCount[STUFF_STEEL].give = false;
        commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_AT_SHANTY;
        commodityRuleCount[STUFF_WASTE].take = false;
        commodityRuleCount[STUFF_WASTE].give = true;
        commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_AT_SHANTY;
        commodityRuleCount[STUFF_LOVOLT].take = true;
        commodityRuleCount[STUFF_LOVOLT].give = false;
    }
    // overriding method that creates a Shanty
    virtual Construction *createConstruction(World& world);
};

extern ShantyConstructionGroup shantyConstructionGroup;

class Shanty: public Construction {
public:
    Shanty(World& world, ConstructionGroup *cstgrp);
    virtual ~Shanty();

    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;

    virtual void init_resources() override;

    virtual void bulldoze() override;
    void makeFire();

    int anim;
    bool start_burning_waste;
    std::list<ExtraFrame>::iterator waste_fire_frit;
    int waste_fire_anim;
};

#endif // __LINCITYNG_LINCITY_MODULES_SHANTY_HPP__

/** @file lincity/modules/shanty.h */
