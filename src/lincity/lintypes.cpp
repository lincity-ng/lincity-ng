/* ---------------------------------------------------------------------- *xmlFormat
 * src/lincity/lintypes.cpp
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

#include "lintypes.hpp"

#include <assert.h>                       // for assert
#include <fmt/base.h>                     // for println
#include <fmt/format.h>                   // for native_formatter::format
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <cstdio>                         // for stderr
#include <cstdlib>                        // for rand
#include <algorithm>                      // for max, min
#include <iostream>                       // for basic_ostream, operator<<
#include <set>                            // for set
#include <stdexcept>                      // for logic_error, runtime_error
#include <utility>                        // for pair
#include <vector>                         // for vector

#include "MapPoint.hpp"                   // for MapPoint, operator<<
#include "Vehicles.hpp"                   // for Vehicle, VehicleStrategy
#include "commodities.hpp"                // for CommodityRule, Commodity
#include "groups.hpp"                     // for GROUP_POWER_LINE, GROUP_FIRE
#include "lin-city.hpp"                   // for FLAG_EVACUATE, FLAG_IS_TRAN...
#include "lincity-ng/Config.hpp"          // for getConfig, Config
#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity-ng/Sound.hpp"           // for getSound, Sound
#include "modules/all_modules.hpp"        // for Powerline, GROUP_MARKET_RANGE
#include "stats.hpp"                      // for Stats, Stat
#include "transport.hpp"                  // for TRANSPORT_QUANTA, TRANSPORT...
#include "util.hpp"                       // for used_in_assert
#include "util/gettextutil.hpp"           // for _
#include "util/xmlutil.hpp"               // for xmlParse, xmlStr, xmlStrF
#include "world.hpp"                      // for World, Map, MapTile

extern int simDelay; // is defined in lincity-ng/MainLincity.cpp


//Construction Declarations

Construction::Construction(World& world) :
  world(world)
{
  for(Commodity stuff = STUFF_INIT; stuff < STUFF_COUNT; stuff++) {
    commodityCount[stuff] = 0;
    commodityProd[stuff] = 0;
    commodityProdPrev[stuff] = 0;
    commodityMaxProd[stuff] = 0;
    commodityMaxCons[stuff] = 0;
  }
}

std::string Construction::getStuffName(Commodity stuff_id)
{
    return commodityNames[stuff_id];
}

void Construction::list_commodities(Mps& mps, bool production) const {
  mps.add_s(production ? _("Production:") : _("Inventory:"));
  for(int r = 0; r < 4; r++) {
    const char *arrow;
    bool give, take;
    switch(r) {
      case 1: arrow = "--> "; give = false; take = true ; break;
      case 0: arrow = "<-- "; give = true ; take = false; break;
      case 2: arrow = "<-> "; give = true ; take = true ; break;
      case 3: arrow = "--- "; give = false; take = false; break;
      default: assert(false);
    }
    if(!production && (flags & FLAG_EVACUATE))
      arrow = "<<< ";

    for(Commodity stuff = STUFF_INIT; stuff < STUFF_COUNT; stuff++) {
      const CommodityRule& rule = constructionGroup->commodityRuleCount[stuff];
      const int& maxprod = commodityMaxProd[stuff];
      const int& maxcons = commodityMaxCons[stuff];
      const bool thisgive = production ? maxprod : rule.give;
      const bool thistake = production ? maxcons : rule.take;
      if(rule.maxload && thisgive == give && thistake == take) {
        int amt = (production ? commodityProdPrev : commodityCount)[stuff];
        int max = production ? (amt >= 0 ? maxprod : -maxcons) : rule.maxload;
        mps.add_tsddp(arrow, commodityNames[stuff], amt, max);
      }
    }
  }
}

void Construction::reset_prod_counters(void) {
    for(Commodity stuff = STUFF_INIT ; stuff < STUFF_COUNT; stuff++) {
        commodityProdPrev[stuff] = commodityProd[stuff];
        commodityProd[stuff] = 0;
#if DEBUG
    if(commodityProdPrev[stuff] > commodityMaxProd[stuff]) {
        // commodityMaxProd[stuff] = commodityProdPrev[stuff];
        std::cerr << "warning:"
          << " construction "
          << constructionGroup->name
          << " exceeded maximum production of commodity "
          << commodityNames[stuff] << "."
          // << " Updating maximum production."
          << '\n';
    }
    if(-commodityProdPrev[stuff] > commodityMaxCons[stuff]) {
        // commodityMaxCons[stuff] = -commodityProdPrev[stuff];
        std::cerr << "warning:"
          << " construction "
          << constructionGroup->name
          << " exceeded maximum consumption of commodity "
          << commodityNames[stuff] << "."
          // << " Updating maximum production."
          << '\n';
    }
#endif
    } //endfor
}

int Construction::produceStuff(Commodity stuff_id, int amt) {
    commodityProd[stuff_id] += amt;
    commodityCount[stuff_id] += amt;
    return amt;
}

int Construction::consumeStuff(Commodity stuff_id, int amt) {
    commodityProd[stuff_id] -= amt;
    commodityCount[stuff_id] -= amt;
    return amt;
}

int Construction::levelStuff(Commodity stuff_id, int amt) {
    int delta = amt - commodityCount[stuff_id];
    commodityProd[stuff_id] += delta;
    commodityCount[stuff_id] = amt;
    return delta;
}

void Construction::initialize_commodities(void)
{
    for(Commodity stuff = STUFF_INIT; stuff < STUFF_COUNT; stuff++)
    {
        commodityCount[stuff] = 0;
        commodityProd[stuff] = 0;
        commodityProdPrev[stuff] = 0;
        commodityMaxProd[stuff] = 0;
        commodityMaxCons[stuff] = 0;
    }
}

void Construction::init_resources()
{
    frameIt = world.map(point)->createframe();
    ResourceGroup *resGroup = ResourceGroup::resMap[constructionGroup->resourceID];
    if (resGroup)
    {
        soundGroup = resGroup;
        //graphicsGroup = resGroup;
        frameIt->resourceGroup = resGroup;
        //std::cout << "graphics for " << constructionGroup->name << "at " << x << ", " << y << std::endl;
    }
#ifdef DEBUG
    else
    {
        std::cout << "missing sounds and graphics for:" << constructionGroup->name << std::endl;
    }
#endif
}

void Construction::bootstrap_commodities(int percent)
{
    for(Commodity stuff = STUFF_INIT ; stuff < STUFF_COUNT ; stuff++)
    {
        CommodityRule& rule = constructionGroup->commodityRuleCount[stuff];
        if (rule.maxload && stuff != STUFF_WASTE)
        {   commodityCount[stuff] = percent * rule.maxload /100;}
    }
}

void Construction::report_commodities(void) {
  for(Commodity stuff = STUFF_INIT ; stuff < STUFF_COUNT ; stuff++) {
    world.stats.inventory[stuff] += (Stats::Inventory<>){
      .amount = commodityCount[stuff],
      .capacity = constructionGroup->commodityRuleCount[stuff].maxload
    };
  }
}

void Construction::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"flags",
    xmlFormatHex(flags & ~VOLATILE_FLAGS));
  for(Commodity stuff = STUFF_INIT; stuff < STUFF_COUNT; stuff++) {
    if(!constructionGroup->commodityRuleCount[stuff].maxload) continue;
    xmlStr name = (xmlStr)commodityStandardName(stuff);
    xmlTextWriterWriteElement(xmlWriter, (xmlStr)commodityStandardName(stuff),
      xmlFormat<int>(commodityCount[stuff]));
  }
}

void
Construction::load(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "Construction");
  [[used_in_assert]] int depth = xmlReader.get_depth();
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    assert(xmlReader.get_depth() == depth + 1);
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }

    if(!loadMember(xmlReader, ldsv_version)) {
      unexpectedXmlElement(xmlReader);
    }

    xmlReader.next();
  }
  assert(xmlReader.get_name() == "Construction");
  assert(xmlReader.get_depth() == depth);
}

bool
Construction::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version
) {
  std::string name = xmlReader.get_name();
  Commodity stuff = commodityFromStandardName(
    xmlParse<std::string>(name).c_str());
  if(stuff != STUFF_COUNT) {
    commodityCount[stuff] = xmlParse<int>(xmlReader.read_inner_xml());
  }
  else if(name == "flags") {
    flags = xmlParse<unsigned int>(xmlReader.read_inner_xml())
      & ~VOLATILE_FLAGS;
    if(ldsv_version <= 2130) {
      flags = 0
        | (flags & 0x00000020 ? FLAG_FED : 0)
        | (flags & 0x00000040 ? FLAG_EMPLOYED : 0)
        | (flags & 0x00000080 ? FLAG_IS_TRANSPORT : 0)
        | (flags & 0x00000100 ? FLAG_NEVER_EVACUATE : 0)
        | (flags & 0x00000200 ? FLAG_EVACUATE : 0)
        | (flags & 0x01000000 ? FLAG_HAD_POWER : 0)
        | (flags & 0x08000000 ? FLAG_TRANSPARENT : 0);
    }
  }
  else return false;
  return true;
}

void Construction::place(MapPoint point) {
  unsigned short size = constructionGroup->size;
  if(!world.map.is_inside(point) || !world.map.is_inside(point.s(size).e(size)))
    throw std::runtime_error("cannot place a Construction outside the map");

  this->point = point;

  init_resources();

  for(MapPoint p = point; p.y < point.y + size; p.y++)
  for(p.x = point.x; p.x < point.x + size; p.x++) {
    MapTile& tile = *world.map(p);
    //never change water upon building something
    if(!tile.is_water()) {
      if(flags & FLAG_TRANSPARENT) {
        if(tile.group != GROUP_DESERT)
          tile.setTerrain(GROUP_BARE);
      }
      else {
        tile.setTerrain(GROUP_DESERT);
      }
    }
    assert(!tile.reportingConstruction);
    tile.reportingConstruction = this;
  }
  world.map(point)->construction = this;
  world.map.constructions.insert(this); //register for Simulation
  world.map.recentPoint = MapPoint(point);
  world.stats.groupCount[constructionGroup->group]++;

  //now look for neighbors
  neighborize();
  world.setUpdated(World::Updatable::MAP);
}

bool
Construction::can_bulldoze(Message::ptr& message) const {
  assert(world.map(point)->construction == this);
  return true;
}

void
Construction::bulldoze() {
  if(Message::ptr msg; !can_bulldoze(msg)) msg->throwEx();
  world.expense(constructionGroup->bul_cost,
    world.stats.expenses.construction);
  detach();
}

void
Construction::torch() {
  detach();
  unsigned short size = constructionGroup->size;
  for(MapPoint p(point); p.y < point.y + size; p.y++)
  for(p.x = point.x; p.x < point.x + size; p.x++) {
    fireConstructionGroup.placeItem(world, p);
  }
  world.map.connect_transport(point.x - 2, point.y - 2,
    point.x + size + 1, point.y + size + 1);
  world.map.desert_water_frontiers(point, point.s(size).e(size));
  world.setUpdated(World::Updatable::MAP);
}

//use this before deleting a construction. Construction requests check independently against NULL
void Construction::detach()
{
  //std::cout << "detaching: " << constructionGroup->name << std::endl;
  // world.map.constructions.erase(this);
  if(world.map(point)->construction == this) {
    world.map(point)->construction = NULL;
    world.map(point)->killframe(frameIt);
    world.stats.groupCount[constructionGroup->group]--;
  }
  else {
    fmt::println(stderr,
      "warning: detaching {} at {}, but it doesn't seem to be on the map.",
      constructionGroup->name, point
    );
    assert(false);
  }

  int size = constructionGroup->size;
  for(int i = 0; i < size; i++)
  for(unsigned short j = 0; j < size; j++) {
    MapTile& tile = *world.map(point.e(i).s(j));
    if(tile.reportingConstruction == this)
      tile.reportingConstruction = nullptr;
  }
  world.setUpdated(World::Updatable::MAP);
  deneighborize();

  world.map.connect_transport(point.x - 2, point.y - 2, point.x + size + 1,
    point.y + size + 1);
  world.map.desert_water_frontiers(point, point.s(size).e(size));
}

bool
Construction::isDead() const {
  return world.map(point)->construction != this;
}

void Construction::deneighborize()
{
    for(size_t i = 0; i < neighbors.size(); ++i)
    {
        std::vector<Construction*> *neib = &(neighbors[i]->neighbors);
        std::vector<Construction*>::iterator neib_it = neib->begin();
        while(neib_it != neib->end() && *neib_it != this)
            {++neib_it;}
/*#ifdef DEBUG
        assert(neib_it != neib->end());
#endif*/
        neib->erase(neib_it);
    }
    neighbors.clear();
    for(size_t i = 0; i < partners.size(); ++i)
    {
        std::vector<Construction*> *partner = &(partners[i]->partners);
        std::vector<Construction*>::iterator partner_it = partner->begin();
        while(partner_it != partner->end() && *partner_it != this)
            {++partner_it;}
/*#ifdef DEBUG
        assert(partner_it != partner->end());
#endif*/
        partner->erase(partner_it);
    }
    partners.clear();
    if (constructionGroup->group == GROUP_POWER_LINE)
    {
        world.map(point.e())->flags &= ~FLAG_POWER_CABLES_90;
        world.map(point.w())->flags &= ~FLAG_POWER_CABLES_90;
        world.map(point.s())->flags &= ~FLAG_POWER_CABLES_0;
        world.map(point.n())->flags &= ~FLAG_POWER_CABLES_0;
    }
}


