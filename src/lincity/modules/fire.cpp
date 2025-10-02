/* ---------------------------------------------------------------------- *
 * src/lincity/modules/fire.cpp
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

#include "fire.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <list>                           // for _List_iterator
#include <map>                            // for map
#include <string>                         // for basic_string, operator==
#include <vector>                         // for vector

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint
#include "lincity/groups.hpp"             // for GROUP_FIRE
#include "lincity/lin-city.hpp"           // for FLAG_FIRE_COVER, ANIM_THRES...
#include "lincity/resources.hpp"          // for ExtraFrame, GraphicsInfo
#include "lincity/world.hpp"              // for Map, MapTile, World
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

FireConstructionGroup fireConstructionGroup(
     N_("Fire"),
     N_("Fires"),
     FALSE,                     /* need credit? */
     GROUP_FIRE,
     GROUP_FIRE_SIZE,
     GROUP_FIRE_COLOUR,
     GROUP_FIRE_COST_MUL,
     GROUP_FIRE_BUL_COST,
     GROUP_FIRE_FIREC,
     GROUP_FIRE_COST,
     GROUP_FIRE_TECH,
     GROUP_FIRE_RANGE
);

//helper groups for graphics and sound sets, dont add them to ConstructionGroup::groupMap
//FireConstructionGroup fireWasteLandConstructionGroup = fireConstructionGroup;

Construction *FireConstructionGroup::createConstruction(World& world) {
  return new Fire(world, this);
}

Fire::Fire(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
    this->constructionGroup = cstgrp;
    this->burning_days = 0;
    this->smoking_days = 0;
    this->anim = 0;
    this->days_before_spread = FIRE_DAYS_PER_SPREAD;
}

void Fire::update()
{
    /* this so we don't get whole blocks changing in one go. */
    if (burning_days == 0)
    {   burning_days = rand() % (FIRE_LENGTH / 5);}

    if (burning_days > FIRE_LENGTH)
    {
        //is_burning = false;
        if (smoking_days == 0)   /* rand length here also */
        {   smoking_days = rand() % (AFTER_FIRE_LENGTH / 6);}
        smoking_days++;
        if (world.map(point)->flags & FLAG_FIRE_COVER)
        {   smoking_days += 4;}
        if(smoking_days > AFTER_FIRE_LENGTH)
          detach();
        return;
    }

    burning_days++;
    if (world.map(point)->flags & FLAG_FIRE_COVER)
    {   burning_days += 4;}
    days_before_spread--;
    world.map(point)->pollution++;
    if(days_before_spread == 0) {
      days_before_spread = FIRE_DAYS_PER_SPREAD;
      spread();
    }
}

void Fire::spread() {
  MapPoint loc;
  switch(rand() % 80) {
  case 0:
    loc = point.n();
    break;
  case 1:
    loc = point.s();
    break;
  case 2:
    loc = point.e();
    break;
  case 3:
    loc = point.w();
    break;
  default:
    return;
  }

  // TODO: spread should be faster than do_random_fire
  // TODO: fire cover should only slow down spread -- not stop it
  if(!world.map.is_visible(loc))
    return;
  Construction *cst = world.map(loc)->reportingConstruction;
  if(!cst) return;
  if(rand() % 100 >= cst->constructionGroup->fire_chance)
    return;
  if(world.map(loc)->flags & FLAG_FIRE_COVER)
    return;

  cst->torch();
}

void Fire::animate(unsigned long real_time) {
  int& frame = frameIt->frame;

  if(smoking_days) {
    frameIt->resourceGroup = ResourceGroup::resMap["FireWasteLand"];
    soundGroup = frameIt->resourceGroup;
    frame = smoking_days * 4 / AFTER_FIRE_LENGTH;
    if(frame >= 4) frame = 4; // shouldn't happen
  }
  else if(real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(FIRE_ANIMATION_SPEED);
    if(++frame >= (int)frameIt->resourceGroup->graphicsInfoVector.size())
      frame = 0;
  }
}

void Fire::report(Mps& mps, bool production) const {
    mps.add_s(constructionGroup->name);
    mps.addBlank();
    mps.add_sd(N_("Air Pollution"), world.map(point)->pollution);
    if(burning_days < FIRE_LENGTH)
      mps.add_sddp(N_("burnt down"), burning_days, FIRE_LENGTH);
    else
      mps.add_sddp(N_("degraded"), smoking_days, AFTER_FIRE_LENGTH);
}

bool
Fire::can_bulldoze(Message::ptr& message) const {
  message = CannotBulldozeThisEverMessage::create(
    point, fireConstructionGroup);
  return false;
}

void
Fire::torch() {
  // seeing how it's already on fire...
  return;
}

void Fire::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"burning_days",       xmlFormat<int>(burning_days));
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"smoking_days",       xmlFormat<int>(smoking_days));
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"days_before_spread", xmlFormat<int>(days_before_spread));
  Construction::save(xmlWriter);
}

bool Fire::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string name = xmlReader.get_name();
  if     (name == "burning_days")       burning_days       = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "smoking_days")       smoking_days       = xmlParse<int>(xmlReader.read_inner_xml());
  else if(name == "days_before_spread") days_before_spread = xmlParse<int>(xmlReader.read_inner_xml());
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/fire.cpp */
