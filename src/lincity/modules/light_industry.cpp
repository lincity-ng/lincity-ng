/* ---------------------------------------------------------------------- *
 * src/lincity/modules/light_industry.cpp
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

#include "light_industry.h"

#include <stdlib.h>                 // for rand
#include <vector>                   // for vector

#include "modules.h"

// IndustryLight:
IndustryLightConstructionGroup industryLightConstructionGroup(
     N_("Light Industry"),
     FALSE,                     /* need credit? */
     GROUP_INDUSTRY_L,
     GROUP_INDUSTRY_L_SIZE,
     GROUP_INDUSTRY_L_COLOUR,
     GROUP_INDUSTRY_L_COST_MUL,
     GROUP_INDUSTRY_L_BUL_COST,
     GROUP_INDUSTRY_L_FIREC,
     GROUP_INDUSTRY_L_COST,
     GROUP_INDUSTRY_L_TECH,
     GROUP_INDUSTRY_L_RANGE
);

//helper groups for graphics and sound sets, dont add them to ConstructionGroup::groupMap
//IndustryLightConstructionGroup industryLight_Q_ConstructionGroup = industryLightConstructionGroup;
//IndustryLightConstructionGroup industryLight_L_ConstructionGroup = industryLightConstructionGroup;
//IndustryLightConstructionGroup industryLight_M_ConstructionGroup = industryLightConstructionGroup;
//IndustryLightConstructionGroup industryLight_H_ConstructionGroup = industryLightConstructionGroup;

Construction *IndustryLightConstructionGroup::createConstruction() {
  return new IndustryLight(this);
}

void IndustryLight::update()
{
    //monthly update
    if (total_time % 100 == 0)
    {
        reset_prod_counters();
        int output_level = goods_this_month / (INDUSTRY_L_MAKE_GOODS * 8);
        busy = output_level;
        goods_this_month = 0;
    }// end monthly update

    goods_today = 0;

    // make some goods with labor and ore
    if ((commodityCount[STUFF_LABOR] >= (INDUSTRY_L_LABOR_USED + INDUSTRY_L_LABOR_LOAD_ORE + LABOR_LOAD_ORE))
     && (commodityCount[STUFF_ORE] >= INDUSTRY_L_ORE_USED)
     && (commodityCount[STUFF_GOODS] + INDUSTRY_L_MAKE_GOODS <= MAX_GOODS_AT_INDUSTRY_L))
    {
        consumeStuff(STUFF_LABOR, INDUSTRY_L_LABOR_USED + INDUSTRY_L_LABOR_LOAD_ORE + LABOR_LOAD_ORE);
        consumeStuff(STUFF_ORE, INDUSTRY_L_ORE_USED);
        goods_today = INDUSTRY_L_MAKE_GOODS;
        //make some pollution and waste
        world(x,y)->pollution += (int)(((double)(INDUSTRY_L_POL_PER_GOOD * goods_today) * (1 - bonus)));
        produceStuff(STUFF_WASTE, (int)(((double)(INDUSTRY_L_POL_PER_GOOD * goods_today) * bonus)*(1-extra_bonus)));
        // if the trash bin is full reburn the filterd pollution
        if (commodityCount[STUFF_WASTE] > MAX_WASTE_AT_INDUSTRY_L)
        {
            int excess = -levelStuff(STUFF_WASTE, MAX_WASTE_AT_INDUSTRY_L);
            world(x,y)->pollution += excess;
        }
        //now double goods_today if there are more labor and steel
        if ((commodityCount[STUFF_LABOR] >= (INDUSTRY_L_LABOR_LOAD_STEEL + LABOR_LOAD_STEEL))
         && (commodityCount[STUFF_STEEL] >= INDUSTRY_L_STEEL_USED )
        && (commodityCount[STUFF_GOODS] + 2 * goods_today <= MAX_GOODS_AT_INDUSTRY_L))
        {
            consumeStuff(STUFF_LABOR, INDUSTRY_L_LABOR_LOAD_STEEL + LABOR_LOAD_STEEL);
            consumeStuff(STUFF_STEEL, INDUSTRY_L_STEEL_USED);
            goods_today *= 2;
        }
        //now check for more ore and power to quadruple goods_today again
        //light industry can use LO-VOLT and HI-VOLT simultaneously
        int total_power_p_good = (commodityCount[STUFF_LOVOLT] + 2 * commodityCount[STUFF_HIVOLT]) / (4 * goods_today);
        if ((total_power_p_good >= INDUSTRY_L_POWER_PER_GOOD)
         && (commodityCount[STUFF_ORE] >= INDUSTRY_L_ORE_USED)
         && (commodityCount[STUFF_GOODS] + 4 * goods_today <= MAX_GOODS_AT_INDUSTRY_L))
        {
            goods_today *= 4;
            consumeStuff(STUFF_ORE, INDUSTRY_L_ORE_USED);
            //prefer the mor abundant power
            if(2 * commodityCount[STUFF_HIVOLT] > commodityCount[STUFF_LOVOLT])
            {
                consumeStuff(STUFF_HIVOLT, INDUSTRY_L_POWER_PER_GOOD * (goods_today / 2));
                if(commodityCount[STUFF_HIVOLT] < 0)
                {
                    int deficit = levelStuff(STUFF_HIVOLT, 0);
                    consumeStuff(STUFF_LOVOLT, deficit * 2);
                }
            }
            else
            {
                consumeStuff(STUFF_LOVOLT, INDUSTRY_L_POWER_PER_GOOD * goods_today);
                if(commodityCount[STUFF_LOVOLT] < 0)
                {
                    int deficit = levelStuff(STUFF_LOVOLT, 0);
                    consumeStuff(STUFF_HIVOLT, deficit / 2);
                }
            }
        }
        produceStuff(STUFF_GOODS, goods_today);
        goods_this_month += goods_today;
    }// endif make some goods
}

