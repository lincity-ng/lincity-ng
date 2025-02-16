/* ---------------------------------------------------------------------- *
 * src/lincity/modules/coalmine.cpp
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

#include "coalmine.h"

#include <list>                           // for _List_iterator
#include <map>                            // for map

#include "lincity/ConstructionManager.h"  // for ConstructionManager
#include "lincity/ConstructionRequest.h"  // for ConstructionDeletionRequest
#include "modules.h"                      // for Commodity, ExtraFrame, MapTile

// Coalmine:
CoalmineConstructionGroup coalmineConstructionGroup(
    N_("Coal Mine"),
     FALSE,                     /* need credit? */
     GROUP_COALMINE,
     GROUP_COALMINE_SIZE,
     GROUP_COALMINE_COLOUR,
     GROUP_COALMINE_COST_MUL,
     GROUP_COALMINE_BUL_COST,
     GROUP_COALMINE_FIREC,
     GROUP_COALMINE_COST,
     GROUP_COALMINE_TECH,
     GROUP_COALMINE_RANGE
);

//CoalmineConstructionGroup coalmine_L_ConstructionGroup = coalmineConstructionGroup;
//CoalmineConstructionGroup coalmine_M_ConstructionGroup = coalmineConstructionGroup;
//CoalmineConstructionGroup coalmine_H_ConstructionGroup = coalmineConstructionGroup;

Construction *CoalmineConstructionGroup::createConstruction(World& world) {
  return new Coalmine(world, this);
}

Coalmine::Coalmine(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->working_days = 0;
  this->busy = 0;
  this->current_coal_reserve = 0;  // has to be auto updated since coalmines may compete
  initialize_commodities();

  commodityMaxProd[STUFF_COAL] = 100 * COAL_PER_RESERVE;
  commodityMaxCons[STUFF_COAL] = 100 * COAL_PER_RESERVE;
  commodityMaxCons[STUFF_LABOR] = 100 * COALMINE_LABOR;
}

void Coalmine::update()
{
    bool coal_found = false;
    //scan available coal_reserve in range
    current_coal_reserve = 0;
    for (int yy = ys; yy < ye ; yy++)
    {
        for (int xx = xs; xx < xe ; xx++)
        {   current_coal_reserve += world.map(xx,yy)->coal_reserve;}
    }
    // mine some coal
    if ((current_coal_reserve > 0)
    && (commodityCount[STUFF_COAL] <= TARGET_COAL_LEVEL * (MAX_COAL_AT_MINE - COAL_PER_RESERVE)/100)
    && (commodityCount[STUFF_LABOR] >= COALMINE_LABOR))
    {
        for (int yy = ys; (yy < ye) && !coal_found; yy++)
        {
            for (int xx = xs; (xx < xe) && !coal_found; xx++)
            {
                if (world.map(xx,yy)->coal_reserve > 0)
                {
                    world.map(xx,yy)->coal_reserve--;
                    world.map(xx,yy)->pollution += COALMINE_POLLUTION;
                    produceStuff(STUFF_COAL, COAL_PER_RESERVE);
                    consumeStuff(STUFF_LABOR, COALMINE_LABOR);
                    if (current_coal_reserve < initial_coal_reserve)
                      world.stats.sustainability.mining_flag = false;
                    coal_found = true;
                    working_days++;
                }
            }
        }
    }
    else if ((commodityCount[STUFF_COAL] - COAL_PER_RESERVE > TARGET_COAL_LEVEL * (MAX_COAL_AT_MINE)/100)
    && (commodityCount[STUFF_LABOR] >= COALMINE_LABOR))
    {
        for (int yy = ys; (yy < ye) && !coal_found; yy++)
        {
            for (int xx = xs; (xx < xe) && !coal_found; xx++)
            {
                if (world.map(xx,yy)->coal_reserve < COAL_RESERVE_SIZE)
                {
                    world.map(xx,yy)->coal_reserve++;
                    consumeStuff(STUFF_COAL, COAL_PER_RESERVE);
                    consumeStuff(STUFF_LABOR, COALMINE_LABOR);
                    coal_found = true;
                    working_days++;
                }
            }
        }
    }
    //Monthly update of activity
    if (world.total_time % 100 == 99) {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }

    // TODO: This may prevent unmining when reserve gets to 0.
    //Evacuate Mine if no more deposits
    if (current_coal_reserve == 0 )
    {   flags |= FLAG_EVACUATE;}

    //Abandon the Coalmine if it is really empty
    if ((current_coal_reserve == 0)
      &&(commodityCount[STUFF_LABOR] == 0)
      &&(commodityCount[STUFF_COAL] == 0) )
    {   ConstructionManager::submitRequest(new ConstructionDeletionRequest(this));}
}

void Coalmine::animate() {
  //choose type depending on availabe coal
  // TODO: make sure case 'nothing' can actually happen
  if(commodityCount[STUFF_COAL] > MAX_COAL_AT_MINE - (MAX_COAL_AT_MINE/4))//75%
  {   frameIt->resourceGroup = ResourceGroup::resMap["CoalMineFull"];}
  else if (commodityCount[STUFF_COAL] > MAX_COAL_AT_MINE / 2)//50%
  {   frameIt->resourceGroup = ResourceGroup::resMap["CoalMineMed"];}
  else if (commodityCount[STUFF_COAL] > 0)//something
  {   frameIt->resourceGroup = ResourceGroup::resMap["CoalMineLow"];}
  else//nothing
  {   frameIt->resourceGroup = ResourceGroup::resMap["CoalMine"];}
  soundGroup = frameIt->resourceGroup;
}

void Coalmine::report()
{
    int i = 0;
    mps_store_title(i, constructionGroup->name);
    mps_store_sfp(i++, N_("busy"), busy);
    mps_store_sddp(i++, N_("Deposits"), current_coal_reserve, initial_coal_reserve);
    // i++;
    list_commodities(&i);
}

void Coalmine::place(int x, int y) {
  Construction::place(x, y);

  int coal = 0;
  int lenm1 = world.map.len()-1;
  int tmp;
  tmp = x - constructionGroup->range;
  this->xs = (tmp < 1) ? 1 : tmp;
  tmp = y - constructionGroup->range;
  this->ys = (tmp < 1) ? 1 : tmp;
  tmp = x + constructionGroup->range + constructionGroup->size;
  this->xe = (tmp > lenm1) ? lenm1 : tmp;
  tmp = y + constructionGroup->range + constructionGroup->size;
  this->ye = (tmp > lenm1) ? lenm1 : tmp;

  for(int yy = ys; yy < ye ; yy++)
  for (int xx = xs; xx < xe ; xx++)
    coal += world.map(xx,yy)->coal_reserve;

  //always provide some coal so player can
  //store sustainable coal before the mine is deleted
  if (coal < 20)
  {
      world.map(x,y)->coal_reserve += 20-coal;
      coal = 20;
  }

  this->initial_coal_reserve = coal;
  this->current_coal_reserve = coal;
}

void Coalmine::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"initial_coal_reserve", "%d", initial_coal_reserve);
  Construction::save(xmlWriter);
}

bool Coalmine::loadMember(xmlpp::TextReader& xmlReader) {
  std::string name = xmlReader.get_name();
  if(name == "initial_coal_reserve") initial_coal_reserve = std::stoi(xmlReader.read_inner_xml());
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/coalmine.cpp */
