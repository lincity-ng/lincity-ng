/* ---------------------------------------------------------------------- *
 * src/lincity/modules/solar_power.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_SOLARPOWER_HPP__
#define __LINCITYNG_LINCITY_MODULES_SOLARPOWER_HPP__

#include <array>                      // for array
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for yellow
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction

class World;

#define GROUP_SOLAR_POWER_COLOUR (yellow(22))
#define GROUP_SOLAR_POWER_COST 500000
#define GROUP_SOLAR_POWER_COST_MUL 5
#define GROUP_SOLAR_POWER_BUL_COST 100000
#define GROUP_SOLAR_POWER_TECH (500 * (MAX_TECH_LEVEL / 1000))
#define GROUP_SOLAR_POWER_FIREC 33
#define GROUP_SOLAR_POWER_RANGE 0
#define GROUP_SOLAR_POWER_SIZE 4

#define SOLAR_POWER_LABOR 50
#define POWERS_SOLAR_OUTPUT 900 //1800
#define MAX_LABOR_AT_SOLARPS (20 * SOLAR_POWER_LABOR)
#define MAX_HIVOLT_AT_SOLARPS (20 * POWERS_SOLAR_OUTPUT)


class SolarPowerConstructionGroup: public ConstructionGroup {
public:
    SolarPowerConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_SOLARPS;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_HIVOLT].maxload = MAX_HIVOLT_AT_SOLARPS;
        commodityRuleCount[STUFF_HIVOLT].take = false;
        commodityRuleCount[STUFF_HIVOLT].give = true;
    };
    // overriding method that creates a Solar Power Plant
    virtual Construction *createConstruction(World& world);
};

extern SolarPowerConstructionGroup solarPowerConstructionGroup;

class SolarPower: public Construction {
public:
    SolarPower(World& world, ConstructionGroup *cstgrp);
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void place(MapPoint point) override;

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) override;

    int  hivolt_output;
    int  tech;
    int  working_days, busy;
};

#endif // __LINCITYNG_LINCITY_MODULES_SOLARPOWER_HPP__

/** @file lincity/modules/solar_power.h */
