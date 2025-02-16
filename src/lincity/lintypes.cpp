/* ---------------------------------------------------------------------- *
 * src/lincity/lintypes.cpp
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

#include "lintypes.h"

#include <assert.h>                       // for assert
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteFormatEle...
#include <stdlib.h>                       // for rand
#include <iostream>                       // for basic_ostream, operator<<
#include <stdexcept>                      // for runtime_error
#include <utility>                        // for pair
#include <vector>                         // for vector

#include "ConstructionCount.h"            // for ConstructionCount
#include "ConstructionManager.h"          // for ConstructionManager
#include "ConstructionRequest.h"          // for ConstructionDeletionRequest
#include "Vehicles.h"                     // for Vehicle, VehicleStrategy
#include "commodities.hpp"                // for CommodityRule, Commodity
#include "engglobs.h"                     // for world, total_money, constru...
#include "groups.h"                       // for GROUP_POWER_LINE, GROUP_FIRE
#include "gui_interface/mps.h"            // for mps_store_ssddp, mps_store_...
#include "lctypes.h"                      // for CST_BLACKSMITH_0, CST_BLACK...
#include "lin-city.h"                     // for BAD, FLAG_EVACUATE, FLAG_IS...
#include "lincity-ng/Config.hpp"          // for getConfig, Config
#include "lincity-ng/Sound.hpp"           // for getSound, Sound
#include "modules/all_modules.h"          // for Powerline, GROUP_MARKET_RANGE
#include "stats.h"                        // for coal_tax, goods_tax, income...
#include "tinygettext/gettext.hpp"        // for _
#include "transport.h"                    // for TRANSPORT_QUANTA, TRANSPORT...
#include "world.h"                        // for Map, MapTile
#include "xmlloadsave.h"                  // for xmlStr

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

void Construction::list_commodities(int *i) {
    switch(mps_map_page) {
    default:
    case 0:
        mps_store_title((*i)++, _("Inventory:"));
        list_inventory(i);
        break;
    case 1:
        mps_store_title((*i)++, _("Production:"));
        list_production(i);
        break;
    }
}

void Construction::list_inventory(int * i)
{
    /*
        Lists all current commodities of a construction in MPS area
        Groups commodities by incomming, outgoing, twoway and inactive
    */

    Commodity stuff;
    if (! (flags & FLAG_EVACUATE))
    {
        for(stuff = STUFF_INIT ; stuff < STUFF_COUNT && *i < 14; stuff++)
        {
            const CommodityRule& rule = constructionGroup->commodityRuleCount[stuff];
            if(rule.maxload && rule.take && !rule.give)
            {
                mps_store_ssddp(*i,"--> ", commodityNames[stuff], commodityCount[stuff], rule.maxload);
                ++*i;
            }//endif
        } //endfor
        for(stuff = STUFF_INIT ; stuff < STUFF_COUNT && *i < 14; stuff++)
        {
            const CommodityRule& rule = constructionGroup->commodityRuleCount[stuff];
            if(rule.maxload && !rule.take && rule.give)
            {
                mps_store_ssddp(*i,"<-- ", commodityNames[stuff], commodityCount[stuff], rule.maxload);
                ++*i;
            }//endif
        } //endfor
        for(stuff = STUFF_INIT ; stuff < STUFF_COUNT && *i < 14; stuff++)
        {
            const CommodityRule& rule = constructionGroup->commodityRuleCount[stuff];
            if(rule.maxload && rule.take && rule.give)
            {
                mps_store_ssddp(*i,"<->", commodityNames[stuff], commodityCount[stuff], rule.maxload);
                ++*i;
            }//endif
        } //endfor
        for(stuff = STUFF_INIT ; stuff < STUFF_COUNT && *i < 14; stuff++)
        {
            const CommodityRule& rule = constructionGroup->commodityRuleCount[stuff];
            if(rule.maxload && !rule.take && !rule.give)
            {
                mps_store_ssddp(*i,"--- ", commodityNames[stuff], commodityCount[stuff], rule.maxload);
                ++*i;
            }//endif
        } //endfor
    }
    else // FLAG_EVACUATE
    {
        for(stuff = STUFF_INIT ; stuff < STUFF_COUNT && *i < 14; stuff++)
        {
            const CommodityRule& rule = constructionGroup->commodityRuleCount[stuff];
            if(rule.maxload) {
                mps_store_ssddp(*i,"<< ",commodityNames[stuff], commodityCount[stuff], rule.maxload);
                ++*i;
            }
        }//endfor
    }
}

