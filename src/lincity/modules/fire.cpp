/* ---------------------------------------------------------------------- *
 * fire.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */


#include "fire.h"

#include <list>                           // for _List_iterator
#include <map>                            // for map
#include <vector>                         // for vector

#include "lincity/ConstructionManager.h"  // for ConstructionManager
#include "lincity/ConstructionRequest.h"  // for ConstructionDeletionRequest
#include "modules.h"                      // for ExtraFrame, do_random_fire
//#include "lincity-ng/Sound.hpp"

FireConstructionGroup fireConstructionGroup(
     N_("Fire"),
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

Construction *FireConstructionGroup::createConstruction() {
  return new Fire(this);
}

void Fire::update()
{
    int i;
    /* this so we don't get whole blocks changing in one go. */
    if (burning_days == 0)
    {   burning_days = rand() % (FIRE_LENGTH / 5);}

    if (burning_days > FIRE_LENGTH)
    {
        //is_burning = false;
        if (smoking_days == 0)   /* rand length here also */
        {   smoking_days = rand() % (AFTER_FIRE_LENGTH / 6);}
        smoking_days++;
        if (world(x,y)->flags & FLAG_FIRE_COVER)
        {   smoking_days += 4;}
        if (!(flags & FLAG_IS_GHOST) && smoking_days > AFTER_FIRE_LENGTH)
        {   ConstructionManager::submitRequest( new ConstructionDeletionRequest(this) ); }
        return;
    }

    burning_days++;
    if (world(x,y)->flags & FLAG_FIRE_COVER)
    {   burning_days += 4;}
    days_before_spread--;
    if( !(flags & FLAG_IS_GHOST) )
    {   world(x,y)->pollution++;}
    if ((days_before_spread == 0) && !(flags & FLAG_IS_GHOST))
    {
        days_before_spread = FIRE_DAYS_PER_SPREAD;
        if ((rand() % 20) == 1)
        {
            i = rand() % 4;
            switch (i)
            {
                case (0):
                    do_random_fire(x - 1, y, 0);
                break;
                case (1):
                    do_random_fire(x, y - 1, 0);
                break;
                case (2):
                    do_random_fire(x + 1, y, 0);
                break;
                case (3):
                    do_random_fire(x, y + 1, 0);
                break;
            }
        }
    }
}

void Fire::animate() {
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

void Fire::report()
{
    int i = 0;

    mps_store_title(i, constructionGroup->name);
    i++;
    mps_store_sd(i++,N_("Air Pollution"), world(x,y)->pollution);
    if (burning_days < FIRE_LENGTH)
    {   mps_store_sddp(i++,N_("burnt down"), burning_days, FIRE_LENGTH);}
    else
    {   mps_store_sddp(i++,N_("degraded"), smoking_days, AFTER_FIRE_LENGTH);}
}

void Fire::save(xmlTextWriterPtr xmlWriter) {
  xmlTextWriterWriteFormatElement(xmlWriter, "burning_days",       "%d", burning_days);
  xmlTextWriterWriteFormatElement(xmlWriter, "smoking_days",       "%d", smoking_days);
  xmlTextWriterWriteFormatElement(xmlWriter, "days_before_spread", "%d", days_before_spread);
}

bool Fire::loadMember(xmlpp::TextReader& xmlReader) {
  std::string name = xmlReader.get_name();
  if     (name == "burning_days")       burning_days       = std::stoi(xmlReader.get_inner_xml());
  else if(name == "smoking_days")       smoking_days       = std::stoi(xmlReader.get_inner_xml());
  else if(name == "days_before_spread") days_before_spread = std::stoi(xmlReader.get_inner_xml());
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/fire.cpp */
