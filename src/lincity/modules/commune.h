/* ---------------------------------------------------------------------- *
 * src/lincity/modules/commune.h
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

#define GROUP_COMMUNE_COLOUR (green(30))
#define GROUP_COMMUNE_COST  1
#define GROUP_COMMUNE_COST_MUL 2
#define GROUP_COMMUNE_BUL_COST  1000
#define GROUP_COMMUNE_TECH  0
#define GROUP_COMMUNE_FIREC 30
#define GROUP_COMMUNE_RANGE 0
#define GROUP_COMMUNE_SIZE 4

//#define LABOR_AT_COMMUNE_GATE 4
#define COMMUNE_ANIM_SPEED 750
#define COMMUNE_POP  5 //Used at shanty

#define COMMUNE_COAL_MADE 3
#define MAX_COAL_AT_COMMUNE (20*COMMUNE_COAL_MADE)
#define COMMUNE_ORE_MADE 6
#define COMMUNE_ORE_FROM_WASTE 1
#define MAX_ORE_AT_COMMUNE (20*(COMMUNE_ORE_MADE + COMMUNE_ORE_FROM_WASTE))
#define COMMUNE_STEEL_MADE 2
#define MAX_STEEL_AT_COMMUNE (20*COMMUNE_STEEL_MADE)
#define COMMUNE_WASTE_GET 20
#define MAX_WASTE_AT_COMMUNE (20*COMMUNE_WASTE_GET)
#define WATER_FOREST 5
#define COMMUNE_WATER_GET (16 * WATER_FOREST)
#define MAX_WATER_AT_COMMUNE (20*COMMUNE_WATER_GET)

#include <array>                    // for array

#include "modules.h"

class CommuneConstructionGroup: public ConstructionGroup {
public:
    CommuneConstructionGroup(
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
        commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_AT_COMMUNE;
        commodityRuleCount[STUFF_COAL].take = false;
        commodityRuleCount[STUFF_COAL].give = true;
        commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_AT_COMMUNE;
        commodityRuleCount[STUFF_ORE].take = false;
        commodityRuleCount[STUFF_ORE].give = true;
        commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_AT_COMMUNE;
        commodityRuleCount[STUFF_STEEL].take = false;
        commodityRuleCount[STUFF_STEEL].give = true;
        commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_AT_COMMUNE;
        commodityRuleCount[STUFF_WASTE].take = true;
        commodityRuleCount[STUFF_WASTE].give = false;
        commodityRuleCount[STUFF_WATER].maxload = MAX_WATER_AT_COMMUNE;
        commodityRuleCount[STUFF_WATER].give = false;
        commodityRuleCount[STUFF_WATER].take = true;
    }
    // overriding method that creates a commune
    virtual Construction *createConstruction();
};

extern CommuneConstructionGroup communeConstructionGroup;

class Commune: public Construction {
public:
    Commune(ConstructionGroup *cstgrp) {
        this->constructionGroup = cstgrp;
        init_resources();
        this->anim = 0; // or real_time?
        this->animate_enable = false;
        this->steel_made = false;
        this->monthly_stuff_made = 0;
        this->last_month_output = 0;
        this->lazy_months = 0;
        initialize_commodities();
        // Check underground water, and reduce coal production accordingly
        int w = 0;
        for (int i = 0; i < constructionGroup->size; i++)
        {
            for (int j = 0; j < constructionGroup->size; j++)
            {
                if (world(x+j, y+i)->flags & FLAG_HAS_UNDERGROUND_WATER)
                {    w++;}
            }// end j
        }//end i
        this->ugwCount = w;
        if (w < 16 / 3)
        {   this->coalprod = COMMUNE_COAL_MADE/3;}
        else if (w < (2 * 16) / 3)
        {   this->coalprod = COMMUNE_COAL_MADE/2;}
        else
        {   this->coalprod = COMMUNE_COAL_MADE;}

        commodityMaxCons[STUFF_WATER] = 100 *
          constructionGroup->size * constructionGroup->size * WATER_FOREST;
        commodityMaxProd[STUFF_COAL] = 100 * COMMUNE_COAL_MADE;
        commodityMaxProd[STUFF_ORE] = 100 *
          (COMMUNE_ORE_MADE + COMMUNE_ORE_FROM_WASTE);
        commodityMaxCons[STUFF_WASTE] = 100 * COMMUNE_WASTE_GET;
        commodityMaxProd[STUFF_STEEL] = 100 / 20 * COMMUNE_STEEL_MADE;
    }
    virtual ~Commune() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;

    int  anim;
    int  ugwCount;
    int  coalprod;
    int  monthly_stuff_made;
    int  last_month_output;
    int  lazy_months;
    bool animate_enable;
    bool steel_made;
};

/** @file lincity/modules/commune.h */