void Construction::list_production(int * i)
{
    /*
        Lists all current commodities of a construction in MPS area
        Groups commodities by incomming, outgoing, twoway and inactive
    */

    Commodity stuff;
    for(stuff = STUFF_INIT ; stuff < STUFF_COUNT && *i < 14; stuff++)
    {
        const CommodityRule& rule = constructionGroup->commodityRuleCount[stuff];
        const int& maxprod = commodityMaxProd[stuff];
        const int& maxcons = commodityMaxCons[stuff];
        if(rule.maxload && maxcons && !maxprod)
        {
            mps_store_ssddp(*i,"--> ", commodityNames[stuff], commodityProdPrev[stuff], -maxcons);
            ++*i;
        }//endif
    } //endfor
    for(stuff = STUFF_INIT ; stuff < STUFF_COUNT && *i < 14; stuff++)
    {
        const CommodityRule& rule = constructionGroup->commodityRuleCount[stuff];
        const int& maxprod = commodityMaxProd[stuff];
        const int& maxcons = commodityMaxCons[stuff];
        if(rule.maxload && !maxcons && maxprod)
        {
            mps_store_ssddp(*i,"<-- ", commodityNames[stuff], commodityProdPrev[stuff], maxprod);
            ++*i;
        }//endif
    } //endfor
    for(stuff = STUFF_INIT ; stuff < STUFF_COUNT && *i < 14; stuff++)
    {
        const CommodityRule& rule = constructionGroup->commodityRuleCount[stuff];
        const int& maxprod = commodityMaxProd[stuff];
        const int& maxcons = commodityMaxCons[stuff];
        if(rule.maxload && maxcons && maxprod)
        {
            int amt = commodityProdPrev[stuff];
            int max = amt >= 0 ? maxprod : -maxcons;
            mps_store_ssddp(*i,"<->", commodityNames[stuff], amt, max);
            ++*i;
        }//endif
    } //endfor
    for(stuff = STUFF_INIT ; stuff < STUFF_COUNT && *i < 14; stuff++)
    {
        const CommodityRule& rule = constructionGroup->commodityRuleCount[stuff];
        const int& maxprod = commodityMaxProd[stuff];
        const int& maxcons = commodityMaxCons[stuff];
        if(rule.maxload && !maxcons && !maxprod)
        {
            mps_store_ssddp(*i,"--- ", commodityNames[stuff], commodityProdPrev[stuff], 1);
            ++*i;
        }//endif
    } //endfor
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
    frameIt = world.map(x,y)->createframe();
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
  xmlTextWriterWriteFormatElement(xmlWriter,
    (xmlStr)"flags", "0x%x", flags & ~VOLATILE_FLAGS);
  for(Commodity stuff = STUFF_INIT; stuff < STUFF_COUNT; stuff++) {
    if(!constructionGroup->commodityRuleCount[stuff].maxload) continue;
    xmlStr name = (xmlStr)commodityStandardName(stuff);
    xmlTextWriterWriteFormatElement(xmlWriter,
      name, "%d", commodityCount[stuff]);
  }
}

void Construction::load(xmlpp::TextReader& xmlReader) {
  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "Construction");
  int depth = xmlReader.get_depth();
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    assert(xmlReader.get_depth() == depth + 1);
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }

    if(!loadMember(xmlReader)) {
      unexpectedXmlElement(xmlReader);
    }

    xmlReader.next();
  }
  assert(xmlReader.get_name() == "Construction");
  assert(xmlReader.get_depth() == depth);
}

bool Construction::loadMember(xmlpp::TextReader& xmlReader) {
  std::string name = xmlReader.get_name();
  Commodity stuff = commodityFromStandardName(name.c_str());
  if(stuff != STUFF_COUNT) {
    commodityCount[stuff] = std::stoi(xmlReader.read_inner_xml());
  }
  else if(name == "flags")
    flags = std::stoul(xmlReader.read_inner_xml(), NULL, 0) & ~VOLATILE_FLAGS;
  else return false;
  return true;
}

