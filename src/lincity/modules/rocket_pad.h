/* ---------------------------------------------------------------------- *
 * src/lincity/modules/rocket_pad.h
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

#define GROUP_ROCKET_COLOUR   (magenta(20))
#define GROUP_ROCKET_COST     700000
#define GROUP_ROCKET_COST_MUL 2
#define GROUP_ROCKET_BUL_COST     1
#define GROUP_ROCKET_TECH     750
#define GROUP_ROCKET_FIREC 0
#define GROUP_ROCKET_RANGE 0
#define GROUP_ROCKET_SIZE 4

#define ROCKET_PAD_LABOR         200
#define MAX_LABOR_AT_ROCKET_PAD  (ROCKET_PAD_LABOR * 20)
#define ROCKET_PAD_GOODS        10000
#define MAX_GOODS_AT_ROCKET_PAD (ROCKET_PAD_GOODS * 20)
#define ROCKET_PAD_WASTE        (ROCKET_PAD_GOODS / 3)
#define MAX_WASTE_AT_ROCKET_PAD (ROCKET_PAD_WASTE * 20)
#define ROCKET_PAD_STEEL        240
#define MAX_STEEL_AT_ROCKET_PAD (ROCKET_PAD_STEEL * 20)
#define ROCKET_PAD_RUNNING_COST 200
#define ROCKET_PAD_STEPS        5000
#define ROCKET_ANIMATION_SPEED  800

#include <array>                    // for array
#include <string>                   // for basic_string

#include "modules.h"


class RocketPadConstructionGroup: public ConstructionGroup {
public:
  RocketPadConstructionGroup(
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
    commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_ROCKET_PAD;
    commodityRuleCount[STUFF_LABOR].take = true;
    commodityRuleCount[STUFF_LABOR].give = false;
    commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_AT_ROCKET_PAD;
    commodityRuleCount[STUFF_GOODS].take = true;
    commodityRuleCount[STUFF_GOODS].give = false;
    commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_AT_ROCKET_PAD;
    commodityRuleCount[STUFF_STEEL].take = true;
    commodityRuleCount[STUFF_STEEL].give = false;
    commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_AT_ROCKET_PAD;
    commodityRuleCount[STUFF_WASTE].take = false;
    commodityRuleCount[STUFF_WASTE].give = true;
  }
  // overriding method that creates a RocketPad
  virtual Construction *createConstruction() override;
};

extern RocketPadConstructionGroup rocketPadConstructionGroup;

class RocketPad: public Construction {
public:
  RocketPad(ConstructionGroup* cstgrp) {
    this->constructionGroup = cstgrp;
    init_resources();
    this->working_days = 0;
    this->busy = 0;
    this->anim = 0;
    this->steps = 0;
    this->stage = BUILDING;
    this->tech = tech_level;
    initialize_commodities();

    commodityMaxCons[STUFF_LABOR] = 100 * MAX_LABOR_AT_ROCKET_PAD;
    commodityMaxCons[STUFF_GOODS] = 100 * MAX_GOODS_AT_ROCKET_PAD;
    commodityMaxCons[STUFF_STEEL] = 100 * MAX_STEEL_AT_ROCKET_PAD;
    commodityMaxProd[STUFF_WASTE] = 100 * MAX_WASTE_AT_ROCKET_PAD;
  }

  virtual ~RocketPad() { }
  virtual void update() override;
  virtual void animate() override;
  virtual void report() override;

  virtual void save(xmlTextWriterPtr xmlWriter) override;
  virtual bool loadMember(xmlpp::TextReader& xmlReader) override;

  void launch_rocket();
  void compute_launch_result();
  void remove_people(int num);

  int working_days, busy;
  int tech;
  int anim;
  int steps;
  enum Stage {
    BUILDING,  // not completed
    AWAITING,  // waiting to be launched
    LAUNCHING, // launch animation playing
    LAUNCH,    // launch animation finished, pending actual launch
    DONE       // finished
  };
  enum Stage stage;
};


/** @file lincity/modules/rocket_pad.h */
