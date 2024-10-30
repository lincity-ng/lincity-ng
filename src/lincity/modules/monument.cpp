/* ---------------------------------------------------------------------- *
 * monument.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "monument.h"

#include <list>                     // for _List_iterator
#include <map>                      // for map

#include "modules.h"

extern int mps_x, mps_y;

MonumentConstructionGroup monumentConstructionGroup(
    N_("Monument"),
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

Construction *MonumentConstructionGroup::createConstruction() {
  return new Monument(this);
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
            if (mps_x == x && mps_y == y)
            {   mps_set(x, y, MPS_MAP);}
            //don't clear commodiyCount for savegame compatability
        }
        /* inc tech level only if fully built and tech less
           than MONUMENT_TECH_EXPIRE */
        if (tech_level < (MONUMENT_TECH_EXPIRE * 1000)
            && (total_time % MONUMENT_DAYS_PER_TECH) == 1)
        {
            tail_off++;
            if (tail_off > (tech_level / 10000) - 2)
            {
                tech_level++;
                tech_made++;
                tail_off = 0;
            }
        }
    }
    //monthly update
    if (total_time % 100 == 99)
    {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
        if(commodityCount[STUFF_LABOR]==0 && completed)
        {   deneighborize();}
    }
}

void Monument::animate() {
  int& frame = frameIt->frame;
  if(completed) {
    frame = 0;
    frameIt->resourceGroup = ResourceGroup::resMap["Monument"];
    soundGroup = frameIt->resourceGroup;
  }
  else
    frame = completion / 20;
}

void Monument::report()
{
    int i = 0;

    mps_store_title(i, constructionGroup->name);
    i++;
    /* Display tech contribution only after monument is complete */
    if (completion >= 100) {
        i++;
        mps_store_sfp(i++, N_("Wisdom bestowed"), tech_made * 100.0 / MAX_TECH_LEVEL);
    }
    else
    {
        mps_store_sfp(i++, N_("busy"), (float) busy);
        // i++;
        list_commodities(&i);
        i++;
        mps_store_sfp(i++, N_("Completion"), completion);
    }
}

void Monument::save(xmlTextWriterPtr xmlWriter) {
  xmlTextWriterWriteFormatElement(xmlWriter, "tech_made", "%d", tech_made);
  xmlTextWriterWriteFormatElement(xmlWriter, "tail_off", "%d", tail_off);
  xmlTextWriterWriteFormatElement(xmlWriter, "completion", "%d", completion);
  xmlTextWriterWriteFormatElement(xmlWriter, "labor_consumed", "%d", labor_consumed);
}

bool Monument::loadMember(xmlpp::TextReader& xmlReader) {
  std::string name = xmlReader.get_name();
  if     (name == "tech_made")  tech_made      = std::stoi(xmlReader.get_inner_xml());
  else if(name == "tail_off")   tail_off       = std::stoi(xmlReader.get_inner_xml());
  else if(name == "completion") completion     = std::stoi(xmlReader.get_inner_xml());
  else if(name == "labor_consumed" || name == "jobs_consumed")
                                labor_consumed = std::stoi(xmlReader.get_inner_xml());
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/monument.cpp */