void Construction::place(int x, int y) {
  unsigned short size = constructionGroup->size;
  if(!world.map.is_inside(x, y) || !world.map.is_inside(x + size, y + size))
    throw std::runtime_error("cannot place a Construction outside the map");

  this->x = x;
  this->y = y;

  init_resources();

  for (unsigned short i = 0; i < size; i++) {
    for (unsigned short j = 0; j < size; j++) {
      //never change water upon building something
      if(!world.map(x + j, y + i)->is_water()) {
        if(!(flags & FLAG_TRANSPARENT)) {
          world.map(x + j, y + i)->setTerrain(GROUP_DESERT);
          world.map(x + j, y + i)->flags |= FLAG_INVISIBLE; // hide maptiles
        }
        else {
          world.map(x + j, y + i)->flags &= ~FLAG_INVISIBLE; // show maptiles
          if(world.map(x + j, y + i)->group != GROUP_DESERT)
            world.map(x + j, y + i)->setTerrain(GROUP_BARE);
        }
      }
      assert(!world.map(x+j, y+i)->reportingConstruction);
      world.map(x + j, y + i)->reportingConstruction = this;
    } //endfor j
  }// endfor i
  world.map(x, y)->construction = this;
  world.map.constructionCount.add_construction(this); //register for Simulation
  constructionGroup->count++;

  //now look for neighbors
  neighborize();
}

