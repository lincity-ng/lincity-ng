/* ---------------------------------------------------------------------- *
 * src/lincity/modules/heavy_industry.cpp
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

#include "heavy_industry.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <list>                           // for _List_iterator
#include <map>                            // for map
#include <string>                         // for basic_string, operator==
#include <vector>                         // for vector

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"             // for GROUP_INDUSTRY_H
#include "lincity/lin-city.hpp"           // for MAX_TECH_LEVEL, ANIM_THRESHOLD
#include "lincity/resources.hpp"          // for ExtraFrame, ResourceGroup
#include "lincity/world.hpp"              // for World, Map, MapTile
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

IndustryHeavyConstructionGroup industryHeavyConstructionGroup(
     N_("Steel Works"),
     N_("Steel Works"),
     FALSE,                     /* need credit? */
     GROUP_INDUSTRY_H,
     GROUP_INDUSTRY_H_SIZE,
     GROUP_INDUSTRY_H_COLOUR,
     GROUP_INDUSTRY_H_COST_MUL,
     GROUP_INDUSTRY_H_BUL_COST,
     GROUP_INDUSTRY_H_FIREC,
     GROUP_INDUSTRY_H_COST,
     GROUP_INDUSTRY_H_TECH,
     GROUP_INDUSTRY_H_RANGE
);

//helper groups for graphics and sound sets, don't add them to ConstructionGroup::groupMap
//IndustryHeavyConstructionGroup industryHeavy_L_ConstructionGroup = industryHeavyConstructionGroup;
//IndustryHeavyConstructionGroup industryHeavy_M_ConstructionGroup = industryHeavyConstructionGroup;
//IndustryHeavyConstructionGroup industryHeavy_H_ConstructionGroup = industryHeavyConstructionGroup;

Construction *IndustryHeavyConstructionGroup::createConstruction(World& world) {
  return new IndustryHeavy(world, this);
}

IndustryHeavy::IndustryHeavy(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  constructionGroup = cstgrp;
  this->tech = world.tech_level;
  this->output_level = 0;
  this->steel_this_month = 0;
  this->anim = 0;
  initialize_commodities();
   //check for pollution bonus
  this->bonus = 0;
  this->extra_bonus = 0;

  int steel_prod = MAX_ORE_USED / ORE_MAKE_STEEL;
  commodityMaxCons[STUFF_HIVOLT] = 100 * (steel_prod * POWER_MAKE_STEEL / 2);
  commodityMaxCons[STUFF_LOVOLT] = 100 * (steel_prod * POWER_MAKE_STEEL);
  commodityMaxCons[STUFF_COAL] = 100 * (steel_prod * COAL_MAKE_STEEL);
  commodityMaxCons[STUFF_LABOR] = 100 * (MAX_ORE_USED / LABOR_MAKE_STEEL +
    LABOR_LOAD_COAL + LABOR_LOAD_ORE + LABOR_LOAD_STEEL);
  commodityMaxCons[STUFF_ORE] = 100 * MAX_ORE_USED;
  commodityMaxProd[STUFF_STEEL] = 100 * steel_prod;
  // commodityMaxProd[STUFF_WASTE] = 100 * (int)(
  //   ((double)(POL_PER_STEEL_MADE * steel_prod) * bonus)*(1-extra_bonus));
}