void IndustryLight::animate() {
  if(real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(INDUSTRY_L_ANIM_SPEED);

    //Choose an animation set depending on output_level
    if (busy > 80)
    {   frameIt->resourceGroup = ResourceGroup::resMap["IndustryLightH"];}
    else if (busy > 55)
    {   frameIt->resourceGroup = ResourceGroup::resMap["IndustryLightM"];}
    else if (busy > 25)
    {   frameIt->resourceGroup = ResourceGroup::resMap["IndustryLightL"];}
    else
    {   frameIt->resourceGroup = ResourceGroup::resMap["IndustryLight"];}
    soundGroup = frameIt->resourceGroup;

    int active = 0;
    if(busy > 70)
      active = 2;
    else if (busy > 5)
      active = 1;
    std::list<ExtraFrame>::iterator frit = fr_begin;
    for(int i = 0; i < 2 && frit != fr_end; ++i, std::advance(frit, 1)) {
      int s = frit->resourceGroup->graphicsInfoVector.size();
      int& smoke = frit->frame;
      if (i >= active) {
        smoke = -1;
      }
      else if(!s) ;
      else if(smoke < 0 || rand() % 1600) {
        // always randomize new plumes and sometimes existing ones
        smoke = rand() % s;
      }
      else if(goods_today && ++smoke >= s) {
        smoke = 0;
      }
    }

    // This is left over from when the smoke was not rendered separately.
    // int& frame = frameIt->frame;
    // if(++frame >= (int)frameIt->resourceGroup->graphicsInfoVector.size())
    //   frame = 0;
  }
}

void IndustryLight::report()
{
    int i = 0;

    mps_store_title(i, constructionGroup->name);
    i++;
    mps_store_sfp(i++, N_("busy"), (busy));
    mps_store_sfp(i++, N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    // i++;
    list_commodities(&i);
}

void IndustryLight::init_resources() {
  Construction::init_resources();

  world(x,y)->framesptr->resize(world(x,y)->framesptr->size()+2);
  std::list<ExtraFrame>::iterator frit = frameIt;
  std::advance(frit, 1);
  fr_begin = frit;
  frit = frameIt;
  std::advance(frit, 1);
  frit->move_x = -113;
  frit->move_y = -210;
  std::advance(frit, 1);
  frit->move_x = -84;
  frit->move_y = -198;
  std::advance(frit, 1);
  fr_end = frit;
  for(frit = fr_begin;
    frit != world(x,y)->framesptr->end() && frit != fr_end;
    std::advance(frit, 1)
  ) {
    frit->resourceGroup = ResourceGroup::resMap["GraySmoke"];
    frit->frame = -1; // hide smoke
  }
}

void IndustryLight::save(xmlTextWriterPtr xmlWriter) {
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tech", "%d", tech);
  Construction::save(xmlWriter);
}

bool IndustryLight::loadMember(xmlpp::TextReader& xmlReader) {
  std::string name = xmlReader.get_name();
  if(name == "tech") tech = std::stoi(xmlReader.read_inner_xml());
  else if(name == "bonus");
  else if(name == "extra_bonus");
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/light_industry.cpp */
