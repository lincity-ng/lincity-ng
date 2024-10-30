/* ---------------------------------------------------------------------- *
 * src/lincity/modules/rocket_pad.cpp
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

#include "rocket_pad.h"

#include <assert.h>                        // for assert
#include <stdlib.h>                        // for rand
#include <iostream>                        // for operator<<, basic_ostream
#include <list>                            // for _List_iterator
#include <stdexcept>                       // for runtime_error

#include "gui_interface/pbar_interface.h"  // for update_pbar, PPOP
#include "lincity-ng/Dialog.hpp"           // for Dialog, ASK_LAUNCH_ROCKET
#include "lincity-ng/Sound.hpp"            // for getSound, Sound
#include "lincity/ConstructionCount.h"     // for ConstructionCount
#include "modules.h"                       // for basic_string, char_traits
#include "residence.h"                     // for Residence

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

Construction *RocketPadConstructionGroup::createConstruction() {
  return new RocketPad(this);
}

extern void ok_dial_box(const char *, int, const char *);

void RocketPad::update() {
  if(stage != DONE)
    rocket_pad_cost += ROCKET_PAD_RUNNING_COST;

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
      new Dialog( ASK_LAUNCH_ROCKET, x, y );
    }
  }
  else if(stage == LAUNCH) {
    compute_launch_result();
    stage = DONE;
  }

  //monthly update
  if (total_time % 100 == 99) {
    reset_prod_counters();
    busy = working_days;
    working_days = 0;
  }
}

void RocketPad::animate() {
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
    rockets_launched++;
    /* The first five failures gives 49.419 % chances of 5 success
     * TODO: some stress could be added by 3,2,1,0 and animation of rocket with sound...
     */
    r = rand() % MAX_TECH_LEVEL;
    if (r > tech_level || r < tech || rand() % 100 > (rockets_launched * 15 + 25))
    {
        /* the launch failed */
        //display_rocket_result_dialog(ROCKET_LAUNCH_BAD);
        getSound()->playSound( "RocketExplosion" );
        ok_dial_box ("launch-fail.mes", BAD, 0L);
        rockets_launched_success = 0;
        xx = ((rand() % 40) - 20) + x;
        yy = ((rand() % 40) - 20) + y;
        for (i = 0; i < 20; i++)
        {
            xxx = ((rand() % 20) - 10) + xx;
            yyy = ((rand() % 20) - 10) + yy;
            if (xxx > 0 && xxx < (world.len() - 1)
                && yyy > 0 && yyy < (world.len() - 1))
            {
                /* don't crash on it's own area */
                if (xxx >= x && xxx < (x + constructionGroup->size) && yyy >= y && yyy < (y + constructionGroup->size))
                {   continue;}
                fire_area(xxx, yyy);
                /* make a sound perhaps */
            }
        }
    }
    else
    {
        getSound()->playSound( "RocketTakeoff" );
        rockets_launched_success++;
        /* TODO: Maybe should generate some pollution ? */
        if (rockets_launched_success > 5)
        {
            remove_people(1000);
            if (people_pool || housed_population)
            {
                //display_rocket_result_dialog(ROCKET_LAUNCH_EVAC);
                ok_dial_box ("launch-evac.mes", GOOD, 0L);
            }
        }
        else
        {
            //display_rocket_result_dialog(ROCKET_LAUNCH_GOOD);
            ok_dial_box ("launch-good.mes", GOOD, 0L);
        }
    }
}

void RocketPad::remove_people(int num)
{
    {
        int ppl = (num < people_pool)?num:people_pool;
        num -= ppl;
        people_pool -= ppl;
        total_evacuated += ppl;
    }
    /* reset housed population so that we can display it correctly */
    housed_population = 1;
    while (housed_population && (num > 0))
    {
        housed_population = 0;
        for (int i = 0; i < constructionCount.size(); i++)
        {
            if (constructionCount[i])
            {
                unsigned short grp = constructionCount[i]->constructionGroup->group;
                if( (grp == GROUP_RESIDENCE_LL)
                 || (grp == GROUP_RESIDENCE_ML)
                 || (grp == GROUP_RESIDENCE_HL)
                 || (grp == GROUP_RESIDENCE_LH)
                 || (grp == GROUP_RESIDENCE_MH)
                 || (grp == GROUP_RESIDENCE_HH) )
                 {
                    Residence* residence = static_cast <Residence *> (constructionCount[i]);
                    if (residence->local_population)
                    {
                        residence->local_population--;
                        housed_population += residence->local_population;
                        num--;
                        total_evacuated++;
                    }
                 }
            }
        }
    }
    update_pbar (PPOP, housed_population + people_pool, 0);
    if (!housed_population && !people_pool)
    {   ok_dial_box("launch-gone.mes", GOOD, 0L);}
}

void RocketPad::report()
{
    int i = 0;
    mps_store_title(i++, constructionGroup->name);
    mps_store_sfp(i++, N_("busy"), (busy));
    mps_store_sfp(i++, N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sfp(i++, N_("Completion"), (double)steps / ROCKET_PAD_STEPS);
    // i++;
    list_commodities(&i);
}

void RocketPad::save(xmlTextWriterPtr xmlWriter) {
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tech", "%d", tech);
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
}

bool RocketPad::loadMember(xmlpp::TextReader& xmlReader) {
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
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/rocket_pad.cpp */
