/* ---------------------------------------------------------------------- *
 * src/lincity/modules/residence.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_RESIDENCE_HPP__
#define __LINCITYNG_LINCITY_MODULES_RESIDENCE_HPP__

#include <array>                      // for array
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for cyan
#include "lincity/commodities.hpp"    // for CommodityRule, Commodity
#include "lincity/groups.hpp"         // for GROUP_RESIDENCE_HH, GROUP_RESID...
#include "lincity/lin-city.hpp"       // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"       // for ConstructionGroup, Construction

class World;

#define POWER_USE_PER_PERSON 3
#define POWER_RES_OVERHEAD 30
#define WORKING_POP_PERCENT 58
#define JOB_SWING 2
#define HC_WORKING_POP 8
#define HC_JOB_SWING 2
#define CRICKET_WORKING_POP 3
#define CRICKET_JOB_SWING 1
#define DAYS_PER_STARVE 20

/* RESIDENCE?_BRM is the birth rate modifier */
#define RESIDENCE_BASE_BR     100
#define RESIDENCE_BASE_DR     (68*12)
#define RESIDENCE_BRM_HEALTH  300
#define RESIDENCE_LL_BRM (RESIDENCE_BASE_BR*12)
#define RESIDENCE_ML_BRM 0
#define RESIDENCE_HL_BRM (RESIDENCE_BASE_BR+RESIDENCE_BASE_BR/4)
#define RESIDENCE_LH_BRM (RESIDENCE_BASE_BR*18)
#define RESIDENCE_MH_BRM (RESIDENCE_BASE_BR/2)
#define RESIDENCE_HH_BRM 0

/* RESIDENCE_PPM is the people_pool mobitily. Higher number=less mobile. */
#define RESIDENCE_PPM   20

#define GROUP_RESIDENCE_LL_COLOUR (cyan(24))
#define GROUP_RESIDENCE_LL_COST 1000
#define GROUP_RESIDENCE_LL_COST_MUL 25
#define GROUP_RESIDENCE_LL_BUL_COST 1000
#define GROUP_RESIDENCE_LL_TECH (0 * (MAX_TECH_LEVEL / 1000))
#define GROUP_RESIDENCE_LL_FIREC 75
#define GROUP_RESIDENCE_LL_MAX_POP 50

#define GROUP_RESIDENCE_ML_COLOUR (cyan(24))
#define GROUP_RESIDENCE_ML_COST 2000
#define GROUP_RESIDENCE_ML_COST_MUL 25
#define GROUP_RESIDENCE_ML_BUL_COST 1000
#define GROUP_RESIDENCE_ML_TECH (0 * (MAX_TECH_LEVEL / 1000))
#define GROUP_RESIDENCE_ML_FIREC 75
#define GROUP_RESIDENCE_ML_MAX_POP 100

#define GROUP_RESIDENCE_HL_COLOUR (cyan(24))
#define GROUP_RESIDENCE_HL_COST 4000
#define GROUP_RESIDENCE_HL_COST_MUL 25
#define GROUP_RESIDENCE_HL_BUL_COST 1000
#define GROUP_RESIDENCE_HL_TECH (0 * (MAX_TECH_LEVEL / 1000))
#define GROUP_RESIDENCE_HL_FIREC 75
#define GROUP_RESIDENCE_HL_MAX_POP 200

#define GROUP_RESIDENCE_LH_COLOUR (cyan(24))
#define GROUP_RESIDENCE_LH_COST 800
#define GROUP_RESIDENCE_LH_COST_MUL 25
#define GROUP_RESIDENCE_LH_BUL_COST 1000
#define GROUP_RESIDENCE_LH_TECH (300 * (MAX_TECH_LEVEL / 1000))
#define GROUP_RESIDENCE_LH_FIREC 75
#define GROUP_RESIDENCE_LH_MAX_POP 100

