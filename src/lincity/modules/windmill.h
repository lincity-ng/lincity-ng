/* ---------------------------------------------------------------------- *
 * src/lincity/modules/windmill.h
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

#define GROUP_WINDMILL_COLOUR (green(25))
#define GROUP_WINDMILL_COST   20000
#define GROUP_WINDMILL_COST_MUL 25
#define GROUP_WINDMILL_BUL_COST   1000
#define GROUP_WINDMILL_TECH   30
#define GROUP_WINDMILL_FIREC  10
#define GROUP_WINDMILL_RANGE  0
#define GROUP_WINDMILL_SIZE  2

#define WINDMILL_LOVOLT     450
#define WINDMILL_LABOR       10
#define MAX_LABOR_AT_WINDMILL 20*(WINDMILL_LABOR)
#define MAX_LOVOLT_AT_WINDMILL 20*(WINDMILL_LOVOLT)
/* WINDMILL_RCOST is days per quid */
#define WINDMILL_RCOST      4
#define ANTIQUE_WINDMILL_ANIM_SPEED 120

#define MODERN_WINDMILL_TECH 450000

#include <array>                    // for array
#include <string>                   // for basic_string

#include "modules.h"

class WindmillConstructionGroup: public ConstructionGroup {
public:
    WindmillConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_WINDMILL;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_AT_WINDMILL;
        commodityRuleCount[STUFF_LOVOLT].take = false;
        commodityRuleCount[STUFF_LOVOLT].give = true;
    }
    // overriding method that creates a Windmill
    virtual Construction *createConstruction(int x, int y);
};

extern WindmillConstructionGroup windmillConstructionGroup;

class Windmill: public RegisteredConstruction<Windmill> { // Windmill inherits from its own RegisteredConstruction
public:
    Windmill(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Windmill>(x, y)
    {
        this->constructionGroup = cstgrp;
        init_resources();
        // this->anim = 0;
        this->animate_enable = false;
        this->tech = tech_level;
        setMemberSaved(&this->tech, "tech");
        this->working_days = 0;
        this->busy = 0;
        // this->lovolt_output = (int)(WINDMILL_LOVOLT + (((double)tech_level * WINDMILL_LOVOLT) / MAX_TECH_LEVEL));
        setMemberSaved(&this->lovolt_output, "kwh_output"); // compatibility
        initialize_commodities();

        commodityMaxCons[STUFF_LABOR] = 100 * WINDMILL_LABOR;
        // commodityMaxProd[STUFF_LOVOLT] = 100 * lovolt_output;
    }

    virtual void initialize() override {
      RegisteredConstruction::initialize();

      this->lovolt_output = (int)(WINDMILL_LOVOLT +
        (((double)tech * WINDMILL_LOVOLT) / MAX_TECH_LEVEL));

      commodityMaxProd[STUFF_LOVOLT] = 100 * lovolt_output;
    }

    virtual ~Windmill() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;

    int  lovolt_output;
    int  tech;
    int  anim;
    int  working_days, busy;
    bool animate_enable;
};

/** @file lincity/modules/windmill.h */