//use this before deleting a construction. Construction requests check independently against NULL
void Construction::detach()
{
    //std::cout << "detaching: " << constructionGroup->name << std::endl;
    world.map.constructionCount.remove_construction(this);
    if(world.map(x,y)->construction == this) {
        world.map(x,y)->construction = NULL;
        world.map(x,y)->killframe(frameIt);
        constructionGroup->count--;
/*
        world.map(x,y)->framesptr->erase(frameIt);
        if(world.map(x,y)->framesptr->empty())
        {
            delete world.map(x,y)->framesptr;
            world.map(x,y)->framesptr = NULL;
        }
*/
    }

    for (unsigned short i = 0; i < constructionGroup->size; ++i)
    {
        for (unsigned short j = 0; j < constructionGroup->size; ++j)
        {
            world.map(x + j, y + i)->flags &= (~FLAG_INVISIBLE);
            // constructions may have children e.g. waste burning markets/shanties
            Construction* child = world.map(x+j,y+i)->construction;
            if(child)
            {
                //std::cout << "killing child: " << world.map(x+j,y+i)->construction->constructionGroup->name << std::endl;
                world.map.constructionCount.remove_construction(child);
                world.map(x+j,y+i)->killframe(child->frameIt);
                delete child;
                child = NULL;
                world.map(x+j,y+i)->construction = NULL;
            }
            world.map(x+j,y+i)->reportingConstruction = NULL;
        }
    }
    deneighborize();
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
        world.map(x + 1, y)->flags &= ~FLAG_POWER_CABLES_90;
        world.map(x - 1, y)->flags &= ~FLAG_POWER_CABLES_90;
        world.map(x, y + 1)->flags &= ~FLAG_POWER_CABLES_0;
        world.map(x, y - 1)->flags &= ~FLAG_POWER_CABLES_0;
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
              cst = world.map(x - 1,y + edge)->reportingConstruction;
              if(cst && cst != cst1
                && cst->constructionGroup->group != GROUP_FIRE
                && cst->constructionGroup->group != GROUP_POWER_LINE
              )
                link_to(cst1 = cst);
              cst = world.map(x + edge,y - 1)->reportingConstruction;
              if(cst && cst != cst2
                && cst->constructionGroup->group != GROUP_FIRE
                && cst->constructionGroup->group != GROUP_POWER_LINE
              )
                link_to(cst2 = cst);
              cst = world.map(x + size,y + edge)->reportingConstruction;
              if(cst && cst != cst3
                && cst->constructionGroup->group != GROUP_FIRE
                && cst->constructionGroup->group != GROUP_POWER_LINE
              )
                link_to(cst3 = cst);
              cst = world.map(x + edge,y + size)->reportingConstruction;
              if(cst && cst != cst4
                && cst->constructionGroup->group != GROUP_FIRE
                && cst->constructionGroup->group != GROUP_POWER_LINE
              )
                link_to(cst4 = cst);
            }
        }
        else // search full market range for constructions
        {
            int tmp;
            int lenm1 = world.map.len()-1;
            tmp = x - GROUP_MARKET_RANGE - GROUP_MARKET_SIZE + 1;
            int xs = (tmp < 1) ? 1 : tmp;
            tmp = y - GROUP_MARKET_RANGE - GROUP_MARKET_SIZE + 1;
            int ys = (tmp < 1)? 1 : tmp;
            tmp = x + GROUP_MARKET_RANGE + 1;
            int xe = (tmp > lenm1) ? lenm1 : tmp;
            tmp = y + GROUP_MARKET_RANGE + 1;
            int ye = (tmp > lenm1)? lenm1 : tmp;

            for(int yy = ys; yy < ye; ++yy)
            {
                for(int xx = xs; xx < xe; ++xx)
                {
                    //don't search at home
                    if(((xx == x )  && (yy == y)))
                    {   continue;}
                    if(world.map(xx,yy)->construction) //be unique
                    {
                        Construction *cst = world.map(xx,yy)->reportingConstruction; //stick with reporting
                        if((cst->constructionGroup->group == GROUP_FIRE)
                        || (cst->constructionGroup->group == GROUP_POWER_LINE)  )
                        {   continue;}
                        //will attempt to make a link
                        link_to(cst);
                    }
                }

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
        int vec_x = other->x - x;
        int vec_y = other->y - y;
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
            world.map(x + i, y - 1)->flags & FLAG_POWER_CABLES_0 )
        {++count;}

        if( (mask & 4) &&
            world.map(x - 1, y + i)->flags & FLAG_POWER_CABLES_90 )
        {   ++count;}

        if( (mask & 2)
            && world.map(x + size, y + i)->flags & FLAG_POWER_CABLES_90 )
        {   ++count;}

        if( (mask & 1) &&
            world.map(x + i, y + size)->flags & FLAG_POWER_CABLES_0 )
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
            && getConfig()->carsEnabled
            && 100 * max_traffic *  TRANSPORT_RATE / TRANSPORT_QUANTA > 2
            && world.map(x,y)->getTransportGroup() == GROUP_ROAD)
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
                            new Vehicle(world, x, y, VEHICLE_BLUECAR, (flow > 0)
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
                if((powerline->flashing && (neighbors[i]->constructionGroup->group == GROUP_POWER_LINE)))
                {   ConstructionManager::submitRequest(new PowerLineFlashRequest(neighbors[i]));}
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
                    world.stats.taxable.labor += flow;
                    break;
                case (STUFF_GOODS) :
                    world.stats.taxable.goods += flow;
                    break;
                case (STUFF_COAL) :
                    world.stats.taxable.coal += flow;
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

int ConstructionGroup::getCosts(World& world) {
  return static_cast<int>(cost *
    (1.0f + cost_mul * world.tech_level / static_cast<float>(MAX_TECH_LEVEL)));
}

int ConstructionGroup::placeItem(World& world, int x, int y)
{

    //std::cout << "building: " << tmpConstr->constructionGroup->name  << "(" << x << "," << y << ")" << std::endl;
    //enforce empty site
    //unsigned short size = tmpConstr->constructionGroup->size;
    for (unsigned short i = 0; i < size; i++)
    {
        for (unsigned short j = 0; j < size; j++)
        {
            if(world.map(x+j, y+i)->reportingConstruction) {
              ConstructionManager::executeRequest(
                new ConstructionDeletionRequest(
                  world.map(x+j, y+i)->reportingConstruction));
            }
        }
    }


    Construction *tmpConstr = createConstruction(world);
#ifdef DEBUG
    if (tmpConstr == NULL)
    {
        std::cout << "failed to create " << name << " at " << "(" << x << ", " << y << ")" << std::endl;
        return -1;
    }
#endif

    tmpConstr->place(x, y);

    return 0;
}

std::string ConstructionGroup::getName(void){
    return _(name.c_str());
}

extern void ok_dial_box(const char *, int, const char *);

bool ConstructionGroup::is_allowed_here(World& world, int x, int y, bool msg) {
    //handle transport quickly
    if(world.map.is_visible(x, y) && (
      group == GROUP_TRACK ||
      group == GROUP_ROAD ||
      group == GROUP_RAIL)
    )
      return world.map(x,y)->is_bare() ||
        world.map(x,y)->getGroup() == GROUP_POWER_LINE ||
        world.map(x,y)->is_water() && !world.map(x,y)->is_transport() ||
        world.map(x,y)->is_transport() &&
          world.map(x,y)->getTransportGroup() != group;

    //now check for special rules
    switch (group) {
    case GROUP_UNIVERSITY:
        if(schoolConstructionGroup.count/4
          - universityConstructionGroup.count < 1\
        ) {
            if (msg)
                ok_dial_box("warning.mes", BAD, _("Not enough students, build more schools."));
            return false;
        }
        break;

    //The Harbour needs a River on the East side.
    //and relies on invisible tiles if attempted at edge
    case GROUP_PORT:
        for(int j = 0; j < size; j++ )
        {
            if (!( world.map(x + size, y + j)->flags & FLAG_IS_RIVER ) )
            {
                if (msg)
                    ok_dial_box("warning.mes", BAD, _("Port must be connected to river all along right side."));
                return false;
            }
        }
        break;

    //Waterwell needs ... water :-)
    case GROUP_WATERWELL:
        {
            bool has_ugw = false;
            for (int i = 0; i < size; i++)
                for (int j = 0; j < size; j++)
                    has_ugw = has_ugw | (world.map(x + j,y + i)->flags & FLAG_HAS_UNDERGROUND_WATER);
            if (!has_ugw)
            {
                if (msg)
                    ok_dial_box("warning.mes", BAD, _("You can't build a water well here: it is all desert."));
                return false;
            }
        }
        break;
    //Oremine
    /* GCS: mines over old mines is OK if there is enough remaining
     *  ore, as is the case when there is partial overlap. */
    case GROUP_OREMINE:
        {
            int total_ore = 0;
            for (int i = 0; i < size; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    total_ore += world.map(x+j, y+i)->ore_reserve;
                }
            }
            if (total_ore < MIN_ORE_RESERVE_FOR_MINE) {
                if (msg) ok_dial_box("warning.mes", BAD, _("You can't build a mine here: there is no ore left at this site"));
                return false; // not enought ore
            }
        }
    break;

    //Parkland
    case GROUP_PARKLAND:
        if (!(world.map(x, y)->flags & FLAG_HAS_UNDERGROUND_WATER))
        {
            if (msg)
                ok_dial_box("warning.mes", BAD, _("You can't build a park here: it is a desert, parks need water"));
            return false;
        }
        break;
    //Other cases
    }

    if(no_credit && world.total_money < getCosts(world)) {
      if(msg) {
        world.addEvent(new CannotBuildOnCreditMessage(world));
      }
      return false;
    }

    //At last check for bare building site
    for(int j = 0; j<size; j++)
    {
        for(int i = 0; i<size; i++)
        {
            if(!world.map(x+i, y+j)->is_bare())
            {   return false;}
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

//Legacy Stuff

unsigned short get_group_of_type(unsigned short type) {
  switch(type) {
    case CST_NONE:
    case CST_GREEN:
    default:
      return GROUP_BARE;

    case CST_FIRE_1:
    case CST_FIRE_2:
    case CST_FIRE_3:
    case CST_FIRE_4:
    case CST_FIRE_5:
      return GROUP_FIRE;

    case CST_FIRE_DONE1:
    case CST_FIRE_DONE2:
    case CST_FIRE_DONE3:
    case CST_FIRE_DONE4:
      return GROUP_FIRE;

    case CST_BURNT:
      return GROUP_BURNT;

    case CST_PARKLAND_PLANE:
    case CST_PARKLAND_LAKE:
      return GROUP_PARKLAND;

    case CST_POWERL_H_L:
    case CST_POWERL_V_L:
    case CST_POWERL_LD_L:
    case CST_POWERL_RD_L:
    case CST_POWERL_LU_L:
    case CST_POWERL_RU_L:
    case CST_POWERL_LDU_L:
    case CST_POWERL_LDR_L:
    case CST_POWERL_LUR_L:
    case CST_POWERL_UDR_L:
    case CST_POWERL_LUDR_L:
    case CST_POWERL_H_D:
    case CST_POWERL_V_D:
    case CST_POWERL_LD_D:
    case CST_POWERL_RD_D:
    case CST_POWERL_LU_D:
    case CST_POWERL_RU_D:
    case CST_POWERL_LDU_D:
    case CST_POWERL_LDR_D:
    case CST_POWERL_LUR_D:
    case CST_POWERL_UDR_D:
    case CST_POWERL_LUDR_D:
      return GROUP_POWER_LINE;

    case CST_RAIL_LR:
    case CST_RAIL_LU:
    case CST_RAIL_LD:
    case CST_RAIL_UD:
    case CST_RAIL_UR:
    case CST_RAIL_DR:
    case CST_RAIL_LUR:
    case CST_RAIL_LDR:
    case CST_RAIL_LUD:
    case CST_RAIL_UDR:
    case CST_RAIL_LUDR:
      return GROUP_RAIL;

    case CST_RAIL_BRIDGE_LR:
    case CST_RAIL_BRIDGE_UD:
    case CST_RAIL_BRIDGE_LRPG:
    case CST_RAIL_BRIDGE_UDPG:
      return GROUP_RAIL_BRIDGE;
    case CST_RAIL_BRIDGE_I1LR:
    case CST_RAIL_BRIDGE_O1LR:
    case CST_RAIL_BRIDGE_I1UD:
    case CST_RAIL_BRIDGE_O1UD:
    case CST_RAIL_BRIDGE_I2LR:
    case CST_RAIL_BRIDGE_O2LR:
    case CST_RAIL_BRIDGE_I2UD:
    case CST_RAIL_BRIDGE_O2UD:
      return GROUP_RAIL;

    case CST_ROAD_LR:
    case CST_ROAD_LU:
    case CST_ROAD_LD:
    case CST_ROAD_UD:
    case CST_ROAD_UR:
    case CST_ROAD_DR:
    case CST_ROAD_LUR:
    case CST_ROAD_LDR:
    case CST_ROAD_LUD:
    case CST_ROAD_UDR:
    case CST_ROAD_LUDR:
      return GROUP_ROAD;

    case CST_ROAD_BRIDGE_LR:
    case CST_ROAD_BRIDGE_UD:
    case CST_ROAD_BRIDGE_LRP:
    case CST_ROAD_BRIDGE_UDP:
    case CST_ROAD_BRIDGE_LRPG:
    case CST_ROAD_BRIDGE_UDPG:
      return GROUP_ROAD_BRIDGE;
    case CST_ROAD_BRIDGE_I1LR:
    case CST_ROAD_BRIDGE_O1LR:
    case CST_ROAD_BRIDGE_I1UD:
    case CST_ROAD_BRIDGE_O1UD:
    case CST_ROAD_BRIDGE_I2LR:
    case CST_ROAD_BRIDGE_O2LR:
    case CST_ROAD_BRIDGE_I2UD:
    case CST_ROAD_BRIDGE_O2UD:
      return GROUP_ROAD;

    case CST_TRACK_LR:
    case CST_TRACK_LU:
    case CST_TRACK_LD:
    case CST_TRACK_UD:
    case CST_TRACK_UR:
    case CST_TRACK_DR:
    case CST_TRACK_LUR:
    case CST_TRACK_LDR:
    case CST_TRACK_LUD:
    case CST_TRACK_UDR:
    case CST_TRACK_LUDR:
      return GROUP_TRACK;

    case CST_TRACK_BRIDGE_LR:
    case CST_TRACK_BRIDGE_UD:
    case CST_TRACK_BRIDGE_LRP:
    case CST_TRACK_BRIDGE_UDP:
      return GROUP_TRACK_BRIDGE;
    case CST_TRACK_BRIDGE_ILR:
    case CST_TRACK_BRIDGE_OLR:
    case CST_TRACK_BRIDGE_IUD:
    case CST_TRACK_BRIDGE_OUD:
      return GROUP_TRACK;

    case CST_WATER:
    case CST_WATER_D:
    case CST_WATER_R:
    case CST_WATER_U:
    case CST_WATER_L:
    case CST_WATER_LR:
    case CST_WATER_UD:
    case CST_WATER_LD:
    case CST_WATER_RD:
    case CST_WATER_LU:
    case CST_WATER_UR:
    case CST_WATER_LUD:
    case CST_WATER_LRD:
    case CST_WATER_LUR:
    case CST_WATER_URD:
    case CST_WATER_LURD:
      return GROUP_WATER;

    case CST_WATERWELL:
      return GROUP_WATERWELL;

    case CST_BLACKSMITH_0:
    case CST_BLACKSMITH_1:
    case CST_BLACKSMITH_2:
    case CST_BLACKSMITH_3:
    case CST_BLACKSMITH_4:
    case CST_BLACKSMITH_5:
    case CST_BLACKSMITH_6:
      return GROUP_BLACKSMITH;

    case CST_CRICKET_1:
    case CST_CRICKET_2:
    case CST_CRICKET_3:
    case CST_CRICKET_4:
    case CST_CRICKET_5:
    case CST_CRICKET_6:
    case CST_CRICKET_7:
      return GROUP_CRICKET;

    case CST_FIRESTATION_1:
    case CST_FIRESTATION_2:
    case CST_FIRESTATION_3:
    case CST_FIRESTATION_4:
    case CST_FIRESTATION_5:
    case CST_FIRESTATION_6:
    case CST_FIRESTATION_7:
    case CST_FIRESTATION_8:
    case CST_FIRESTATION_9:
    case CST_FIRESTATION_10:
      return GROUP_FIRESTATION;

    case CST_HEALTH:
      return GROUP_HEALTH;

    case CST_MARKET_EMPTY:
    case CST_MARKET_LOW:
    case CST_MARKET_MED:
    case CST_MARKET_FULL:
      return GROUP_MARKET;

    case CST_MILL_0:
    case CST_MILL_1:
    case CST_MILL_2:
    case CST_MILL_3:
    case CST_MILL_4:
    case CST_MILL_5:
    case CST_MILL_6:
      return GROUP_MILL;

    case CST_MONUMENT_0:
    case CST_MONUMENT_1:
    case CST_MONUMENT_2:
    case CST_MONUMENT_3:
    case CST_MONUMENT_4:
    case CST_MONUMENT_5:
      return GROUP_MONUMENT;

    case CST_POTTERY_0:
    case CST_POTTERY_1:
    case CST_POTTERY_2:
    case CST_POTTERY_3:
    case CST_POTTERY_4:
    case CST_POTTERY_5:
    case CST_POTTERY_6:
    case CST_POTTERY_7:
    case CST_POTTERY_8:
    case CST_POTTERY_9:
    case CST_POTTERY_10:
      return GROUP_POTTERY;

    case CST_RECYCLE:
      return GROUP_RECYCLE;

    case CST_SCHOOL:
      return GROUP_SCHOOL;

    case CST_SHANTY:
      return GROUP_SHANTY;

    case CST_SUBSTATION_R:
    case CST_SUBSTATION_G:
    case CST_SUBSTATION_RG:
      return GROUP_SUBSTATION;

    case CST_WINDMILL_1_G:
    case CST_WINDMILL_2_G:
    case CST_WINDMILL_3_G:
    case CST_WINDMILL_1_RG:
    case CST_WINDMILL_2_RG:
    case CST_WINDMILL_3_RG:
    case CST_WINDMILL_1_R:
    case CST_WINDMILL_2_R:
    case CST_WINDMILL_3_R:
      return GROUP_WIND_POWER;
    case CST_WINDMILL_1_W:
    case CST_WINDMILL_2_W:
    case CST_WINDMILL_3_W:
      return GROUP_WINDMILL;

    case CST_INDUSTRY_L_C:
    case CST_INDUSTRY_L_Q1:
    case CST_INDUSTRY_L_Q2:
    case CST_INDUSTRY_L_Q3:
    case CST_INDUSTRY_L_Q4:
    case CST_INDUSTRY_L_L1:
    case CST_INDUSTRY_L_L2:
    case CST_INDUSTRY_L_L3:
    case CST_INDUSTRY_L_L4:
    case CST_INDUSTRY_L_M1:
    case CST_INDUSTRY_L_M2:
    case CST_INDUSTRY_L_M3:
    case CST_INDUSTRY_L_M4:
    case CST_INDUSTRY_L_H1:
    case CST_INDUSTRY_L_H2:
    case CST_INDUSTRY_L_H3:
    case CST_INDUSTRY_L_H4:
      return GROUP_INDUSTRY_L;

    case CST_RESIDENCE_LL:
      return GROUP_RESIDENCE_LL;
    case CST_RESIDENCE_ML:
      return GROUP_RESIDENCE_ML;
    case CST_RESIDENCE_HL:
      return GROUP_RESIDENCE_HL;
    case CST_RESIDENCE_LH:
      return GROUP_RESIDENCE_LH;
    case CST_RESIDENCE_MH:
      return GROUP_RESIDENCE_MH;
    case CST_RESIDENCE_HH:
      return GROUP_RESIDENCE_HH;

    case CST_UNIVERSITY:
      return GROUP_UNIVERSITY;

    case CST_COALMINE_EMPTY:
    case CST_COALMINE_LOW:
    case CST_COALMINE_MED:
    case CST_COALMINE_FULL:
      return GROUP_COALMINE;

    case CST_COMMUNE_1:
    case CST_COMMUNE_2:
    case CST_COMMUNE_3:
    case CST_COMMUNE_4:
    case CST_COMMUNE_5:
    case CST_COMMUNE_6:
    case CST_COMMUNE_7:
    case CST_COMMUNE_8:
    case CST_COMMUNE_9:
    case CST_COMMUNE_10:
    case CST_COMMUNE_11:
    case CST_COMMUNE_12:
    case CST_COMMUNE_13:
    case CST_COMMUNE_14:
      return GROUP_COMMUNE;

    case CST_EX_PORT:
      return GROUP_PORT;

    case CST_FARM_O0:
    case CST_FARM_O1:
    case CST_FARM_O2:
    case CST_FARM_O3:
    case CST_FARM_O4:
    case CST_FARM_O5:
    case CST_FARM_O6:
    case CST_FARM_O7:
    case CST_FARM_O8:
    case CST_FARM_O9:
    case CST_FARM_O10:
    case CST_FARM_O11:
    case CST_FARM_O12:
    case CST_FARM_O13:
    case CST_FARM_O14:
    case CST_FARM_O15:
    case CST_FARM_O16:
      return GROUP_ORGANIC_FARM;

    case CST_INDUSTRY_H_C:
    case CST_INDUSTRY_H_L1:
    case CST_INDUSTRY_H_L2:
    case CST_INDUSTRY_H_L3:
    case CST_INDUSTRY_H_L4:
    case CST_INDUSTRY_H_L5:
    case CST_INDUSTRY_H_L6:
    case CST_INDUSTRY_H_L7:
    case CST_INDUSTRY_H_L8:
    case CST_INDUSTRY_H_M1:
    case CST_INDUSTRY_H_M2:
    case CST_INDUSTRY_H_M3:
    case CST_INDUSTRY_H_M4:
    case CST_INDUSTRY_H_M5:
    case CST_INDUSTRY_H_M6:
    case CST_INDUSTRY_H_M7:
    case CST_INDUSTRY_H_M8:
    case CST_INDUSTRY_H_H1:
    case CST_INDUSTRY_H_H2:
    case CST_INDUSTRY_H_H3:
    case CST_INDUSTRY_H_H4:
    case CST_INDUSTRY_H_H5:
    case CST_INDUSTRY_H_H6:
    case CST_INDUSTRY_H_H7:
    case CST_INDUSTRY_H_H8:
      return GROUP_INDUSTRY_H;

    case CST_OREMINE_1:
    case CST_OREMINE_2:
    case CST_OREMINE_3:
    case CST_OREMINE_4:
    case CST_OREMINE_5:
    case CST_OREMINE_6:
    case CST_OREMINE_7:
    case CST_OREMINE_8:
      return GROUP_OREMINE;

    case CST_POWERS_COAL_EMPTY:
    case CST_POWERS_COAL_LOW:
    case CST_POWERS_COAL_MED:
    case CST_POWERS_COAL_FULL:
      return GROUP_COAL_POWER;

    case CST_POWERS_SOLAR:
      return GROUP_SOLAR_POWER;

    case CST_ROCKET_1:
    case CST_ROCKET_2:
    case CST_ROCKET_3:
    case CST_ROCKET_4:
    case CST_ROCKET_5:
    case CST_ROCKET_6:
    case CST_ROCKET_7:
    case CST_ROCKET_FLOWN:
      return GROUP_ROCKET;

    case CST_TIP_0:
    case CST_TIP_1:
    case CST_TIP_2:
    case CST_TIP_3:
    case CST_TIP_4:
    case CST_TIP_5:
    case CST_TIP_6:
    case CST_TIP_7:
    case CST_TIP_8:
      return GROUP_TIP;

    case CST_DESERT:
    case CST_DESERT_0:
    case CST_DESERT_1D:
    case CST_DESERT_1L:
    case CST_DESERT_1R:
    case CST_DESERT_1U:
    case CST_DESERT_2LD:
    case CST_DESERT_2LR:
    case CST_DESERT_2LU:
    case CST_DESERT_2RD:
    case CST_DESERT_2RU:
    case CST_DESERT_2UD:
    case CST_DESERT_3LRD:
    case CST_DESERT_3LRU:
    case CST_DESERT_3LUD:
    case CST_DESERT_3RUD:
      return GROUP_DESERT;

    case CST_TREE:
      return GROUP_TREE;
    case CST_TREE2:
      return GROUP_TREE2;
    case CST_TREE3:
      return GROUP_TREE3;
  }
}

/** @file lincity/lintypes.cpp */
