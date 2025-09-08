/* ---------------------------------------------------------------------- *
 * src/lincity/modules/monument.cpp
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

#include "monument.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <list>                           // for _List_iterator
#include <map>                            // for map
#include <string>                         // for basic_string, operator==

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/groups.hpp"             // for GROUP_MONUMENT
#include "lincity/lin-city.hpp"           // for FALSE, FLAG_EVACUATE, FLAG_...
#include "lincity/resources.hpp"          // for ExtraFrame, ResourceGroup
#include "lincity/world.hpp"              // for World
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

MonumentConstructionGroup monumentConstructionGroup(
    N_("Monument"),
    N_("Monuments"),
    FALSE,                     /* need credit? */
    GROUP_MONUMENT,
    GROUP_MONUMENT_SIZE,
    GROUP_MONUMENT_COLOUR,
    GROUP_MONUMENT_COST_MUL,
    GROUP_MONUMENT_BUL_COST,
    GROUP_MONUMENT_FIREC,
    GROUP_MONUMENT_COST,
    GROUP_MONUMENT_TECH,
    GROUP_MONUMENT_RANGE
);

//MonumentConstructionGroup monumentFinishedConstructionGroup = monumentConstructionGroup;

Construction *MonumentConstructionGroup::createConstruction(World& world) {
  return new Monument(world, this);
}

Monument::Monument(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->busy = 0;
  this->working_days = 0;
  this->tech_made = 0;
  this->tail_off = 0;
  this->completion = 0;
  this->completed = false; //don't save this one
  this->labor_consumed = 0;
  initialize_commodities();

  commodityMaxCons[STUFF_LABOR] = 100 * MONUMENT_GET_LABOR;
}

void Monument::update()
{
    if ((commodityCount[STUFF_LABOR] > MONUMENT_GET_LABOR) && (completion < 100))
    {
        consumeStuff(STUFF_LABOR, MONUMENT_GET_LABOR);
        labor_consumed += MONUMENT_GET_LABOR;
        completion = labor_consumed * 100 / BUILD_MONUMENT_LABOR;
        ++working_days;
    }
    else if (completion >= 100)
    {
        if(!completed)
        {
            completed = true;
            flags |= (FLAG_EVACUATE | FLAG_NEVER_EVACUATE);
            world.setUpdated(World::Updatable::MAP);
            //don't clear commodiyCount for savegame compatability
        }
        /* inc tech level only if fully built and tech less
           than MONUMENT_TECH_EXPIRE */
        if (world.tech_level < (MONUMENT_TECH_EXPIRE * 1000)
            && (world.total_time % MONUMENT_DAYS_PER_TECH) == 1)
        {
            tail_off++;
            if (tail_off > (world.tech_level / 10000) - 2)
            {
                world.tech_level++;
                tech_made++;
                tail_off = 0;
            }
        }
    }
    //monthly update
    if(world.total_time % 100 == 99) {
      reset_prod_counters();
      busy = working_days;
      working_days = 0;
      if(commodityCount[STUFF_LABOR]==0 && completed)
        deneighborize();
    }
}

void Monument::animate(unsigned long real_time) {
  int& frame = frameIt->frame;
  if(completed) {
    frame = 0;
    frameIt->resourceGroup = ResourceGroup::resMap["Monument"];
    soundGroup = frameIt->resourceGroup;
  }
  else
    frame = completion / 20;
}

void Monument::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.addBlank();
  /* Display tech contribution only after monument is complete */
  if(completion >= 100) {
    mps.addBlank();
    mps.add_sfp(N_("Wisdom bestowed"), tech_made * 100.0 / MAX_TECH_LEVEL);
  }
  else {
    mps.add_sfp(N_("busy"), (float) busy);
    // i++;
    list_commodities(mps, production);
    mps.addBlank();
    mps.add_sfp(N_("Completion"), completion);
  }
}

bool
Monument::can_bulldoze(Message::ptr& message) const {
  if(!completed) {
    message = CannotBulldozeIncompleteMonumentMessage::create(point);
    return false;
  }
  return Construction::can_bulldoze(message);
}

void Monument::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"tech_made", xmlFormat<int>(tech_made));
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"tail_off", xmlFormat<int>(tail_off));
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"completion", xmlFormat<int>(completion));
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"labor_consumed", xmlFormat<int>(labor_consumed));
  Construction::save(xmlWriter);
}

bool Monument::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string name = xmlReader.get_name();
  if     (name == "tech_made")  tech_made      = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "tail_off")   tail_off       = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "completion") completion     = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "labor_consumed" || name == "jobs_consumed")
                                labor_consumed = xmlParse<int>(xmlReader.read_inner_xml());
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/monument.cpp */