void Construction::neighborize()
{
//skip ghosts (aka burning waste) and powerlines here
    if(constructionGroup->group != GROUP_FIRE
      && constructionGroup->group != GROUP_POWER_LINE
    ) {
        if(flags & FLAG_IS_TRANSPORT)//search adjacent tiles only
        {
            Construction* cst = NULL;
            Construction* cst1 = NULL;
            Construction* cst2 = NULL;
            Construction* cst3 = NULL;
            Construction* cst4 = NULL;
            unsigned short size = constructionGroup->size;
            for(unsigned short edge = 0; edge < size; ++edge) {
              //here we rely on invisible edge tiles
              cst = world.map(point.w().s(edge))->reportingConstruction;
              if(cst && cst != cst1
                && cst->constructionGroup->group != GROUP_FIRE
                && cst->constructionGroup->group != GROUP_POWER_LINE
              )
                link_to(cst1 = cst);
              cst = world.map(point.w(edge).n())->reportingConstruction;
              if(cst && cst != cst2
                && cst->constructionGroup->group != GROUP_FIRE
                && cst->constructionGroup->group != GROUP_POWER_LINE
              )
                link_to(cst2 = cst);
              cst = world.map(point.e(size).s(edge))->reportingConstruction;
              if(cst && cst != cst3
                && cst->constructionGroup->group != GROUP_FIRE
                && cst->constructionGroup->group != GROUP_POWER_LINE
              )
                link_to(cst3 = cst);
              cst = world.map(point.e(edge).s(size))->reportingConstruction;
              if(cst && cst != cst4
                && cst->constructionGroup->group != GROUP_FIRE
                && cst->constructionGroup->group != GROUP_POWER_LINE
              )
                link_to(cst4 = cst);
            }
        }
        else { // search full market range for constructions
          MapPoint nw(
            std::max(1, point.x - GROUP_MARKET_RANGE - GROUP_MARKET_SIZE + 1),
            std::max(1, point.y - GROUP_MARKET_RANGE - GROUP_MARKET_SIZE + 1)
          );
          MapPoint se(
            std::min(world.map.len()-1, point.x + GROUP_MARKET_RANGE + 1),
            std::min(world.map.len()-1, point.y + GROUP_MARKET_RANGE + 1)
          );
          for(MapPoint p = nw; p.y < se.y; p.y++)
          for(p.x = nw.x; p.x < se.x; p.x++) {
            if(p == point) continue;
            Construction *cst = world.map(p)->construction;
            if(!cst) continue;
            if(cst->constructionGroup->group == GROUP_FIRE
              || cst->constructionGroup->group == GROUP_POWER_LINE
            ) continue;

            link_to(cst);
          }
        }
    }
}

