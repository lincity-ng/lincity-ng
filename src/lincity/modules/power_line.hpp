/* ---------------------------------------------------------------------- *
 * src/lincity/modules/power_line.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_POWERLINE_HPP__
#define __LINCITYNG_LINCITY_MODULES_POWERLINE_HPP__

#include <array>                    // for array
#include <string>                   // for string

#include "lincity/commodities.hpp"  // for CommodityRule, Commodity
#include "lincity/lintypes.hpp"     // for ConstructionGroup, Construction
#include "lincity/transport.hpp"    // for MAX_HIVOLT_ON_POWERLINE

class World;

#define POWER_LINE_FLASH_SPEED 100

class PowerlineConstructionGroup: public ConstructionGroup {
public:
    PowerlineConstructionGroup(
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
        commodityRuleCount[STUFF_HIVOLT].maxload = MAX_HIVOLT_ON_POWERLINE;
        commodityRuleCount[STUFF_HIVOLT].take = true;
        commodityRuleCount[STUFF_HIVOLT].give = true;
    }
    // overriding method that creates a power line
    virtual Construction *createConstruction(World& world);
};

extern PowerlineConstructionGroup powerlineConstructionGroup;

class Powerline: public Construction {
public:
    Powerline(World& world, ConstructionGroup *cstgrp);
    virtual ~Powerline() {}
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;
    void flow_power();
    std::array<int, STUFF_COUNT> trafficCount;
    int anim_counter;
    int anim;
    bool flashing;
};

#endif // __LINCITYNG_LINCITY_MODULES_POWERLINE_HPP__

/** @file lincity/modules/power_line.h */
