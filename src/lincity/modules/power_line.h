/* ---------------------------------------------------------------------- *
 * src/lincity/modules/power_line.h
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

#include <array>                // for array

#include "lincity/transport.h"  // for MAX_HIVOLT_ON_POWERLINE
#include "modules.h"            // for Commodity, CommodityRule, Constructio...

#define POWER_LINE_FLASH_SPEED 100

class PowerlineConstructionGroup: public ConstructionGroup {
public:
    PowerlineConstructionGroup(
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
        commodityRuleCount[STUFF_HIVOLT].maxload = MAX_HIVOLT_ON_POWERLINE;
        commodityRuleCount[STUFF_HIVOLT].take = true;
        commodityRuleCount[STUFF_HIVOLT].give = true;
    }
    // overriding method that creates a power line
    virtual Construction *createConstruction();
};

extern PowerlineConstructionGroup powerlineConstructionGroup;

class Powerline: public Construction {
public:
    Powerline(ConstructionGroup *cstgrp) {
        this->constructionGroup = cstgrp;
        this->flags |= (FLAG_TRANSPARENT | FLAG_NEVER_EVACUATE);
        this->anim_counter = 0;
        this->anim = 0;
        this->flashing = false;
        initialize_commodities();
        this->trafficCount = this->commodityCount;

        commodityMaxCons[STUFF_HIVOLT] = 100 * 1;
    }
    virtual ~Powerline() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;
    void flow_power();
    std::array<int, STUFF_COUNT> trafficCount;
    int anim_counter;
    int anim;
    bool flashing;
};


/** @file lincity/modules/power_line.h */
