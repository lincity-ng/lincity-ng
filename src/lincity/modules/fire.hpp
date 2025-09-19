/* ---------------------------------------------------------------------- *
 * src/lincity/modules/fire.hpp
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

#ifndef __LINCITYNG_LINCITY_MODULES_FIRE_HPP__
#define __LINCITYNG_LINCITY_MODULES_FIRE_HPP__

#include <stdlib.h>                   // for rand
#include <string>                     // for string

#include "lincity/all_buildings.hpp"  // for red
#include "lincity/lintypes.hpp"       // for NUMOF_DAYS_IN_YEAR, Constructio...
#include "lincity/messages.hpp"       // for Message

class World;

#define GROUP_FIRE_COLOUR    (red(30))
#define GROUP_FIRE_COST   0     /* Unused */
#define GROUP_FIRE_COST_MUL   1 /* Unused */
#define GROUP_FIRE_BUL_COST  1000
#define GROUP_FIRE_TECH   0     /* Unused */
#define GROUP_FIRE_FIREC  0
#define GROUP_FIRE_RANGE  0
#define GROUP_FIRE_SIZE   1

#define DAYS_BETWEEN_FIRES (NUMOF_DAYS_IN_YEAR*2)
#define FIRE_ANIMATION_SPEED (200 + rand()%350 -175)
#define FIRE_DAYS_PER_SPREAD (NUMOF_DAYS_IN_YEAR/8)
#define FIRE_LENGTH (NUMOF_DAYS_IN_YEAR*5)
#define AFTER_FIRE_LENGTH (NUMOF_DAYS_IN_YEAR*10)

class FireConstructionGroup: public ConstructionGroup {
public:
    FireConstructionGroup(
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

    };
    // overriding method that creates a Fire
    virtual Construction *createConstruction(World& world);
};

extern FireConstructionGroup fireConstructionGroup;

class Fire: public Construction {
public:
    Fire(World& world, ConstructionGroup *cstgrp);
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;

    virtual void save(xmlTextWriterPtr xmlWriter) const override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) override;

    virtual bool can_bulldoze(Message::ptr& message) const override;
    virtual void torch() override;

    int burning_days;
    int smoking_days;
    int days_before_spread;
    int anim;

private:
  void spread();
};

#endif // __LINCITYNG_LINCITY_MODULES_FIRE_HPP__

/** @file lincity/modules/fire.h */