void Construction::link_to(Construction* other)
{
/*
    std::cout << "new link requested : " << constructionGroup->name << "(" << x << "," << y << ") - "
    << other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
*/
//Theses tests are only for rules of the game, simulation would run happily with duplicate/oneway links
/*#ifdef DEBUG
    std::vector<Construction*>::iterator neib_it = neighbors.begin();
    bool ignore = false;
    while(neib_it !=  neighbors.end() && *neib_it != other)
            {++neib_it;}

    if(neib_it != neighbors.end())
    {
        std::cout << "duplicate neighbor : " << constructionGroup->name << "(" << x << "," << y << ") - "
        << other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
        ignore = true;
    }
    neib_it = partners.begin();
    while(neib_it !=  partners.end() && *neib_it != other)
            {++neib_it;}

    if(neib_it != partners.end())
    {
        std::cout << "duplicate partner : " << constructionGroup->name << "(" << x << "," << y << ") - "
        << other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
        ignore = true;
    }
    if (this == other)
    {
        std::cout << "home link error : " << constructionGroup->name << "(" << x << "," << y << ") - "
        << other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
        ignore = true;
    }
    if(ignore)
    {   return;}
#endif*/
    bool useful = false;
    Commodity stuff_ID;
    for(stuff_ID = STUFF_INIT ; !useful && stuff_ID < STUFF_COUNT ; stuff_ID++ )
    {
        CommodityRule& rule = constructionGroup->commodityRuleCount[stuff_ID];
        CommodityRule& other_rule =
          other->constructionGroup->commodityRuleCount[stuff_ID];
        if(!rule.maxload || !other_rule.maxload) continue;

        useful = (rule.take && other_rule.give) ||
          (rule.give && other_rule.take);
    }
    if (useful)
    {
        if(constructionGroup->group == GROUP_POWER_LINE)
        {
            neighbors.push_back(other);
            other->neighbors.push_back(this);
            //std::cout << "power link : " << constructionGroup->name << "(" << x << "," << y << ") - "
            //<< other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
            return;
        }
        int vec_x = other->point.x - point.x;
        int vec_y = other->point.y - point.y;
        int ns = other->constructionGroup->size;
        int s = constructionGroup->size;
        //Check if *this is adjacent to *other
        if(!(((vec_x == s) || (vec_x == -ns)) && ((vec_y == s) || (vec_y == -ns))) &&
            (((vec_x <= s) && (vec_x >= -ns)) && ((vec_y <= s) && (vec_y >= -ns))))
        {
            neighbors.push_back(other);
            other->neighbors.push_back(this);
            //std::cout << "neighbor : " << constructionGroup->name << "(" << x << "," << y << ") - "
            //<< other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
        }
        //transport may never be a distant partner
        else if(! ((flags | other->flags) & FLAG_IS_TRANSPORT))
        {
            if ((other->constructionGroup->group == GROUP_MARKET)^(constructionGroup->group == GROUP_MARKET))
            {
                partners.push_back(other);
                other->partners.push_back(this);
                //std::cout << "partner : " << constructionGroup->name << "(" << x << "," << y << ") - "
                //<< other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
            }
            //else
            //std::cout << "rejected connection : " << constructionGroup->name << "(" << x << "," << y << ") - "
            //<< other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
        }
        //else
        //std::cout << "to far transport : " << constructionGroup->name << "(" << x << "," << y << ") - "
        //<< other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
    }
    //else
    //std::cout << "useless connection : " << constructionGroup->name << "(" << x << "," << y << ") - "
    //<< other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
}

