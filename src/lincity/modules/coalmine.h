/* ---------------------------------------------------------------------- *
 * src/lincity/modules/coalmine.h
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

#define GROUP_COALMINE_COLOUR 0
#define GROUP_COALMINE_COST   10000
#define GROUP_COALMINE_COST_MUL 25
#define GROUP_COALMINE_BUL_COST   10000
#define GROUP_COALMINE_TECH   85
#define GROUP_COALMINE_FIREC  85
#define GROUP_COALMINE_RANGE  6
#define GROUP_COALMINE_SIZE  4


#define COALMINE_POLLUTION      3
#define COAL_PER_RESERVE   1000
#define LABOR_DIG_COAL 900
#define COALMINE_LABOR (LABOR_DIG_COAL + LABOR_LOAD_COAL)
#define MAX_LABOR_AT_COALMINE (20 * COALMINE_LABOR)
#define MAX_COAL_AT_MINE (20 * COAL_PER_RESERVE)


#define TARGET_COAL_LEVEL 80

#include <array>                    // for array
#include <string>                   // for basic_string

#include "modules.h"

class CoalmineConstructionGroup: public ConstructionGroup {
public:
    CoalmineConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    ) {
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_COALMINE;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_AT_MINE;
        commodityRuleCount[STUFF_COAL].take = true;
        commodityRuleCount[STUFF_COAL].give = true;
    }
    // overriding method that creates an Coalmine
    virtual Construction *createConstruction(int x, int y);
};

extern CoalmineConstructionGroup coalmineConstructionGroup;
//extern CoalmineConstructionGroup coalmine_L_ConstructionGroup;
//extern CoalmineConstructionGroup coalmine_M_ConstructionGroup;
//extern CoalmineConstructionGroup coalmine_H_ConstructionGroup;

class Coalmine: public RegisteredConstruction<Coalmine> { // Coalmine inherits from its RegisteredConstruction
public:
    Coalmine(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Coalmine>(x, y)
    {
        this->constructionGroup = cstgrp;
        init_resources();
        this->working_days = 0;
        this->busy = 0;
        this->current_coal_reserve = 0;  // has to be auto updated since coalmines may compete
        initialize_commodities();

        int coal = 0;
        int lenm1 = world.len()-1;
        int tmp;
        tmp = x - constructionGroup->range;
        this->xs = (tmp < 1) ? 1 : tmp;
        tmp = y - constructionGroup->range;
        this->ys = (tmp < 1)? 1 : tmp;
        tmp = x + constructionGroup->range + constructionGroup->size;
        this->xe = (tmp > lenm1) ? lenm1 : tmp;
        tmp = y + constructionGroup->range + constructionGroup->size;
        this->ye = (tmp > lenm1)? lenm1 : tmp;

        for (int yy = ys; yy < ye ; yy++)
        {
            for (int xx = xs; xx < xe ; xx++)
            {   coal += world(xx,yy)->coal_reserve;}
        }
        //always provide some coal so player can
        //store sustainable coal
        if (coal < 20)
        {
            world(x,y)->coal_reserve += 20-coal;
            coal = 20;
        }
        this->initial_coal_reserve = coal;
        setMemberSaved(&this->initial_coal_reserve,"initial_coal_reserve");
        this->current_coal_reserve = coal;

        commodityMaxProd[STUFF_COAL] = 100 * COAL_PER_RESERVE;
        commodityMaxCons[STUFF_COAL] = 100 * COAL_PER_RESERVE;
        commodityMaxCons[STUFF_LABOR] = 100 * COALMINE_LABOR;
    }
    virtual ~Coalmine() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;

    int xs, ys, xe, ye;
    int initial_coal_reserve;
    int current_coal_reserve;
    int working_days, busy;
};

/** @file lincity/modules/coalmine.h */
