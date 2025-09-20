/* ---------------------------------------------------------------------- *
 * src/lincity/modules/market.cpp
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

#include "market.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <algorithm>                      // for max, min
#include <cstdlib>                        // for size_t
#include <map>                            // for map
#include <random>                         // for bernoulli_distribution
#include <string>                         // for basic_string, operator<
#include <vector>                         // for vector

#include "fire.hpp"                       // for FIRE_ANIMATION_SPEED
#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"             // for GROUP_MARKET
#include "lincity/lin-city.hpp"           // for FLAG_EVACUATE, ANIM_THRESHOLD
#include "lincity/messages.hpp"           // for FireStartedMessage
#include "lincity/resources.hpp"          // for ExtraFrame, ResourceGroup
#include "lincity/world.hpp"              // for World, Map, MapTile
#include "util/gettextutil.hpp"           // for N_
#include "util/randutil.hpp"              // for BasicUrbg
#include "util/xmlutil.hpp"               // for xmlFormat, xmlStr, xmlStrF

MarketConstructionGroup marketConstructionGroup(
     N_("Market"),
     N_("Markets"),
     FALSE,                     /* need credit? */
     GROUP_MARKET,
     GROUP_MARKET_SIZE,
     GROUP_MARKET_COLOUR,
     GROUP_MARKET_COST_MUL,
     GROUP_MARKET_BUL_COST,
     GROUP_MARKET_FIREC,
     GROUP_MARKET_COST,
     GROUP_MARKET_TECH,
     GROUP_MARKET_RANGE
);

//helper groups for graphics and sound sets, dont add them to ConstructionGroup::groupMap
//MarketConstructionGroup market_low_ConstructionGroup  = marketConstructionGroup;
//MarketConstructionGroup market_med_ConstructionGroup  = marketConstructionGroup;
//MarketConstructionGroup market_full_ConstructionGroup = marketConstructionGroup;


Construction *MarketConstructionGroup::createConstruction(World& world) {
  return new Market(world, this);
}

Market::Market(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  //local copy of commodityRuCount
  commodityRuleCount = constructionGroup->commodityRuleCount;
  initialize_commodities();
  this->labor = LABOR_MARKET_EMPTY;
  this->anim = 0;
  this->busy = 0;
  this->working_days = 0;
  this->market_ratio = 0;
  this->start_burning_waste = false;
  this->waste_fire_anim = 0;

  commodityMaxCons[STUFF_LABOR] = 100 * LABOR_MARKET_FULL;
  commodityMaxCons[STUFF_WASTE] = 100 * ((7 * MAX_WASTE_IN_MARKET) / 10);
}

Market::~Market() {
  world.map(point)->killframe(waste_fire_frit);
}

