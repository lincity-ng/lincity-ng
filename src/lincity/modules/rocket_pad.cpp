/* ---------------------------------------------------------------------- *
 * src/lincity/modules/rocket_pad.cpp
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

#include "rocket_pad.hpp"

#include <assert.h>                       // for assert
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteFormatEle...
#include <stdlib.h>                       // for rand
#include <algorithm>                      // for min
#include <iostream>                       // for char_traits, basic_ostream
#include <list>                           // for _List_iterator
#include <set>                            // for _Rb_tree_const_iterator, set
#include <stdexcept>                      // for runtime_error
#include <string>                         // for basic_string, allocator

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"               // for GROUP_RESIDENCE_HH, GROUP_R...
#include "lincity/lin-city.hpp"             // for MAX_TECH_LEVEL, ANIM_THRESHOLD
#include "lincity/messages.hpp"           // for RocketResultMessage, OutOfM...
#include "lincity/resources.hpp"          // for ExtraFrame
#include "lincity/stats.hpp"                // for Stats
#include "lincity/world.hpp"                // for World, Map
#include "lincity/xmlloadsave.hpp"          // for xmlStr
#include "residence.hpp"                    // for Residence
#include "tinygettext/gettext.hpp"        // for N_

RocketPadConstructionGroup rocketPadConstructionGroup(
    N_("Rocket Pad"),
     TRUE,                     /* need credit? */
     GROUP_ROCKET,
     GROUP_ROCKET_SIZE,
     GROUP_ROCKET_COLOUR,
     GROUP_ROCKET_COST_MUL,
     GROUP_ROCKET_BUL_COST,
     GROUP_ROCKET_FIREC,
     GROUP_ROCKET_COST,
     GROUP_ROCKET_TECH,
     GROUP_ROCKET_RANGE
);

Construction *RocketPadConstructionGroup::createConstruction(World& world) {
  return new RocketPad(world, this);
}

extern void ok_dial_box(const char *, int, const char *);

RocketPad::RocketPad(World& world, ConstructionGroup* cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->working_days = 0;
  this->busy = 0;
  this->anim = 0;
  this->steps = 0;
  this->stage = BUILDING;
  this->tech = world.tech_level;
  initialize_commodities();

  commodityMaxCons[STUFF_LABOR] = 100 * MAX_LABOR_AT_ROCKET_PAD;
  commodityMaxCons[STUFF_GOODS] = 100 * MAX_GOODS_AT_ROCKET_PAD;
  commodityMaxCons[STUFF_STEEL] = 100 * MAX_STEEL_AT_ROCKET_PAD;
  commodityMaxProd[STUFF_WASTE] = 100 * MAX_WASTE_AT_ROCKET_PAD;
}

void RocketPad::update() {
  try{
    if(stage != DONE)
      world.expense(ROCKET_PAD_RUNNING_COST, world.stats.expenses.rockets);

    if(stage == BUILDING) {
      int stepsToday;
      int stepsRemaining = ROCKET_PAD_STEPS - steps;
      int stepsLabor = commodityCount[STUFF_LABOR] / ROCKET_PAD_LABOR;
      int stepsGoods = commodityCount[STUFF_GOODS] / ROCKET_PAD_GOODS;
      int stepsSteel = commodityCount[STUFF_STEEL] / ROCKET_PAD_STEEL;
      int stepsWaste = commodityCount[STUFF_WASTE] / ROCKET_PAD_GOODS;
      stepsToday = stepsRemaining;
      if(stepsLabor < stepsToday) stepsToday = stepsLabor;
      if(stepsGoods < stepsToday) stepsToday = stepsGoods;
      if(stepsSteel < stepsToday) stepsToday = stepsSteel;
      if(stepsWaste < stepsToday) stepsToday = stepsWaste;
      assert(stepsToday >= 0);

      consumeStuff(STUFF_LABOR, stepsToday * ROCKET_PAD_LABOR);
      consumeStuff(STUFF_GOODS, stepsToday * ROCKET_PAD_GOODS);
      consumeStuff(STUFF_STEEL, stepsToday * ROCKET_PAD_STEEL);
      consumeStuff(STUFF_WASTE, stepsToday * ROCKET_PAD_WASTE);
      steps += stepsToday;
      if(stepsToday)
        working_days++;

      if(steps >= ROCKET_PAD_STEPS) {
        stage = AWAITING;
        world.pushMessage(RocketReadyMessage::create(point));
      }
    }
    else if(stage == LAUNCH) {
      compute_launch_result();
      stage = DONE;
    }
  } catch(const OutOfMoneyMessage::Exception& ex) { }

  //monthly update
  if(world.total_time % 100 == 99) {
    reset_prod_counters();
    busy = working_days;
    working_days = 0;
  }
}

