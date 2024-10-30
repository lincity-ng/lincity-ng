/* ---------------------------------------------------------------------- *
 * src/lincity/modules/blacksmith.cpp
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

#include "blacksmith.h"

#include <list>                     // for _List_iterator
#include <string>                   // for basic_string
#include <vector>                   // for vector

#include "modules.h"

BlacksmithConstructionGroup blacksmithConstructionGroup(
  GROUP_BLACKSMITH_NAME,
  GROUP_BLACKSMITH_NEED_CREDIT,
  GROUP_BLACKSMITH,
  GROUP_BLACKSMITH_SIZE,
  GROUP_BLACKSMITH_COLOUR,
  GROUP_BLACKSMITH_COST_MUL,
  GROUP_BLACKSMITH_BUL_COST,
  GROUP_BLACKSMITH_FIREC,
  GROUP_BLACKSMITH_COST,
  GROUP_BLACKSMITH_TECH,
  GROUP_BLACKSMITH_RANGE
);

Construction *BlacksmithConstructionGroup::createConstruction() {
  return new Blacksmith(this);
}

void Blacksmith::update()
{
  if(pauseCounter++ < 0)
    ;
  if(commodityCount[STUFF_GOODS] + GOODS_MADE_BY_BLACKSMITH
      <= MAX_GOODS_AT_BLACKSMITH
    && commodityCount[STUFF_COAL] >= BLACKSMITH_COAL_USED
    && commodityCount[STUFF_STEEL] >= BLACKSMITH_STEEL_USED
    && commodityCount[STUFF_LABOR] >= BLACKSMITH_LABOR
  ) {
    produceStuff(STUFF_GOODS, GOODS_MADE_BY_BLACKSMITH);
    consumeStuff(STUFF_COAL, BLACKSMITH_COAL_USED);
    consumeStuff(STUFF_STEEL, BLACKSMITH_STEEL_USED);
    consumeStuff(STUFF_LABOR, BLACKSMITH_LABOR);
    working_days++;
    if ((goods_made += GOODS_MADE_BY_BLACKSMITH) >= BLACKSMITH_BATCH) {
      animate_enable = true;
      world(x,y)->pollution++;
      goods_made = 0;
    }
  }
  else {
    animate_enable = false;
    pauseCounter = -BLACKSMITH_CLOSE_TIME;
  }

  //monthly update
  if (total_time % 100 == 99) {
    reset_prod_counters();
    busy = working_days;
    working_days = 0;
  }
}

void Blacksmith::animate() {
  if(!animate_enable) {
    frameIt->frame = 0;
    anim = 0;
  }
  else if(real_time >= anim ) {
    anim = real_time + ANIM_THRESHOLD(BLACKSMITH_ANIM_SPEED);
    int s = frameIt->resourceGroup->graphicsInfoVector.size();
    if(++frameIt->frame >= s) {
      frameIt->frame = 1;
      animate_enable = false;
    }
  }
}

void Blacksmith::report()
{
  int i = 0;

  mps_store_title(i++, constructionGroup->name);
  i++;
  mps_store_sfp(i++, N_("busy"), (float) busy);
  // i++;
    list_commodities(&i);
}

/** @file lincity/modules/blacksmith.cpp */
