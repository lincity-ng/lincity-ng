/* ---------------------------------------------------------------------- *
 * university.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "university.h"

#include "modules.h"

// university place:
UniversityConstructionGroup universityConstructionGroup(
    N_("University"),
     TRUE,                     /* need credit? */
     GROUP_UNIVERSITY,
     GROUP_UNIVERSITY_SIZE,
     GROUP_UNIVERSITY_COLOUR,
     GROUP_UNIVERSITY_COST_MUL,
     GROUP_UNIVERSITY_BUL_COST,
     GROUP_UNIVERSITY_FIREC,
     GROUP_UNIVERSITY_COST,
     GROUP_UNIVERSITY_TECH,
     GROUP_UNIVERSITY_RANGE
);

Construction *UniversityConstructionGroup::createConstruction() {
  return new University(this);
}

void University::update()
{
    university_cost += UNIVERSITY_RUNNING_COST;
    //do the teaching
    if (commodityCount[STUFF_LABOR] >= UNIVERSITY_LABOR
    &&  commodityCount[STUFF_GOODS] >= UNIVERSITY_GOODS
    &&  commodityCount[STUFF_WASTE] + UNIVERSITY_GOODS / 3 <= MAX_WASTE_AT_UNIVERSITY)
    {
        consumeStuff(STUFF_LABOR, UNIVERSITY_LABOR);
        consumeStuff(STUFF_GOODS, UNIVERSITY_GOODS);
        produceStuff(STUFF_WASTE, UNIVERSITY_GOODS / 3);
        ++working_days;
        tech_level += UNIVERSITY_TECH_MADE;
        total_tech_made += UNIVERSITY_TECH_MADE;
    }
    //monthly update
    if ((total_time % 100) == 99)
    {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }
}

void University::report()
{
    int i = 0;
    mps_store_title(i, constructionGroup->name);
    i++;
    mps_store_sfp(i++, N_("busy"), busy);
    mps_store_sfp(i++, N_("Tech researched"), total_tech_made * 100.0 / MAX_TECH_LEVEL);
    // i++;
    list_commodities(&i);
}

void University::save(xmlTextWriterPtr xmlWriter) {
  xmlTextWriterWriteFormatElement(xmlWriter, "total_tech_made", "%d", total_tech_made);
}

bool University::loadMember(xmlpp::TextReader& xmlReader) {
  std::string name = xmlReader.get_name();
  if(name == "total_tech_made") total_tech_made = std::stoi(xmlReader.get_inner_xml());
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/university.cpp */
