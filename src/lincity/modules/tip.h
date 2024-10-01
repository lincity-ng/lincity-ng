/* ---------------------------------------------------------------------- *
 * src/lincity/modules/tip.h
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

#define GROUP_TIP_COLOUR (white(16))
#define GROUP_TIP_COST 10000
#define GROUP_TIP_COST_MUL 25
#define GROUP_TIP_BUL_COST 1000000
#define GROUP_TIP_TECH 0
#define GROUP_TIP_FIREC 50
#define GROUP_TIP_RANGE 0
#define GROUP_TIP_SIZE 4

#define TIP_DEGRADE_TIME (200 * NUMOF_DAYS_IN_YEAR)
#define MAX_WASTE_AT_TIP  10000000

#define WASTE_BURRIED 200
#define CRITICAL_WASTE_LEVEL 20 //gives waste if inbox is below and swallows if above
#define TIP_TAKES_WASTE (20 * WASTE_BURRIED)

#include <array>                    // for array
#include <string>                   // for basic_string

#include "modules.h"


class TipConstructionGroup: public ConstructionGroup {
public:
    TipConstructionGroup(
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
        commodityRuleCount[STUFF_WASTE].maxload = TIP_TAKES_WASTE;
        commodityRuleCount[STUFF_WASTE].take = true;
        commodityRuleCount[STUFF_WASTE].give = true;
    }
    // overriding method that creates a tip
    virtual Construction *createConstruction(int x, int y);
};

extern TipConstructionGroup tipConstructionGroup;

class Tip: public RegisteredConstruction<Tip>{ // Tip inherits from its own RegisteredConstruction
public:
    Tip(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Tip>(x, y)
    {
        this->constructionGroup = cstgrp;
        init_resources();
        this->flags |= FLAG_NEVER_EVACUATE;
        this->total_waste = 0;
        setMemberSaved(&this->total_waste,"total_waste");
        this->working_days = 0;
        this->busy = 0;
        this->degration_days = 0;
        setMemberSaved(&this->degration_days,"degration_days");
        initialize_commodities();

        commodityMaxCons[STUFF_WASTE] = 100 * WASTE_BURRIED;
        commodityMaxProd[STUFF_WASTE] = 100 * WASTE_BURRIED;
    }
    virtual ~Tip() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;

    int  working_days, busy;
    int  total_waste;
    int  degration_days;
};
