/* ---------------------------------------------------------------------- *
 * src/lincity/modules/light_industry.cpp
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

#include "light_industry.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <stdlib.h>                       // for rand
#include <map>                            // for map
#include <string>                         // for basic_string, operator==
#include <vector>                         // for vector

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"             // for GROUP_INDUSTRY_L
#include "lincity/lin-city.hpp"           // for MAX_TECH_LEVEL, ANIM_THRESHOLD
#include "lincity/resources.hpp"          // for ExtraFrame, ResourceGroup
#include "lincity/world.hpp"              // for World, Map, MapTile
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

IndustryLightConstructionGroup industryLightConstructionGroup(
     N_("Light Industry"),
     N_("Light Industries"),
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

Construction *IndustryLightConstructionGroup::createConstruction(World& world) {
  return new IndustryLight(world, this);
}

IndustryLight::IndustryLight(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->tech = world.tech_level;
  this->working_days = 0;
  this->busy = 0;
  this->goods_this_month = 0;
  this->anim = 0;
  initialize_commodities();
  this->bonus = 0;
  this->extra_bonus = 0;
  // if (tech > MAX_TECH_LEVEL)
  // {
  //     bonus = (tech - MAX_TECH_LEVEL);
  //     if (bonus > MAX_TECH_LEVEL)
  //         bonus = MAX_TECH_LEVEL;
  //     bonus /= MAX_TECH_LEVEL;
  //     // check for filter technology bonus
  //     if (tech > 2 * MAX_TECH_LEVEL)
  //     {
  //         extra_bonus = tech - 2 * MAX_TECH_LEVEL;
  //         if (extra_bonus > MAX_TECH_LEVEL)
  //             extra_bonus = MAX_TECH_LEVEL;
  //         extra_bonus /= MAX_TECH_LEVEL;
  //     }
  // }

  commodityMaxCons[STUFF_LABOR] = 100 * (INDUSTRY_L_LABOR_USED +
    INDUSTRY_L_LABOR_LOAD_ORE + LABOR_LOAD_ORE +
    INDUSTRY_L_LABOR_LOAD_STEEL + LABOR_LOAD_STEEL);
  commodityMaxCons[STUFF_ORE] = 100 * INDUSTRY_L_ORE_USED * 2;
  commodityMaxCons[STUFF_STEEL] = 100 * INDUSTRY_L_STEEL_USED;
  commodityMaxCons[STUFF_LOVOLT] = 100 *
    INDUSTRY_L_POWER_PER_GOOD * INDUSTRY_L_MAKE_GOODS * 8;
  commodityMaxCons[STUFF_HIVOLT] = 100 *
    INDUSTRY_L_POWER_PER_GOOD * INDUSTRY_L_MAKE_GOODS * 4;
  commodityMaxProd[STUFF_GOODS] = 100 * INDUSTRY_L_MAKE_GOODS * 8;
  // commodityMaxProd[STUFF_WASTE] = 100 * (int)(INDUSTRY_L_POL_PER_GOOD *
  //   INDUSTRY_L_MAKE_GOODS * bonus * (1-extra_bonus));
}

IndustryLight::~IndustryLight() {
}

void IndustryLight::update()
{
    //monthly update
    if(world.total_time % 100 == 0)
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
        world.map(point)->pollution += (int)(((double)(INDUSTRY_L_POL_PER_GOOD * goods_today) * (1 - bonus)));
        produceStuff(STUFF_WASTE, (int)(((double)(INDUSTRY_L_POL_PER_GOOD * goods_today) * bonus)*(1-extra_bonus)));
        // if the trash bin is full reburn the filterd pollution
        if (commodityCount[STUFF_WASTE] > MAX_WASTE_AT_INDUSTRY_L)
        {
            int excess = -levelStuff(STUFF_WASTE, MAX_WASTE_AT_INDUSTRY_L);
            world.map(point)->pollution += excess;
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

void IndustryLight::animate(unsigned long real_time) {
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
    else if (busy > 10)
      active = 1;
    for(int i = 0; i < frits.size(); i++) {
      auto& frit = frits[i];
      int s = frit->resourceGroup->graphicsInfoVector.size();
      int& smoke = frit->frame;
      if(!s);
      else if(i >= active) {
        smoke = -1;
      }
      else if(smoke < 0 || !(rand() % 1600)) {
        // randomize new plumes and sometimes existing ones
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

void IndustryLight::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  mps.add_sfp(N_("busy"), (busy));
  mps.add_sfp(N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
  list_commodities(mps, production);
}

void IndustryLight::init_resources() {
  Construction::init_resources();

  MapTile& tile = *world.map(point);
  for(auto& frit : frits) {
    frit = tile.createframe();
    frit->resourceGroup = ResourceGroup::resMap["GraySmoke"];
    frit->frame = -1; // hide smoke
  }
  frits[0]->move_x = -113;
  frits[0]->move_y = -210;
  frits[1]->move_x = -84;
  frits[1]->move_y = -198;
}

void
IndustryLight::detach() {
  MapTile& tile = *world.map(point);
  for(const auto& frit : frits) {
    tile.killframe(frit);
  }
  Construction::detach();
}

void
IndustryLight::place(MapPoint point) {
  Construction::place(point);

  if (tech > MAX_TECH_LEVEL) {
    bonus = (tech - MAX_TECH_LEVEL);
    if (bonus > MAX_TECH_LEVEL)
      bonus = MAX_TECH_LEVEL;
    bonus /= MAX_TECH_LEVEL;
    // check for filter technology bonus
    if (tech > 2 * MAX_TECH_LEVEL) {
      extra_bonus = tech - 2 * MAX_TECH_LEVEL;
      if (extra_bonus > MAX_TECH_LEVEL)
        extra_bonus = 1;
      else
        extra_bonus /= MAX_TECH_LEVEL;
    }
  }

  commodityMaxProd[STUFF_WASTE] = 100 * (int)(INDUSTRY_L_POL_PER_GOOD *
    INDUSTRY_L_MAKE_GOODS * bonus * (1-extra_bonus));
}

void IndustryLight::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"tech", xmlFormat<int>(tech));
  Construction::save(xmlWriter);
}

bool IndustryLight::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string name = xmlReader.get_name();
  if(name == "tech") tech = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "bonus");
  else if(name == "extra_bonus");
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/light_industry.cpp */