int Construction::countPowercables(int mask)
{
    //must match mask definition from connect_transport() in transport.cpp
    //y-1    //mask & 8
    //x-1    //mask & 4
    //x+size //mask & 2
    //y+size //mask & 1
    unsigned short size = constructionGroup->size;
    int count = 0;

    for(unsigned short i = 0; i < size; ++i)
    {
        if( (mask & 8) &&
            world.map(point.n().e(i))->flags & FLAG_POWER_CABLES_0 )
        {++count;}

        if( (mask & 4) &&
            world.map(point.s(i).w())->flags & FLAG_POWER_CABLES_90 )
        {   ++count;}

        if( (mask & 2)
            && world.map(point.s(i).e(size))->flags & FLAG_POWER_CABLES_90 )
        {   ++count;}

        if( (mask & 1) &&
            world.map(point.s(size).e(i))->flags & FLAG_POWER_CABLES_0 )
        {   ++count;}

    } //end for size
/*    //TODO needs changes in GameView in order to be actually drawn
    if (constructionGroup == &substationConstructionGroup )
    {
        //here size = 2
        world.map(x+1,y)->flags &= (world.map(x + 2, y)->flags & FLAG_POWER_CABLES_90);
        world.map(x,y+1)->flags &= (world.map(x, y + 2)->flags & FLAG_POWER_CABLES_0);
    }
*/
    return count;
}




