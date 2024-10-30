/* ---------------------------------------------------------------------- *
 * src/lincity/modules/health_centre.h
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

#define GROUP_HEALTH_COLOUR   (green(24))
#define GROUP_HEALTH_COST     100000
#define GROUP_HEALTH_COST_MUL 2
#define GROUP_HEALTH_BUL_COST     1000
#define GROUP_HEALTH_TECH     110
#define GROUP_HEALTH_FIREC 2
#define GROUP_HEALTH_RANGE 14
#define GROUP_HEALTH_SIZE 2

#define HEALTH_CENTRE_LABOR   6
#define MAX_LABOR_AT_HEALTH_CENTRE (20 * HEALTH_CENTRE_LABOR)
#define HEALTH_CENTRE_GOODS  40
#define MAX_GOODS_AT_HEALTH_CENTRE (20 * HEALTH_CENTRE_GOODS)
#define MAX_WASTE_AT_HEALTH_CENTRE (20 * HEALTH_CENTRE_GOODS / 3)
#define HEALTH_RUNNING_COST  2
#define HEALTH_RUNNING_COST_MUL 9

#include <array>                    // for array
#include <string>                   // for basic_string

#include "modules.h"

class HealthCentreConstructionGroup: public ConstructionGroup {
public:
    HealthCentreConstructionGroup(
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
    virtual Construction *createConstruction();
};

extern HealthCentreConstructionGroup healthCentreConstructionGroup;

class HealthCentre: public Construction {
public:
    HealthCentre(ConstructionGroup *cstgrp) {
        this->constructionGroup = cstgrp;
        init_resources();
        this->active = false;
        this->busy = 0;
        this->daycount = 0;
        this->working_days = 0;
        this->covercount = 0;
        initialize_commodities();

        int tmp;
        int lenm1 = world.len()-1;
        tmp = x - constructionGroup->range;
        this->xs = (tmp < 1) ? 1 : tmp;
        tmp = y - constructionGroup->range;
        this->ys = (tmp < 1)? 1 : tmp;
        tmp = x + constructionGroup->range + constructionGroup->size;
        this->xe = (tmp > lenm1) ? lenm1 : tmp;
        tmp = y + constructionGroup->range + constructionGroup->size;
        this->ye = (tmp > lenm1)? lenm1 : tmp;

        commodityMaxCons[STUFF_LABOR] = 100 * HEALTH_CENTRE_LABOR;
        commodityMaxCons[STUFF_GOODS] = 100 * HEALTH_CENTRE_GOODS;
        commodityMaxProd[STUFF_WASTE] = 100 * (HEALTH_CENTRE_GOODS / 3);
    }
    virtual ~HealthCentre() { }
    virtual void update();
    virtual void report();
    void cover();

    virtual void save(xmlTextWriterPtr xmlWriter) override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader) override;

    int xs, ys, xe, ye;
    int daycount, covercount;
    bool active;
    int working_days, busy;
};

/** @file lincity/modules/health_centre.h */
