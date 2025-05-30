/* ---------------------------------------------------------------------- *
 * src/lincity/modules/market.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_MARKET_HPP__
#define __LINCITYNG_LINCITY_MODULES_MARKET_HPP__

#include <array>                      // for array
#include <list>                       // for list
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for blue
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction
#include "lincity/transport.hpp"      // for MAX_COAL_IN_MARKET, MAX_FOOD_IN...

class World;
struct ExtraFrame;

#define GROUP_MARKET_COLOUR (blue(28))
#define GROUP_MARKET_COST 100
#define GROUP_MARKET_COST_MUL 25
#define GROUP_MARKET_BUL_COST 100
#define GROUP_MARKET_TECH (0 * (MAX_TECH_LEVEL / 1000))
#define GROUP_MARKET_FIREC 80
#define GROUP_MARKET_RANGE 9
#define GROUP_MARKET_SIZE 2

//#define MARKET_ANIM_SPEED 750

#define LABOR_MARKET_EMPTY   1
#define LABOR_MARKET_LOW     5
#define LABOR_MARKET_MED    12
#define LABOR_MARKET_FULL   28

class MarketConstructionGroup: public ConstructionGroup {
public:
    MarketConstructionGroup(
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
        commodityRuleCount[STUFF_FOOD].maxload = MAX_FOOD_IN_MARKET;
        commodityRuleCount[STUFF_FOOD].take = true;
        commodityRuleCount[STUFF_FOOD].give = true;
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_IN_MARKET;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = true;
        commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_IN_MARKET;
        commodityRuleCount[STUFF_COAL].take = true;
        commodityRuleCount[STUFF_COAL].give = true;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_IN_MARKET;
        commodityRuleCount[STUFF_GOODS].take = true;
        commodityRuleCount[STUFF_GOODS].give = true;
        commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_IN_MARKET;
        commodityRuleCount[STUFF_ORE].take = true;
        commodityRuleCount[STUFF_ORE].give = true;
        commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_IN_MARKET;
        commodityRuleCount[STUFF_STEEL].take = true;
        commodityRuleCount[STUFF_STEEL].give = true;
        commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_IN_MARKET;
        commodityRuleCount[STUFF_WASTE].take = true;
        commodityRuleCount[STUFF_WASTE].give = true;
        commodityRuleCount[STUFF_WATER].maxload = MAX_WATER_IN_MARKET;
        commodityRuleCount[STUFF_WATER].take = true;
        commodityRuleCount[STUFF_WATER].give = true;
    };
    // overriding method that creates a Market
    virtual Construction *createConstruction(World& world);
};

extern MarketConstructionGroup marketConstructionGroup;

class Market: public Construction {
public:
    Market(World& world, ConstructionGroup *cstgrp);
    virtual ~Market();

    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;
    virtual void init_resources() override;
    virtual void place(MapPoint point) override;
    void cover();
    void toggleEvacuation();

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) override;

    int xs, ys, xe, ye;
    int working_days, busy;
    int labor;
    std::array<CommodityRule, STUFF_COUNT> commodityRuleCount;
    int anim;
    int market_ratio;
    bool start_burning_waste;
    std::list<ExtraFrame>::iterator waste_fire_frit;
    int waste_fire_anim;
};

#endif // __LINCITYNG_LINCITY_MODULES_MARKET_HPP__

/** @file lincity/modules/market.h */
