/* ---------------------------------------------------------------------- *
 * src/lincity/modules/power_line.cpp
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

#include "power_line.h"

#include <list>                     // for _List_iterator

#include "modules.h"

//Power line
PowerlineConstructionGroup powerlineConstructionGroup(
    N_("Power line"),
    FALSE,                     /* need credit? */
    GROUP_POWER_LINE,
    GROUP_POWER_LINE_SIZE,
    GROUP_POWER_LINE_COLOUR,
    GROUP_POWER_LINE_COST_MUL,
    GROUP_POWER_LINE_BUL_COST,
    GROUP_POWER_LINE_FIREC,
    GROUP_POWER_LINE_COST,
    GROUP_POWER_LINE_TECH,
    GROUP_POWER_LINE_RANGE
);

Construction *PowerlineConstructionGroup::createConstruction(World& world) {
  return new Powerline(world, this);
}

Powerline::Powerline(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->flags |= (FLAG_TRANSPARENT | FLAG_NEVER_EVACUATE);
  this->anim_counter = 0;
  this->anim = 0;
  this->flashing = false;
  initialize_commodities();
  this->trafficCount = this->commodityCount;

  commodityMaxCons[STUFF_HIVOLT] = 100 * 1;
}

void Powerline::update()
{
    if (commodityCount[STUFF_HIVOLT] > 0)
    {
        consumeStuff(STUFF_HIVOLT, 1);// loss on powerline
    }

    if(world.total_time % 100 == 99) {
      reset_prod_counters();
    }
}

void Powerline::animate() {
  switch(anim_counter) {
  case POWER_MODULUS - 2:
    if ( !(frameIt->frame >= 11) )
      break;
    flashing = false;
    frameIt->frame -= 11;
    break;
  case POWER_MODULUS:
    if ( !(frameIt->frame < 11) )
      break;
    flashing = true;
    frameIt->frame += 11;
    break;
  }
  if(anim_counter > 0 && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(POWER_LINE_FLASH_SPEED);
    --anim_counter;
  }
}

void Powerline::report()
{
    int i = 0;

    mps_store_title(i, constructionGroup->name);
    mps_store_sfp(i++, N_("usage"), trafficCount[STUFF_HIVOLT] * 107.77 * TRANSPORT_RATE / TRANSPORT_QUANTA);
    // i++;
    list_commodities(&i);
}


/** @file lincity/modules/power_line.cpp */