void Market::update()
{
    int ratio, trade_ratio, n;
    int lvl, market_lvl;
    int cap, market_cap;
    market_ratio = 0;
    const size_t partsize = partners.size();
    std::vector<bool> lvls(partsize);
    Commodity stuff_ID;
    n = 0;
    for(stuff_ID = STUFF_INIT ; stuff_ID < STUFF_COUNT ; stuff_ID++ )
    {
        //dont handle stuff if neither give nor take
        //dont handle anything else if there are to little labor
        const CommodityRule& market_rule = commodityRuleCount[stuff_ID];
        if ((!market_rule.give
         && !market_rule.take) ||
         (commodityCount[STUFF_LABOR] < labor && stuff_ID != STUFF_LABOR))
        {   continue;}

        market_lvl = commodityCount[stuff_ID];
        market_cap = market_rule.maxload;
        ratio = market_lvl * TRANSPORT_QUANTA / market_cap;
        market_ratio += ratio;
        n++;
        lvl = market_lvl;
        cap = market_cap;
        for(unsigned int i = 0; i < lvls.size(); ++i)
        {
            lvls[i] = false;
            Construction *pear = partners[i];
            const CommodityRule& pearrule = pear->constructionGroup->commodityRuleCount[stuff_ID];
            if(pearrule.maxload)
            {
                int lvlsi = pear->commodityCount[stuff_ID];
                int capsi = pearrule.maxload;
                if(pear->flags & FLAG_EVACUATE)
                {   capsi = 0;}
                else
                {
                    int pearat = lvlsi * TRANSPORT_QUANTA / capsi;
                    //only consider stuff that would tentatively move
                    //Here the local rules of this market apply
                    if(((pearat > ratio)&&!(market_rule.take &&
                            pearrule.give)) ||
                       ((pearat < ratio)&&!(market_rule.give &&
                            pearrule.take)))
                    {   continue;}
                }
                lvls[i] = true;
                lvl += lvlsi;
                cap += capsi;
            }
        }
        trade_ratio = lvl * TRANSPORT_QUANTA / cap;
        for(unsigned int i = 0; i < lvls.size(); ++i)
        {
            if(lvls[i])
            {   partners[i]->equilibrate_stuff(&market_lvl, market_rule, trade_ratio, stuff_ID);}
        }
        commodityCount[stuff_ID] = market_lvl;
    }

    if (commodityCount[STUFF_LABOR] >= labor)
    {
        consumeStuff(STUFF_LABOR, labor);
        //Have to collect taxes here since transport does not consider the market a consumer but rather as another transport
        world.taxable.labor += labor;
        ++working_days;
    }

    if(world.total_time % 50)
    if(commodityCount[STUFF_WASTE] >= 85 * MAX_WASTE_IN_MARKET / 100) {
      start_burning_waste = true;
      world.map(point.s().e())->pollution += MAX_WASTE_IN_MARKET/20;
      consumeStuff(STUFF_WASTE, (7 * MAX_WASTE_IN_MARKET) / 10);

      double uncontrolledP = WASTE_BURN_UNCONTROLLED
        * constructionGroup->fire_chance / 100;
      if(std::bernoulli_distribution(uncontrolledP)(BasicUrbg::get())
        && !(world.map(point)->flags & FLAG_FIRE_COVER)
      ) {
        torch();
        world.pushMessage(
          FireStartedMessage::create(point, *constructionGroup));
      }
    }

    //monthly update
    if(world.total_time % 100 == 99) {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }
    if(world.total_time % 25 == 17)
    {
        //average filling of the market, catch n == 0 in case market has
        //not yet any commodities initialized
        if (n > 0)
        {   market_ratio = 100*market_ratio/(n * TRANSPORT_QUANTA);}
        else
        {   market_ratio = 0;}

        if (market_ratio < 10)
        {
            labor = LABOR_MARKET_EMPTY;
        }
        else if (market_ratio < 20)
        {
            labor = LABOR_MARKET_LOW;
        }
        else if (market_ratio < 50)
        {
            labor = LABOR_MARKET_MED;
        }
        else
        {
            labor = LABOR_MARKET_FULL;
        }
    }

    if(world.total_time % DAYS_BETWEEN_COVER == 75)
      cover();
}

void Market::cover() {
  MapPoint nw(
    std::max(point.x - constructionGroup->range, 1),
    std::max(point.y - constructionGroup->range, 1)
  );
  MapPoint se(
    std::min(point.x + constructionGroup->range, world.map.len() - 1),
    std::min(point.y + constructionGroup->range, world.map.len() - 1)
  );
  for(MapPoint p(nw); p.y < se.y; p.y++)
  for(p.x = nw.x; p.x < se.x; p.x++)
    world.map(p)->flags |= FLAG_MARKET_COVER_CHECK;
}

