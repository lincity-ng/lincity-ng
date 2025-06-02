/* ---------------------------------------------------------------------- *
 * src/lincity/modules/oremine.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_OREMINE_HPP__
#define __LINCITYNG_LINCITY_MODULES_OREMINE_HPP__

#include <array>                      // for array
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for LABOR_LOAD_ORE, ORE_RESERVE, red
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction
#include "lincity/messages.hpp"       // for Message

class World;

#define GROUP_OREMINE_COLOUR (red(18))
#define GROUP_OREMINE_COST 500
#define GROUP_OREMINE_COST_MUL 10
#define GROUP_OREMINE_BUL_COST 500000
#define GROUP_OREMINE_TECH (0 * (MAX_TECH_LEVEL / 1000))
#define GROUP_OREMINE_FIREC 0
#define GROUP_OREMINE_RANGE 0
#define GROUP_OREMINE_SIZE 4

// Some ore related stuff has to be in all_buildings.h
#define ORE_PER_RESERVE   5000
#define MAX_ORE_AT_MINE (20 * ORE_PER_RESERVE)
#define MIN_ORE_RESERVE_FOR_MINE (ORE_RESERVE)
#define LABOR_DIG_ORE  200
#define OREMINE_LABOR (LABOR_DIG_ORE + LABOR_LOAD_ORE)
#define MAX_LABOR_AT_OREMINE (20 * OREMINE_LABOR)

#define ORE_LEVEL_TARGET 80 //mine will only supply so much
#define OREMINE_ANIMATION_SPEED 200

class OremineConstructionGroup: public ConstructionGroup {
public:
    OremineConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_OREMINE;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_AT_MINE;
        commodityRuleCount[STUFF_ORE].take = true;
        commodityRuleCount[STUFF_ORE].give = true;

    }
    // overriding method that creates an Oremine
    virtual Construction *createConstruction(World& world) override;

    virtual bool can_build_here(const World& world, const MapPoint point,
      Message::ptr& message) const override;
};

extern OremineConstructionGroup oremineConstructionGroup;

class Oremine: public Construction {
public:
    Oremine(World& world, ConstructionGroup *cstgrp);
    virtual ~Oremine() {}
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;
    virtual void place(MapPoint point) override;
    virtual void bulldoze() override;
    void makeLake();

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) override;

    int total_ore_reserve;
    int anim;
    bool animate_enable;
    int working_days, busy;
    unsigned int anim_count;
    // int days_offset;
};

#endif // __LINCITYNG_LINCITY_MODULES_OREMINE_HPP__

/** @file lincity/modules/oremine.h */
