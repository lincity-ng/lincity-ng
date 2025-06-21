/* ---------------------------------------------------------------------- *
 * src/lincity/modules/tip.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_TIP_HPP__
#define __LINCITYNG_LINCITY_MODULES_TIP_HPP__

#include <array>                      // for array
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for white
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for NUMOF_DAYS_IN_YEAR, Constructio...
#include "lincity/messages.hpp"       // for Message

class World;

#define GROUP_TIP_COLOUR (white(16))
#define GROUP_TIP_COST 10000
#define GROUP_TIP_COST_MUL 25
#define GROUP_TIP_BUL_COST 1000000
#define GROUP_TIP_TECH (0 * (MAX_TECH_LEVEL / 1000))
#define GROUP_TIP_FIREC 50
#define GROUP_TIP_RANGE 0
#define GROUP_TIP_SIZE 4

#define TIP_DEGRADE_TIME (200 * NUMOF_DAYS_IN_YEAR)
#define MAX_WASTE_AT_TIP  10000000

#define WASTE_BURRIED 200
#define CRITICAL_WASTE_LEVEL 20 //gives waste if inbox is below and swallows if above
#define TIP_TAKES_WASTE (20 * WASTE_BURRIED)


class TipConstructionGroup: public ConstructionGroup {
public:
    TipConstructionGroup(
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
        commodityRuleCount[STUFF_WASTE].maxload = TIP_TAKES_WASTE;
        commodityRuleCount[STUFF_WASTE].take = true;
        commodityRuleCount[STUFF_WASTE].give = true;
    }
    // overriding method that creates a tip
    virtual Construction *createConstruction(World& world);
};

extern TipConstructionGroup tipConstructionGroup;

class Tip: public Construction{
public:
    Tip(World& world, ConstructionGroup *cstgrp);
    virtual ~Tip() { }
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;

    virtual bool can_bulldoze(Message::ptr& messag) const override;

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) override;

    int  working_days, busy;
    int  total_waste;
    int  degration_days;
};

#endif // __LINCITYNG_LINCITY_MODULES_TIP_HPP__