void Market::animate(unsigned long real_time) {
  if (market_ratio < 10) {
      frameIt->resourceGroup = ResourceGroup::resMap["MarketEmpty"];
  }
  else if (market_ratio < 20) {
      frameIt->resourceGroup = ResourceGroup::resMap["MarketLow"];
  }
  else if (market_ratio < 50) {
      frameIt->resourceGroup = ResourceGroup::resMap["MarketMed"];
  }
  else {
      frameIt->resourceGroup = ResourceGroup::resMap["MarketFull"];
  }
  soundGroup = frameIt->resourceGroup;

  if(start_burning_waste) { // start fire
    start_burning_waste = false;
    anim = real_time + ANIM_THRESHOLD(6 * WASTE_BURN_TIME);
  }
  if(real_time >= anim) { // stop fire
    waste_fire_frit->frame = -1;
  }
  else if(real_time >= waste_fire_anim) { // continue fire
    waste_fire_anim = real_time + ANIM_THRESHOLD(FIRE_ANIMATION_SPEED);
    int num_frames = waste_fire_frit->resourceGroup->graphicsInfoVector.size();
    if(++waste_fire_frit->frame >= num_frames)
      waste_fire_frit->frame = 0;
  }
}

void Market::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  mps.add_sfp(N_("busy"), (float)busy);
  mps.addBlank();
  //list_commodities(mps, production);
  for(Commodity stuff = STUFF_INIT; stuff < STUFF_COUNT; stuff++) {
    const CommodityRule& rule = commodityRuleCount[stuff];
    if(!rule.maxload) continue;
    char arrows[4]="---";
    if (flags & FLAG_EVACUATE) {
      arrows[0] = '<';
      arrows[1] = '<';
      arrows[2] = ' ';
    }
    else {
      if(rule.take)
        arrows[2] = '>';
      if(rule.give)
        arrows[0] = '<';
    }

    mps.add_tsddp(arrows, commodityName(stuff),
      commodityCount[stuff], rule.maxload);
  }
}

void Market::init_resources() {
  Construction::init_resources();

  waste_fire_frit = world.map(point)->createframe();
  waste_fire_frit->resourceGroup = ResourceGroup::resMap["Fire"];
  waste_fire_frit->move_x = 0;
  waste_fire_frit->move_y = 0;
  waste_fire_frit->frame = -1;
}

void
Market::place(MapPoint point) {
  Construction::place(point);
  cover();
}

void Market::toggleEvacuation()
{
    bool evacuate = flags & FLAG_EVACUATE; //actually the previous state
    for(Commodity stuff = STUFF_INIT ; stuff < STUFF_COUNT ; stuff++)
    {
        CommodityRule& rule = commodityRuleCount[stuff];
        if(!rule.maxload) continue;
        if(!evacuate)
        {
            rule.give = true;
            rule.take = false;
        }
        else
        {
            rule.give = true;
            rule.take = true;
        }

    }
    flags &= ~FLAG_EVACUATE;
    if(!evacuate)
    {   flags |= FLAG_EVACUATE;}
}

void Market::save(xmlTextWriterPtr xmlWriter) const {
  const std::string givePfx("give_");
  const std::string takePfx("take_");
  for(Commodity stuff = STUFF_INIT; stuff < STUFF_COUNT; stuff++) {
    const CommodityRule& rule = commodityRuleCount[stuff];
    if(!rule.maxload) continue;
    const char *name = commodityStandardName(stuff);
    const std::string giveName = givePfx + name;
    const std::string takeName = takePfx + name;
    const xmlStr giveNameXml = (xmlStr)giveName.c_str();
    const xmlStr takeNameXml = (xmlStr)takeName.c_str();
    xmlTextWriterWriteElement(xmlWriter, giveNameXml, xmlFormat<int>(rule.give));
    xmlTextWriterWriteElement(xmlWriter, takeNameXml, xmlFormat<int>(rule.take));
  }

  Construction::save(xmlWriter);
}

bool Market::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string tag = xmlReader.get_name();
  bool give;
  Commodity stuff;
  if(((give = tag.find("give_")) == 0 || tag.find("take_") == 0) &&
    (stuff = commodityFromStandardName(tag.substr(5).c_str())) != STUFF_COUNT &&
    commodityRuleCount[stuff].maxload
  ) {
    CommodityRule& rule = commodityRuleCount[stuff];
    give ? rule.give : rule.take = xmlParse<int>(xmlReader.read_inner_xml());
  }
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/market.cpp */
