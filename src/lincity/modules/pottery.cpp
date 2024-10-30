/* ---------------------------------------------------------------------- *
 * src/lincity/modules/pottery.cpp
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

#include "pottery.h"

#include <list>                     // for _List_iterator
#include <vector>                   // for vector

#include "modules.h"

PotteryConstructionGroup potteryConstructionGroup(
    N_("Pottery"),
    FALSE,                     /* need credit? */
    GROUP_POTTERY,
    GROUP_POTTERY_SIZE,
    GROUP_POTTERY_COLOUR,
    GROUP_POTTERY_COST_MUL,
    GROUP_POTTERY_BUL_COST,
    GROUP_POTTERY_FIREC,
    GROUP_POTTERY_COST,
    GROUP_POTTERY_TECH,
    GROUP_POTTERY_RANGE
);

Construction *PotteryConstructionGroup::createConstruction() {
  return new Pottery(this);
}

void Pottery::update()
{
    if (total_time % 100 == 0)
    {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }

    if (pauseCounter++ < 0)
    {   return;}

    if ((commodityCount[STUFF_GOODS] + POTTERY_MADE_GOODS <= MAX_GOODS_AT_POTTERY)
     && (commodityCount[STUFF_ORE] >= POTTERY_ORE_MAKE_GOODS)
     && (commodityCount[STUFF_COAL] >= POTTERY_COAL_MAKE_GOODS)
     && (commodityCount[STUFF_LABOR] >= POTTERY_LABOR))
    {
        produceStuff(STUFF_GOODS, POTTERY_MADE_GOODS);
        consumeStuff(STUFF_ORE, POTTERY_ORE_MAKE_GOODS);
        consumeStuff(STUFF_COAL, POTTERY_COAL_MAKE_GOODS);
        consumeStuff(STUFF_LABOR, POTTERY_LABOR);

        animate_enable = true;
        if(!((working_days++)%10))
        {   world(x,y)->pollution++;}
    }
    else
    {
        animate_enable = false;
        pauseCounter = -POTTERY_CLOSE_TIME;
        return;
    }
}

void Pottery::animate() {
  int& frame = frameIt->frame;
  if (animate_enable && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(POTTERY_ANIM_SPEED);
    if(++frame >= (int)frameIt->resourceGroup->graphicsInfoVector.size()) {
      frame = 1;
      animate_enable = false;
    }
  }
  else if(!busy) {
    frame = 0;
  }
}

void Pottery::report()
{
    int i = 0;

    mps_store_title(i, constructionGroup->name);
    i++;
    mps_store_sfp(i++, N_("busy"), (float) busy);
    // i++;
    list_commodities(&i);
}

/** @file lincity/modules/pottery.cpp */
