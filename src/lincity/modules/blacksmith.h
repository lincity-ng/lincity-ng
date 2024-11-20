/* ---------------------------------------------------------------------- *
 * src/lincity/modules/blacksmith.h
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

#ifndef __LINCITY_MODULES_BLACKSMITH_H__
#define __LINCITY_MODULES_BLACKSMITH_H__

#include <array>                    // for array

#include "modules.h"

#define GROUP_BLACKSMITH_NAME N_("Blacksmith")
#define GROUP_BLACKSMITH_COLOUR (white(15))
#define GROUP_BLACKSMITH_COST   5000
#define GROUP_BLACKSMITH_COST_MUL 25
#define GROUP_BLACKSMITH_BUL_COST   1000
#define GROUP_BLACKSMITH_TECH   3
#define GROUP_BLACKSMITH_FIREC 60
#define GROUP_BLACKSMITH_RANGE 0
#define GROUP_BLACKSMITH_SIZE 2
#define GROUP_BLACKSMITH_NEED_CREDIT false

#define BLACKSMITH_LABOR    35

#define BLACKSMITH_STEEL_USED 1
#define BLACKSMITH_COAL_USED  1
#define GOODS_MADE_BY_BLACKSMITH 50
#define MAX_LABOR_AT_BLACKSMITH (BLACKSMITH_LABOR*20)
#define MAX_COAL_AT_BLACKSMITH (BLACKSMITH_COAL_USED*20)
#define MAX_STEEL_AT_BLACKSMITH (BLACKSMITH_STEEL_USED*20)
#define MAX_GOODS_AT_BLACKSMITH (GOODS_MADE_BY_BLACKSMITH*20)
#define BLACKSMITH_CLOSE_TIME 25

#define BLACKSMITH_BATCH (GOODS_MADE_BY_BLACKSMITH*100)
//#define BLACKSMITH_ANIM_THRESHOLD 10
#define BLACKSMITH_ANIM_SPEED    200

class BlacksmithConstructionGroup: public ConstructionGroup {
public:
    BlacksmithConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    )
    {
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_BLACKSMITH;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_AT_BLACKSMITH;
        commodityRuleCount[STUFF_COAL].take = true;
        commodityRuleCount[STUFF_COAL].give = false;
        commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_AT_BLACKSMITH;
        commodityRuleCount[STUFF_STEEL].take = true;
        commodityRuleCount[STUFF_STEEL].give = false;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_AT_BLACKSMITH;
        commodityRuleCount[STUFF_GOODS].take = false;
        commodityRuleCount[STUFF_GOODS].give = true;
    }
    // overriding method that creates a blacksmith
    virtual Construction *createConstruction();
};

extern BlacksmithConstructionGroup blacksmithConstructionGroup;

class Blacksmith: public Construction {
public:
    Blacksmith(ConstructionGroup *cstgrp) {
        this->constructionGroup = cstgrp;
        this->anim = 0;
        this->pauseCounter = 0;
        this->busy = 0;
        this->working_days = 0;
        this->animate_enable = false;
        this->goods_made = 0;
        initialize_commodities();

        commodityMaxProd[STUFF_GOODS] = 100 * GOODS_MADE_BY_BLACKSMITH;
        commodityMaxCons[STUFF_COAL] = 100 * BLACKSMITH_COAL_USED;
        commodityMaxCons[STUFF_STEEL] = 100 * BLACKSMITH_STEEL_USED;
        commodityMaxCons[STUFF_LABOR] = 100 * BLACKSMITH_LABOR;
    }
    virtual ~Blacksmith() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;

    int  goods_made;
    int  anim;
    int  pauseCounter;
    int  working_days, busy;
    bool animate_enable;
};

#endif // __LINCITY_MODULES_BLACKSMITH_H__
/** @file lincity/modules/blacksmith.h */