#define GROUP_RESIDENCE_MH_COLOUR (cyan(24))
#define GROUP_RESIDENCE_MH_COST 1600
#define GROUP_RESIDENCE_MH_COST_MUL 25
#define GROUP_RESIDENCE_MH_BUL_COST 1000
#define GROUP_RESIDENCE_MH_TECH (300 * (MAX_TECH_LEVEL / 1000))
#define GROUP_RESIDENCE_MH_FIREC 75
#define GROUP_RESIDENCE_MH_MAX_POP 200

#define GROUP_RESIDENCE_HH_COLOUR (cyan(24))
#define GROUP_RESIDENCE_HH_COST 3200
#define GROUP_RESIDENCE_HH_COST_MUL 25
#define GROUP_RESIDENCE_HH_BUL_COST 1000
#define GROUP_RESIDENCE_HH_TECH (300 * (MAX_TECH_LEVEL / 1000))
#define GROUP_RESIDENCE_HH_FIREC 75
#define GROUP_RESIDENCE_HH_MAX_POP 400

#define GROUP_RESIDENCE_RANGE 0
#define GROUP_RESIDENCE_SIZE 3


class ResidenceConstructionGroup: public ConstructionGroup {
public:
    ResidenceConstructionGroup(
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
        switch (group)
        {
            case GROUP_RESIDENCE_LL :
                commodityRuleCount[STUFF_LOVOLT].maxload =   20 * (POWER_RES_OVERHEAD + (GROUP_RESIDENCE_LL_MAX_POP * POWER_USE_PER_PERSON));
                commodityRuleCount[STUFF_WASTE].maxload =  6 * GROUP_RESIDENCE_LL_MAX_POP;
                commodityRuleCount[STUFF_GOODS].maxload = 10 * GROUP_RESIDENCE_LL_MAX_POP;
                commodityRuleCount[STUFF_LABOR].maxload =  20 * GROUP_RESIDENCE_LL_MAX_POP;
                commodityRuleCount[STUFF_FOOD].maxload =  20 * GROUP_RESIDENCE_LL_MAX_POP;
                commodityRuleCount[STUFF_WATER].maxload =  20 * GROUP_RESIDENCE_LL_MAX_POP;
            break;
            case GROUP_RESIDENCE_ML :
                commodityRuleCount[STUFF_LOVOLT].maxload = 20 * (POWER_RES_OVERHEAD + (GROUP_RESIDENCE_ML_MAX_POP * POWER_USE_PER_PERSON));
                commodityRuleCount[STUFF_WASTE].maxload =  6 * GROUP_RESIDENCE_ML_MAX_POP;
                commodityRuleCount[STUFF_GOODS].maxload = 10 * GROUP_RESIDENCE_ML_MAX_POP;
                commodityRuleCount[STUFF_LABOR].maxload =  20 * GROUP_RESIDENCE_ML_MAX_POP;
                commodityRuleCount[STUFF_FOOD].maxload =  20 * GROUP_RESIDENCE_ML_MAX_POP;
                commodityRuleCount[STUFF_WATER].maxload =  20 * GROUP_RESIDENCE_ML_MAX_POP;
            break;
            case GROUP_RESIDENCE_HL :
                commodityRuleCount[STUFF_LOVOLT].maxload =  20 * (POWER_RES_OVERHEAD + (GROUP_RESIDENCE_HL_MAX_POP * POWER_USE_PER_PERSON));
                commodityRuleCount[STUFF_WASTE].maxload =  6 * GROUP_RESIDENCE_HL_MAX_POP;
                commodityRuleCount[STUFF_GOODS].maxload = 10 * GROUP_RESIDENCE_HL_MAX_POP;
                commodityRuleCount[STUFF_LABOR].maxload =  20 * GROUP_RESIDENCE_HL_MAX_POP;
                commodityRuleCount[STUFF_FOOD].maxload =  20 * GROUP_RESIDENCE_HL_MAX_POP;
                commodityRuleCount[STUFF_WATER].maxload =  20 * GROUP_RESIDENCE_HL_MAX_POP;
            break;
            case GROUP_RESIDENCE_LH :
                commodityRuleCount[STUFF_LOVOLT].maxload =   20 * (POWER_RES_OVERHEAD + (GROUP_RESIDENCE_LH_MAX_POP * POWER_USE_PER_PERSON));
                commodityRuleCount[STUFF_WASTE].maxload =  6 * GROUP_RESIDENCE_LH_MAX_POP;
                commodityRuleCount[STUFF_GOODS].maxload = 10 * GROUP_RESIDENCE_LH_MAX_POP;
                commodityRuleCount[STUFF_LABOR].maxload =  20 * GROUP_RESIDENCE_LH_MAX_POP;
                commodityRuleCount[STUFF_FOOD].maxload =  20 * GROUP_RESIDENCE_LH_MAX_POP;
                commodityRuleCount[STUFF_WATER].maxload =  20 * GROUP_RESIDENCE_LH_MAX_POP;
            break;
            case GROUP_RESIDENCE_MH :
                commodityRuleCount[STUFF_LOVOLT].maxload =   20 * (POWER_RES_OVERHEAD + (GROUP_RESIDENCE_MH_MAX_POP * POWER_USE_PER_PERSON));
                commodityRuleCount[STUFF_WASTE].maxload =  6 * GROUP_RESIDENCE_MH_MAX_POP;
                commodityRuleCount[STUFF_GOODS].maxload = 10 * GROUP_RESIDENCE_MH_MAX_POP;
                commodityRuleCount[STUFF_LABOR].maxload =  20 * GROUP_RESIDENCE_MH_MAX_POP;
                commodityRuleCount[STUFF_FOOD].maxload =  20 * GROUP_RESIDENCE_MH_MAX_POP;
                commodityRuleCount[STUFF_WATER].maxload =  20 * GROUP_RESIDENCE_MH_MAX_POP;
            break;
            case GROUP_RESIDENCE_HH :
                commodityRuleCount[STUFF_LOVOLT].maxload =   20 * (POWER_RES_OVERHEAD + (GROUP_RESIDENCE_HH_MAX_POP * POWER_USE_PER_PERSON));
                commodityRuleCount[STUFF_WASTE].maxload =  6 * GROUP_RESIDENCE_HH_MAX_POP;
                commodityRuleCount[STUFF_GOODS].maxload = 10 * GROUP_RESIDENCE_HH_MAX_POP;
                commodityRuleCount[STUFF_LABOR].maxload =  20 * GROUP_RESIDENCE_HH_MAX_POP;
                commodityRuleCount[STUFF_FOOD].maxload =  20 * GROUP_RESIDENCE_HH_MAX_POP;
                commodityRuleCount[STUFF_WATER].maxload =  20 * GROUP_RESIDENCE_HH_MAX_POP;
            break;
        }
        commodityRuleCount[STUFF_FOOD].take = true;
        commodityRuleCount[STUFF_FOOD].give = false;
        commodityRuleCount[STUFF_LABOR].take = false;
        commodityRuleCount[STUFF_LABOR].give = true;
        commodityRuleCount[STUFF_GOODS].take = true;
        commodityRuleCount[STUFF_GOODS].give = false;
        commodityRuleCount[STUFF_WASTE].take = false;
        commodityRuleCount[STUFF_WASTE].give = true;
        commodityRuleCount[STUFF_LOVOLT].take = true;
        commodityRuleCount[STUFF_LOVOLT].give = false;
        commodityRuleCount[STUFF_WATER].take = true;
        commodityRuleCount[STUFF_WATER].give = false;
    }
    // overriding method that creates a residence
    virtual Construction *createConstruction(World& world);
};

extern ResidenceConstructionGroup residenceLLConstructionGroup, residenceMLConstructionGroup, residenceHLConstructionGroup;
extern ResidenceConstructionGroup residenceLHConstructionGroup, residenceMHConstructionGroup, residenceHHConstructionGroup;

class Residence: public Construction {
public:
    Residence(World& world, ConstructionGroup *cstgrp);
    virtual ~Residence();
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) override;

    virtual void torch() override;

    int local_population;
    int max_population;
    int desireability;
    int births, deaths, pol_deaths;
};

#endif // __LINCITYNG_LINCITY_MODULES_RESIDENCE_HPP__

/** @file lincity/modules/residence.h */