void IndustryHeavy::update()
{
    // can we produce steel?
    int steel = ( commodityCount[STUFF_LABOR] >= MAX_ORE_USED / LABOR_MAKE_STEEL + LABOR_LOAD_ORE + LABOR_LOAD_COAL + LABOR_LOAD_STEEL
    && commodityCount[STUFF_ORE] >= MAX_ORE_USED
    && commodityCount[STUFF_STEEL] + MAX_ORE_USED / ORE_MAKE_STEEL <= MAX_STEEL_AT_INDUSTRY_H) ? MAX_ORE_USED / ORE_MAKE_STEEL : 0;

    if (steel > 0)
    {
        //Steel works may either use LO-VOLT, HI-VOLT or COAL to produce steel
        int used_hivolt = 0, used_lovolt = 0, used_COAL = 0;
        int powered_steel = 0;
        //First use up HI-VOLT
        used_hivolt = commodityCount[STUFF_HIVOLT];
        if (used_hivolt > (steel - powered_steel) * POWER_MAKE_STEEL / 2)
            used_hivolt = (steel - powered_steel) * POWER_MAKE_STEEL / 2;
        powered_steel += 2 * used_hivolt / POWER_MAKE_STEEL;
        //Second use up LO-VOLT
        if (steel > powered_steel)
        {
            used_lovolt = commodityCount[STUFF_LOVOLT];
            if (used_lovolt > (steel - powered_steel) * POWER_MAKE_STEEL)
                used_lovolt = (steel - powered_steel) * POWER_MAKE_STEEL;
            powered_steel += used_lovolt / POWER_MAKE_STEEL;
            //Third use up COAL
            if (steel > powered_steel)
            {
                used_COAL = commodityCount[STUFF_COAL];
                if (used_COAL > (steel - powered_steel) * COAL_MAKE_STEEL)
                    used_COAL = (steel - powered_steel) * COAL_MAKE_STEEL;
                powered_steel += used_COAL / COAL_MAKE_STEEL;
            }//end Third
        }//end Second
        if (powered_steel == steel)
        {
            consumeStuff(STUFF_HIVOLT, used_hivolt);
            consumeStuff(STUFF_LOVOLT, used_lovolt);
            if(used_COAL)// coal power is more laborous
            {
                consumeStuff(STUFF_COAL, used_COAL);
                consumeStuff(STUFF_LABOR, LABOR_LOAD_COAL);
            }
        }
        else
        {    steel /= 5;} //inefficient and still dirty unpowered production

        if (steel>0)
        {
            consumeStuff(STUFF_LABOR, MAX_ORE_USED / LABOR_MAKE_STEEL);
            //use labor for loading the ore
            consumeStuff(STUFF_LABOR, LABOR_LOAD_ORE);
            //use labor for loading the steel
            consumeStuff(STUFF_LABOR, LABOR_LOAD_STEEL);
            consumeStuff(STUFF_ORE, MAX_ORE_USED);
            produceStuff(STUFF_STEEL, steel);
            steel_this_month += steel;
            //cause some pollution and waste depending on bonuses
            world.map(point)->pollution += (int)(((double)(POL_PER_STEEL_MADE * steel) * (1 - bonus)));
            produceStuff(STUFF_WASTE, (int)(((double)(POL_PER_STEEL_MADE * steel) * bonus)*(1-extra_bonus)));
            // if the trash bin is full reburn the filterd pollution
            if (commodityCount[STUFF_WASTE] > MAX_WASTE_AT_INDUSTRY_H)
            {
                world.map(point)->pollution += (commodityCount[STUFF_WASTE] - MAX_WASTE_AT_INDUSTRY_H);
                levelStuff(STUFF_WASTE, MAX_WASTE_AT_INDUSTRY_H);
            }
        }//endif steel still > 0
    }//endif steel > 0

    //monthly update
    if(world.total_time % 100 == 99) {
        reset_prod_counters();
        output_level = steel_this_month * ORE_MAKE_STEEL / MAX_ORE_USED;
        steel_this_month = 0;
    }//end monthly update
}

void IndustryHeavy::animate(unsigned long real_time) {
  if(real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(INDUSTRY_H_ANIM_SPEED);

    if (output_level > 80)
    {   frameIt->resourceGroup = ResourceGroup::resMap["IndustryHighH"];}
    else if (output_level > 30)
    {   frameIt->resourceGroup = ResourceGroup::resMap["IndustryHighM"];}
    else if (output_level > 0)
    {   frameIt->resourceGroup = ResourceGroup::resMap["IndustryHighL"];}
    else
    {   frameIt->resourceGroup = ResourceGroup::resMap["IndustryHigh"];}
    soundGroup = frameIt->resourceGroup;

    int& frame = frameIt->frame;
    if(++frame >= (int)frameIt->resourceGroup->graphicsInfoVector.size())
      frame = 0;
  }
}

void IndustryHeavy::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  mps.add_sfp(N_("busy"), (output_level));
  mps.add_sfp(N_("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
  list_commodities(mps, production);
}

void
IndustryHeavy::place(MapPoint point) {
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

  int steel_prod = MAX_ORE_USED / ORE_MAKE_STEEL;
  commodityMaxProd[STUFF_WASTE] = 100 * (int)(
    ((double)(POL_PER_STEEL_MADE * steel_prod) * bonus)*(1-extra_bonus));
}

void IndustryHeavy::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"tech", xmlFormat<int>(tech));
  Construction::save(xmlWriter);
}

bool IndustryHeavy::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string name = xmlReader.get_name();
  if(name == "tech") tech = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "bonus");
  else if(name == "extra_bonus");
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/heavy_industry.cpp */
