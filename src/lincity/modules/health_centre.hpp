/* ---------------------------------------------------------------------- *
 * src/lincity/modules/health_centre.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_HEALTHCENTRE_HPP__
#define __LINCITYNG_LINCITY_MODULES_HEALTHCENTRE_HPP__

#include <array>                      // for array
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for green
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction

class World;

#define GROUP_HEALTH_COLOUR   (green(24))
#define GROUP_HEALTH_COST     100000
#define GROUP_HEALTH_COST_MUL 2
#define GROUP_HEALTH_BUL_COST     1000
#define GROUP_HEALTH_TECH     (110 * (MAX_TECH_LEVEL / 1000))
#define GROUP_HEALTH_FIREC 10
#define GROUP_HEALTH_RANGE 14
#define GROUP_HEALTH_SIZE 2

#define HEALTH_CENTRE_LABOR   6
#define MAX_LABOR_AT_HEALTH_CENTRE (20 * HEALTH_CENTRE_LABOR)
#define HEALTH_CENTRE_GOODS  40
#define MAX_GOODS_AT_HEALTH_CENTRE (20 * HEALTH_CENTRE_GOODS)
#define MAX_WASTE_AT_HEALTH_CENTRE (20 * HEALTH_CENTRE_GOODS / 3)
#define HEALTH_RUNNING_COST  2
#define HEALTH_RUNNING_COST_MUL 9

class HealthCentreConstructionGroup: public ConstructionGroup {
public:
    HealthCentreConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_HEALTH_CENTRE;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_AT_HEALTH_CENTRE;
        commodityRuleCount[STUFF_GOODS].take = true;
        commodityRuleCount[STUFF_GOODS].give = false;
        commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_AT_HEALTH_CENTRE;
        commodityRuleCount[STUFF_WASTE].take = false;
        commodityRuleCount[STUFF_WASTE].give = true;
    }
    // overriding method that creates a HealthCentre
    virtual Construction *createConstruction(World& world);
};

extern HealthCentreConstructionGroup healthCentreConstructionGroup;

class HealthCentre: public Construction {
public:
    HealthCentre(World& world, ConstructionGroup *cstgrp);
    virtual ~HealthCentre() { }
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    void cover();

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) override;

    int xs, ys, xe, ye;
    int daycount, covercount;
    bool active;
    int working_days, busy;
};

#endif // __LINCITYNG_LINCITY_MODULES_HEALTHCENTRE_HPP__

/** @file lincity/modules/health_centre.h */