int Construction::tellstuff(Commodity stuff_ID, int center_ratio) //called by Minimap and connecttransport
{
    CommodityRule& rule = constructionGroup->commodityRuleCount[stuff_ID];
    if (rule.maxload)
    {
        int loc_lvl = commodityCount[stuff_ID];
        int loc_cap = rule.maxload;
        if ((flags & FLAG_EVACUATE) && (center_ratio != -2))
        {   return (loc_lvl>1)?loc_lvl:-1;}

/*#ifdef DEBUG
        if (loc_lvl > loc_cap)
        {
            std::cout<<"fixed "<<commodityNames[stuff_ID]<<" > maxload at "<<constructionGroup->name<<" x,y = "<<x<<","<<y<<std::endl;
            commodityCount[stuff_ID] = loc_cap;
            loc_lvl = loc_cap;
        }
        if (loc_lvl < 0)
        {
            std::cout<<"fixed "<<commodityNames[stuff_ID]<<" < 0 at "<<constructionGroup->name<<" x,y = "<<x<<","<<y<<std::endl;
            commodityCount[stuff_ID] = loc_cap;
            loc_lvl = 0;
        }

        if (loc_cap < 1)
        {
            std::cout<<"maxload "<<commodityNames[stuff_ID]<<" <= 0 error at "<<constructionGroup->name<<" x,y = "<<x<<","<<y<<std::endl;
        }
#endif*/
        int loc_ratio = loc_lvl * TRANSPORT_QUANTA / (loc_cap);
        //Tell actual stock if we would tentatively participate in transport
        if ((center_ratio < 0) || (
        loc_ratio>center_ratio?rule.give:rule.take) )
        {   return (loc_ratio);}
    }
    return -1;
}

