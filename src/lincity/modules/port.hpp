/* ---------------------------------------------------------------------- *
 * src/lincity/modules/port.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_PORT_HPP__
#define __LINCITYNG_LINCITY_MODULES_PORT_HPP__

#include <array>                      // for array
#include <map>                        // for map
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for blue
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction
#include "lincity/messages.hpp"       // for Message

class World;

#define GROUP_PORT_COLOUR (blue(28))
#define GROUP_PORT_COST 100000
#define GROUP_PORT_COST_MUL 2
#define GROUP_PORT_BUL_COST 1000
#define GROUP_PORT_TECH (35 * (MAX_TECH_LEVEL / 1000))
#define GROUP_PORT_FIREC 50
#define GROUP_PORT_RANGE 0
#define GROUP_PORT_SIZE 4
#define GROUP_PORT_NEED_CREDIT false

#define PORT_FOOD_RATE    1
#define PORT_LABOR_RATE    5
#define PORT_COAL_RATE    50
#define PORT_ORE_RATE     1
#define PORT_GOODS_RATE   3
#define PORT_STEEL_RATE   100
#define PORT_POLLUTION    1
//FIXME Guessing some values
#define PORT_LABOR  100
#define PORT_FOOD  500
#define PORT_COAL  100
#define PORT_GOODS 100
#define PORT_ORE   300
#define PORT_STEEL 50

#define MAX_LABOR_ON_PORT  (20 * PORT_LABOR)
#define MAX_FOOD_ON_PORT  (20 * PORT_FOOD)
#define MAX_COAL_ON_PORT  (20 * PORT_COAL)
#define MAX_GOODS_ON_PORT (20 * PORT_GOODS)
#define MAX_ORE_ON_PORT   (20 * PORT_ORE)
#define MAX_STEEL_ON_PORT (20 * PORT_STEEL)

/*
  These next two control the stuff bought or sold as a % of what's on the
  transport.  1000=100%  500=50% etc. Port will only work if % of stuff/available
 capacity excced capacity/PORT_TRIGGER_RATE
*/
#define PORT_EXPORT_RATE  500
#define PORT_IMPORT_RATE  500
#define PORT_TRIGGER_RATE  15

#define IMPORT_EXPORT_ENABLE_PERIOD  100
#define IMPORT_EXPORT_DISABLE_PERIOD 150

class PortConstructionGroup: public ConstructionGroup {
public:
    PortConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_ON_PORT;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_FOOD].maxload = MAX_FOOD_ON_PORT;
        commodityRuleCount[STUFF_FOOD].take = true;
        commodityRuleCount[STUFF_FOOD].give = true;
        commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_ON_PORT;
        commodityRuleCount[STUFF_COAL].take = true;
        commodityRuleCount[STUFF_COAL].give = true;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_ON_PORT;
        commodityRuleCount[STUFF_GOODS].take = true;
        commodityRuleCount[STUFF_GOODS].give = true;
        commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_ON_PORT;
        commodityRuleCount[STUFF_ORE].take = true;
        commodityRuleCount[STUFF_ORE].give = true;
        commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_ON_PORT;
        commodityRuleCount[STUFF_STEEL].take = true;
        commodityRuleCount[STUFF_STEEL].give = true;

        commodityRates[STUFF_FOOD] = PORT_FOOD_RATE;
        commodityRates[STUFF_COAL] = PORT_COAL_RATE;
        commodityRates[STUFF_GOODS] = PORT_GOODS_RATE;
        commodityRates[STUFF_ORE] = PORT_ORE_RATE;
        commodityRates[STUFF_STEEL] = PORT_STEEL_RATE;
    };
    //map that holds the Rates for the commodities
    std::map<Commodity, int> commodityRates;
    // overriding method that creates a Port
    virtual Construction *createConstruction(World& world) override;

    virtual bool can_build_here(const World& world, const MapPoint point,
      Message::ptr& message) const override;
};

extern PortConstructionGroup portConstructionGroup;

class Port: public Construction {
public:
    Port(World& world, ConstructionGroup *cstgrp);
    virtual ~Port() { }
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) override;

    int buy_stuff(Commodity stuff_ID);
    int sell_stuff(Commodity stuff_ID);
    void trade_connection();

    std::array<CommodityRule, STUFF_COUNT> commodityRuleCount;
    int daily_ic, monthly_ic, lastm_ic; //import cost
    int daily_et, monthly_et, lastm_et; //export tax
    int pence;
    int working_days, busy;
    int tech_made;
};

#endif // __LINCITYNG_LINCITY_MODULES_PORT_HPP__

/** @file lincity/modules/port.h */
