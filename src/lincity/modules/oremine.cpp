/* ---------------------------------------------------------------------- *
 * src/lincity/modules/oremine.cpp
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

#include "oremine.h"

#include <list>                           // for _List_iterator

#include "lincity/ConstructionManager.h"  // for ConstructionManager
#include "lincity/ConstructionRequest.h"  // for OreMineDeletionRequest
#include "modules.h"                      // for Commodity, ConstructionGroup

// Oremine:
OremineConstructionGroup oremineConstructionGroup(
     N_("Ore Mine"),
     FALSE,                     /* need credit? */
     GROUP_OREMINE,
     GROUP_OREMINE_SIZE,
     GROUP_OREMINE_COLOUR,
     GROUP_OREMINE_COST_MUL,
     GROUP_OREMINE_BUL_COST,
     GROUP_OREMINE_FIREC,
     GROUP_OREMINE_COST,
     GROUP_OREMINE_TECH,
     GROUP_OREMINE_RANGE
);

Construction *OremineConstructionGroup::createConstruction()
{
  return new Oremine(this);
}

void Oremine::update()
{
    int xx,yy;
    animate_enable = false;

    // see if we can/need to extract some underground ore
    if ((total_ore_reserve)
    && (commodityCount[STUFF_ORE] <= ORE_LEVEL_TARGET * (MAX_ORE_AT_MINE - ORE_PER_RESERVE)/100)
    && (commodityCount[STUFF_LABOR] >= OREMINE_LABOR))
    {
        for (yy = y; (yy < y + constructionGroup->size); yy++)
        {
            for (xx = x; (xx < x + constructionGroup->size); xx++)
            {
                if (world(xx,yy)->ore_reserve > 0)
                {
                    world(xx,yy)->ore_reserve--;
                    world(xx,yy)->flags |= FLAG_ALTERED;
                    total_ore_reserve--;
                    produceStuff(STUFF_ORE, ORE_PER_RESERVE);
                    consumeStuff(STUFF_LABOR, OREMINE_LABOR);
                    //FIXME ore_tax should be handled upon delivery
                    //ore_made += ORE_PER_RESERVE;
                    if (total_ore_reserve < (constructionGroup->size * constructionGroup->size * ORE_RESERVE))
                    {   sust_dig_ore_coal_tip_flag = 0;}
                    animate_enable = true;
                    working_days++;
                    goto end_mining;
                }
            }
        }
    }
    // return the ore to ore_reserve if there is enough sustainable ore available
    else if ((commodityCount[STUFF_ORE] - ORE_PER_RESERVE > ORE_LEVEL_TARGET * (MAX_ORE_AT_MINE )/100)
    && (commodityCount[STUFF_LABOR] >= LABOR_DIG_ORE))
    {
        for (yy = y; (yy < y + constructionGroup->size); yy++)
        {
            for (xx = x; (xx < x + constructionGroup->size); xx++)
            {
                if (world(xx,yy)->ore_reserve < (3 * ORE_RESERVE/2))
                {
                    world(xx,yy)->ore_reserve++;
                    world(xx,yy)->flags |= FLAG_ALTERED;
                    total_ore_reserve++;
                    consumeStuff(STUFF_ORE, ORE_PER_RESERVE);
                    consumeStuff(STUFF_LABOR, OREMINE_LABOR);
                    animate_enable = true;
                    working_days++;
                    goto end_mining;
                }
            }
        }
    }
    end_mining:

    //Monthly update of activity
    if (total_time % 100 == 99) {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }

    //Evacuate Mine if no more deposits
    if ( total_ore_reserve == 0 )
    {   flags |= FLAG_EVACUATE;}

    //Abandon the Oremine if it is really empty
    if ((total_ore_reserve == 0)
      &&(commodityCount[STUFF_LABOR] == 0)
      &&(commodityCount[STUFF_ORE] == 0) )
    {   ConstructionManager::submitRequest(new OreMineDeletionRequest(this));}
}

void Oremine::animate() {
  int& frame = frameIt->frame;

  if(animate_enable && real_time >= anim) {
    //faster animation for more active mines
    anim = real_time + ANIM_THRESHOLD((14 - busy/11) * OREMINE_ANIMATION_SPEED);
    if(anim_count < 8)
      frame = anim_count;
    else if (anim_count < 12)
      frame = 14 - anim_count;
    else
      frame = 16 - anim_count;
    if(++anim_count == 16)
      anim_count = 0;
  }
}

void Oremine::report()
{
    int i = 0;
    mps_store_title(i, constructionGroup->name);
    mps_store_sfp(i++, N_("busy"), busy);
    mps_store_sddp(i++, N_("Deposits"), total_ore_reserve, (constructionGroup->size * constructionGroup->size * ORE_RESERVE));
    // i++;
    list_commodities(&i);
}

void Oremine::place(int x, int y) {
  Construction::place(x, y);

  int ore = 0;
  for(int yy = y; yy < y + constructionGroup->size; yy++)
  for(int xx = x; xx < x + constructionGroup->size; xx++)
    ore += world(xx,yy)->ore_reserve;
  if(ore < 1)
    ore = 1;
  this->total_ore_reserve = ore;
}

void Oremine::save(xmlTextWriterPtr xmlWriter) {
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_ore_reserve", "%d", total_ore_reserve);
  Construction::save(xmlWriter);
}

bool Oremine::loadMember(xmlpp::TextReader& xmlReader) {
  std::string tag = xmlReader.get_name();
  if(tag == "total_ore_reserve") total_ore_reserve = std::stoi(xmlReader.read_inner_xml());
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/oremine.cpp */
