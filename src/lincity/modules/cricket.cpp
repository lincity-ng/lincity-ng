/* ---------------------------------------------------------------------- *
 * src/lincity/modules/cricket.cpp
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

#include "cricket.h"

#include <list>                     // for _List_iterator
#include <vector>                   // for vector

#include "modules.h"

// cricket place:
CricketConstructionGroup cricketConstructionGroup(
    N_("Basketball court"),
     FALSE,                     /* need credit? */
     GROUP_CRICKET,
     GROUP_CRICKET_SIZE,
     GROUP_CRICKET_COLOUR,
     GROUP_CRICKET_COST_MUL,
     GROUP_CRICKET_BUL_COST,
     GROUP_CRICKET_FIREC,
     GROUP_CRICKET_COST,
     GROUP_CRICKET_TECH,
     GROUP_CRICKET_RANGE
);

Construction *CricketConstructionGroup::createConstruction() {
  return new Cricket(this);
}

void Cricket::update()
{
    ++daycount;
    if (commodityCount[STUFF_LABOR] >= CRICKET_LABOR
    &&  commodityCount[STUFF_GOODS] >= CRICKET_GOODS
    &&  commodityCount[STUFF_WASTE] + (CRICKET_GOODS / 3) <= MAX_WASTE_AT_CRICKET)
    {
        consumeStuff(STUFF_LABOR, CRICKET_LABOR);
        consumeStuff(STUFF_GOODS, CRICKET_GOODS);
        produceStuff(STUFF_WASTE, CRICKET_GOODS / 3);
        ++covercount;
        ++working_days;
    }
    //monthly update
    if (total_time % 100 == 99) {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }
    /* That's all. Cover is done by different functions every 3 months or so. */
    cricket_cost += CRICKET_RUNNING_COST;
    if(refresh_cover)
    {   cover();}
}

void Cricket::cover()
{
    if(covercount + COVER_TOLERANCE_DAYS < daycount)
    {
        daycount = 0;
        active = false;
        return;
    }
    active = true;
    covercount -= daycount;
    daycount = 0;
    animate_enable = true;
    for(int yy = ys; yy < ye; ++yy)
    {
        for(int xx = xs; xx < xe; ++xx)
        {   world(xx,yy)->flags |= FLAG_CRICKET_COVER;}
    }
}

void Cricket::animate() {
  int& frame = frameIt->frame;
  if(animate_enable && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(CRICKET_ANIMATION_SPEED);
    if(++frame >= (int)frameIt->resourceGroup->graphicsInfoVector.size())
    {
      frame = 0;
      animate_enable = false;
    }
  }
}

void Cricket::report()
{
    int i = 0;
    const char* p;

    mps_store_title(i, constructionGroup->name);
    mps_store_sfp(i++, N_("busy"), busy);
    // i++;
    list_commodities(&i);
    p = active?N_("Yes"):N_("No");
    mps_store_ss(i++, N_("Public sports"), p);
}

void Cricket::save(xmlTextWriterPtr xmlWriter) {
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"active",     "%d", active);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"daycount",   "%d", daycount);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"covercount", "%d", covercount);
}

bool Cricket::loadMember(xmlpp::TextReader& xmlReader) {
  std::string name = xmlReader.get_name();
  if(name == "active") active = std::stoi(xmlReader.read_inner_xml());
  else if(name == "daycount") daycount = std::stoi(xmlReader.read_inner_xml());
  else if(name == "covercount") covercount = std::stoi(xmlReader.read_inner_xml());
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/cricket.cpp */