void Construction::trade()
{
    int ratio, cap, lvl, center_lvl, center_cap;
    int traffic, max_traffic;
    Commodity stuff_ID;
    const size_t neighsize = neighbors.size();
    std::vector<bool> lvls(neighsize);
    Transport *transport = NULL;
    Powerline *powerline = NULL;
    if(flags & FLAG_IS_TRANSPORT)
    {   transport = dynamic_cast<Transport*>(this);}
    else if(constructionGroup->group == GROUP_POWER_LINE)
    {   powerline = dynamic_cast<Powerline*>(this);}
    /*begin for over all different stuff*/
    for(stuff_ID = STUFF_INIT ; stuff_ID < STUFF_COUNT ; stuff_ID++ )
    {
        const CommodityRule& center_rule = constructionGroup->commodityRuleCount[stuff_ID];
        if(!center_rule.maxload) continue;
        center_lvl = commodityCount[stuff_ID];
        center_cap = center_rule.maxload;
        if(flags & FLAG_EVACUATE) {
            if(!center_lvl) continue;
            center_cap = 0;
        }
        //first order approximation for ratio
        // ratio = (center_lvl * TRANSPORT_QUANTA / (center_cap) );
        lvl = center_lvl;
        cap = center_cap;
        for(unsigned int i = 0; i < lvls.size(); ++i)
        {
            Construction *pear = neighbors[i];
            CommodityRule& pearrule = pear->constructionGroup->commodityRuleCount[stuff_ID];
            if(!pearrule.maxload) {
                lvls[i] = false;
                continue;
            }
            lvls[i] = true;
            int lvlsi = pear->commodityCount[stuff_ID];
            int capsi = pearrule.maxload;
            // int pearat = lvlsi * TRANSPORT_QUANTA / capsi;
            //only consider stuff that would tentatively move
            if(pear->flags & FLAG_EVACUATE) {
                lvls[i] = true;
                capsi = 0;
            }
            else if(!(((long)lvlsi * center_cap > (long)center_lvl * capsi) ?
              (center_rule.take && pearrule.give) :
              (center_rule.give && pearrule.take)))
            {   continue;}
            lvl += lvlsi;
            cap += capsi;
        }
        if(!cap) continue; // cannot evacuate
        ratio = lvl * TRANSPORT_QUANTA / cap;
        max_traffic = 0;
        int old_center = center_lvl;
        //make flow towards ratio
        for(unsigned int i = 0; i < lvls.size(); ++i)
        {
            if(lvls[i])
            {
                traffic = neighbors[i]->equilibrate_stuff(&center_lvl, center_rule, ratio, stuff_ID);
                if( traffic > max_traffic )
                {   max_traffic = traffic;}
            }
        }
        int flow = center_lvl - old_center;
        max_traffic = max_traffic * TRANSPORT_QUANTA / center_rule.maxload;
        //do some smoothing to suppress fluctuations from random order
        // max possible 92.8%
        if(transport) //Special for transport
        {
            transport->trafficCount[stuff_ID] = (9 * transport->trafficCount[stuff_ID] + max_traffic) / 10;
            if(simDelay != SIM_DELAY_FAST
            && getConfig()->carsEnabled.get()
            && 100 * max_traffic *  TRANSPORT_RATE / TRANSPORT_QUANTA > 2
            && world.map(point)->getTransportGroup() == GROUP_ROAD)
            {
                int yield = 50 * max_traffic *  TRANSPORT_RATE / TRANSPORT_QUANTA;
                if(simDelay == SIM_DELAY_MED) // compensate for overall animation
                {   yield = (yield+1)/2;}
                switch (stuff_ID)
                {
                    case STUFF_LABOR :
                        if((rand()%COMMUTER_TRAFFIC_RATE) < (yield+1)/2
                          && transport->canPlaceVehicle()
                        ) {
                          world.vehicleList.push_back(
                            new Vehicle(world, point,
                              VEHICLE_BLUECAR, (flow > 0)
                                ? VEHICLE_STRATEGY_MAXIMIZE
                                : VEHICLE_STRATEGY_MINIMIZE
                            )
                          );
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        else if(powerline) //Special for powerlines
        {
            powerline->trafficCount[stuff_ID] = (9 * powerline->trafficCount[stuff_ID] + max_traffic) / 10;
            for(unsigned int i = 0; i < neighsize; ++i)
            {
                if((powerline->anim_counter == 0)
                && !(neighbors[i]->constructionGroup->group == GROUP_POWER_LINE)
                && neighbors[i]->constructionGroup->commodityRuleCount[stuff_ID].give
                && (neighbors[i]->commodityCount[stuff_ID] > 0))
                {   powerline->anim_counter = POWER_MODULUS + rand()%POWER_MODULUS;}
                if(powerline->flashing &&
                  neighbors[i]->constructionGroup->group == GROUP_POWER_LINE
                ) {
                  int& anim_counter =
                    dynamic_cast<Powerline*>(neighbors[i])->anim_counter;
                  if(anim_counter <= POWER_MODULUS / 3)
                    anim_counter = POWER_MODULUS;
                }
            }
        }

        commodityCount[stuff_ID] += flow; //update center_lvl
    } //endfor all different STUFF
}

int Construction::equilibrate_stuff(int *rem_lvl, CommodityRule rem_rule, int ratio, Commodity stuff_ID)
{
    // if ( !commodityCount.count(stuff_ID) ) // we know stuff_id
    //     return -1;
    // valid commodity for this construction is precondition for this method

    int flow, traffic;
    int& rem_cap = rem_rule.maxload;
    int *loc_lvl;
    int loc_cap;
    CommodityRule& loc_rule = constructionGroup->commodityRuleCount[stuff_ID];
    loc_lvl = &(commodityCount[stuff_ID]);
    loc_cap = loc_rule.maxload;
    if (!(flags & FLAG_EVACUATE))
    {
        flow = (ratio * (loc_cap) / TRANSPORT_QUANTA) - (*loc_lvl);
        if (flow > 0 ?
          !(loc_rule.take && rem_rule.give) :
          !(loc_rule.give && rem_rule.take))
        {   //construction refuses the flow
            return 0;
        }
        if (flow > 0)
        {
            if (flow * TRANSPORT_RATE > rem_cap )
            {   flow = rem_cap / TRANSPORT_RATE;}
            if (flow > *rem_lvl)
            {   flow = *rem_lvl;}
        }
        else if (flow < 0)
        {
            if(-flow * TRANSPORT_RATE > rem_cap)
            {   flow = - rem_cap / TRANSPORT_RATE;}
            if(-flow > (rem_cap-*rem_lvl)) {
                flow = -(rem_cap-*rem_lvl);
                if(flow > 0) flow = 0; // the other construction is evacuating
            }
        }
        //std::cout.flush();
        if (!(flags & FLAG_IS_TRANSPORT) && (flow > 0)
            && (constructionGroup->group != GROUP_MARKET))
        //something is given to a consumer
        {
            switch (stuff_ID)
            {
                case (STUFF_LABOR) :
                    world.taxable.labor += flow;
                    break;
                case (STUFF_GOODS) :
                    world.taxable.goods += flow;
                    break;
                case (STUFF_COAL) :
                    world.taxable.coal += flow;
                    break;
                default:
                    break;
            }
        }
    }
    else // we are evacuating
    {
        flow = -(rem_cap-*rem_lvl);
        if (-flow > *loc_lvl)
        {   flow = -*loc_lvl;}
    }
    // traffic = flow * TRANSPORT_QUANTA / rem_cap;
    traffic = flow;
    // incomming and outgoing traffic don't cancel but add up
    if (traffic < 0)
    {
        traffic = -traffic;
    }
    *loc_lvl += flow;
    *rem_lvl -= flow;
    return traffic;
}

void Construction::playSound()
{
    int s = soundGroup->chunks.size();
    if(soundGroup->sounds_loaded && s)
    {   getSound()->playASound( soundGroup->chunks[ rand()%s ] );}
}



//ConstructionGroup Declarations

ConstructionGroup::ConstructionGroup(
  const std::string& name,
  const std::string& name_plural,
  bool no_credit,
  unsigned short group,
  unsigned short size, int colour,
  int cost_mul, int bul_cost, int fire_chance,
  int cost, int tech, int range
) {
  this->name = name;
  this->name_plural = name_plural;
  this->no_credit = no_credit;
  this->group = group;
  this->size = size;
  this->colour = colour;
  this->cost_mul = cost_mul;
  this->bul_cost = bul_cost;
  this->fire_chance = fire_chance;
  this->cost = cost;
  this->tech = tech;
  this->range = range;

  for(Commodity stuff = STUFF_INIT; stuff < STUFF_COUNT; stuff++) {
    this->commodityRuleCount[stuff] = (CommodityRule){
      .maxload = 0,
      .take = false,
      .give = false
    };
  }
}

int ConstructionGroup::getCosts(const World& world) const {
  return static_cast<int>(cost *
    (1.0f + cost_mul * world.tech_level / static_cast<float>(MAX_TECH_LEVEL)));
}

void
ConstructionGroup::placeItem(World& world, MapPoint point) {
  for(unsigned short i = 0; i < size; i++)
  for(unsigned short j = 0; j < size; j++) {
    MapTile& tile = *world.map(point.s(i).e(j));
    Construction *cst = tile.reportingConstruction;
    if(cst) {
      throw std::logic_error("space occupied");
    }
    if(tile.is_water() && !dynamic_cast<TransportConstructionGroup *>(this)) {
      throw std::logic_error("water here");
    }
  }

  Construction *tmpConstr = createConstruction(world);
#ifdef DEBUG
  if(tmpConstr == NULL) {
    std::cout << "failed to create " << name
      << " at " << point
      << std::endl;
  }
#endif
  tmpConstr->place(point);
}

std::string
ConstructionGroup::getName(void) const {
  return _(name.c_str());
}

bool
ConstructionGroup::can_build(const World& world, Message::ptr& message) const {
  if(world.tech_level < tech) {
    message = NotEnoughTechMessage::create(world.tech_level, tech);
    return false;
  }

  if(no_credit && world.total_money < getCosts(world)) {
    message = OutOfMoneyMessage::create(false);
    return false;
  }

  return true;
}

bool
ConstructionGroup::can_build_here(const World& world, const MapPoint point,
  Message::ptr& message
) const {
  assert(can_build(world, message)); // already checked in World::place_item

  if(!world.map.is_visible(point) ||
    !world.map.is_visible(point.s(size-1).e(size-1))
  ) {
    message = OutsideMapMessage::create(point);
    return false;
  }

  //handle transport quickly
  if(world.map.is_visible(point) && (
    group == GROUP_TRACK ||
    group == GROUP_ROAD ||
    group == GROUP_RAIL)
  ) {
    bool open = world.map(point)->is_bare() ||
      world.map(point)->getGroup() == GROUP_POWER_LINE ||
      world.map(point)->is_water() && !world.map(point)->is_transport() ||
      world.map(point)->is_transport() &&
        world.map(point)->getTransportGroup() != group;

    if(!open)
      message = SpaceOccupiedMessage::create(point);
    return open;
  }

  //At last check for bare building site
  for(int j = 0; j<size; j++) {
    for(int i = 0; i<size; i++) {
      if(!world.map(point.e(i).s(j))->is_bare()) {
        message = SpaceOccupiedMessage::create(point);
        return false;
      }
    }
  }
  return true;
}



void ConstructionGroup::printGroups()
{
    std::map<unsigned short, ConstructionGroup *>::iterator iterator;
    for (iterator = groupMap.begin(); iterator != groupMap.end(); iterator++)
    {
        std::cout << "group #" << iterator->first << ": " << iterator->second->name << std::endl;
    }
}

std::map<unsigned short, ConstructionGroup *> ConstructionGroup::groupMap;
std::map<std::string, ConstructionGroup *> ConstructionGroup::resourceMap;

/** @file lincity/lintypes.cpp */
