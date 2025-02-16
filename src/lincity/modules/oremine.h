/* ---------------------------------------------------------------------- *
 * src/lincity/modules/oremine.h
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2024 David Bears <dbear4q@gmail.com>
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

#define GROUP_OREMINE_COLOUR (red(18))
#define GROUP_OREMINE_COST 500
#define GROUP_OREMINE_COST_MUL 10
#define GROUP_OREMINE_BUL_COST 500000
#define GROUP_OREMINE_TECH 0
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

#include <array>                    // for array

#include "modules.h"

class OremineConstructionGroup: public ConstructionGroup {
public:
    OremineConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance, int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    ) {
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_OREMINE;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_AT_MINE;
        commodityRuleCount[STUFF_ORE].take = true;
        commodityRuleCount[STUFF_ORE].give = true;

    }
    // overriding method that creates an Oremine
    virtual Construction *createConstruction(World& world);
};

extern OremineConstructionGroup oremineConstructionGroup;

class Oremine: public Construction {
public:
    Oremine(World& world, ConstructionGroup *cstgrp);
    virtual ~Oremine() {}
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;
    virtual void place(int x, int y) override;

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader) override;

    int total_ore_reserve;
    int anim;
    bool animate_enable;
    int working_days, busy;
    unsigned int anim_count;
    // int days_offset;
};

/** @file lincity/modules/oremine.h */