void RocketPad::animate(unsigned long real_time) {
  if(stage == BUILDING) {
    if(steps < (25 * ROCKET_PAD_STEPS) / 100)
      frameIt->frame = 0;
    else if(steps < (60 * ROCKET_PAD_STEPS) / 100)
      frameIt->frame = 1;
    else if(steps < (90 * ROCKET_PAD_STEPS) / 100)
      frameIt->frame = 2;
    else if(steps < (100 * ROCKET_PAD_STEPS) / 100)
      frameIt->frame = 3;
    else
      frameIt->frame = 4;
  }
  else if(stage == LAUNCHING) {
    if (real_time >= anim) {
      anim = real_time + ANIM_THRESHOLD(ROCKET_ANIMATION_SPEED);
      if(++frameIt->frame >= 6) {
        stage = LAUNCH;
      }
      assert(frameIt->frame <= 6);
    }
  }
  else if(stage == DONE) {
    frameIt->frame = 7;
  }
}

void RocketPad::launch_rocket() {
  if(stage != AWAITING) {
    std::cerr << "oopsie: It looks like you tried to launch a rocket that is"
      << " not awaiting launch.\n";
    return;
  }
  stage = LAUNCHING;
}

void RocketPad::compute_launch_result() {
    int i, r, xx, yy, xxx, yyy;
    world.rockets_launched++;
    /* The first five failures gives 49.419 % chances of 5 success
     * TODO: some stress could be added by 3,2,1,0 and animation of rocket with sound...
     */
    r = rand() % MAX_TECH_LEVEL;
    if(r > world.tech_level || r > tech
      || rand() % 100 > world.rockets_launched * 15 + 25
    ) {
        /* the launch failed */
        world.pushMessage(RocketResultMessage::create(
          point, RocketResultMessage::LaunchResult::FAIL));
        // TODO: getSound()->playSound( "RocketExplosion" );
        world.rockets_launched_success = 0;
        xx = ((rand() % 40) - 20) + x;
        yy = ((rand() % 40) - 20) + y;
        for (i = 0; i < 20; i++)
        {
            xxx = ((rand() % 20) - 10) + xx;
            yyy = ((rand() % 20) - 10) + yy;
            if (xxx > 0 && xxx < (world.map.len() - 1)
                && yyy > 0 && yyy < (world.map.len() - 1))
            {
                /* don't crash on it's own area */
                if (xxx >= x && xxx < (x + constructionGroup->size) && yyy >= y && yyy < (y + constructionGroup->size))
                {   continue;}
                world.fire_area(MapPoint(xxx, yyy));
                /* make a sound perhaps */
            }
        }
    }
    else
    {
        // TODO: getSound()->playSound( "RocketTakeoff" );
        world.rockets_launched_success++;
        /* TODO: Maybe should generate some pollution ? */
        if(world.rockets_launched_success > 5) {
          remove_people(1000);
        }
        else {
          world.pushMessage(RocketResultMessage::create(
            point, RocketResultMessage::LaunchResult::SUCCESS));
        }
    }
}

void RocketPad::remove_people(int num)
{
  {
    int ppl = std::min(num, world.people_pool);
    num -= ppl;
    world.people_pool -= ppl;
    world.stats.population.evacuated_t += ppl;
  }
  /* reset housed population so that we can display it correctly */
  while(num > 0) {
    int housed = 0;
    for(Construction *cst : world.map.constructions) {
      if(cst->isDead()) continue;
      unsigned short grp = cst->constructionGroup->group;
      if(  grp == GROUP_RESIDENCE_LL
        || grp == GROUP_RESIDENCE_ML
        || grp == GROUP_RESIDENCE_HL
        || grp == GROUP_RESIDENCE_LH
        || grp == GROUP_RESIDENCE_MH
        || grp == GROUP_RESIDENCE_HH
      ) {
        Residence* residence = static_cast<Residence *>(cst);
        if(residence->local_population) {
          residence->local_population--;
          housed += residence->local_population;
          world.stats.population.evacuated_t++;
          if(!--num) break;
        }
      }
    }

    if(!housed) {
      world.pushMessage(RocketResultMessage::create(
        point, RocketResultMessage::LaunchResult::EVAC_WIN));
      return;
    }
  }

  world.pushMessage(RocketResultMessage::create(
    point, RocketResultMessage::LaunchResult::EVAC));
}

void RocketPad::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.add_sfp(N_("busy"), (busy));
  mps.add_sfp(N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
  mps.add_sfp(N_("Completion"), (double)steps / ROCKET_PAD_STEPS);
  list_commodities(mps, production);
}

void RocketPad::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tech",  "%d", tech);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"steps", "%d", steps);
  const char *stStr;
  switch(stage) {
  case BUILDING: stStr = "building"; break;
  case AWAITING:
  case LAUNCHING:
  case LAUNCH:   stStr = "awaiting"; break;
  case DONE:     stStr = "done";     break;
  default:
    throw std::runtime_error("unknown rocket stage");
  }
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"stage", "%s", stStr);
  Construction::save(xmlWriter);
}

bool RocketPad::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string tag = xmlReader.get_name();
  if     (tag == "tech")  tech  = std::stoi(xmlReader.read_inner_xml());
  else if(tag == "steps") steps = std::stoi(xmlReader.read_inner_xml());
  else if(tag == "stage") {
    std::string stStr = xmlReader.read_inner_xml();
    if     (stStr == "building") stage = BUILDING;
    else if(stStr == "awaiting") stage = AWAITING;
    else if(stStr == "done")     stage = DONE;
    else throw std::runtime_error("unknown rocket stage");
  }
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/rocket_pad.cpp */
