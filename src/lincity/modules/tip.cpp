/* ---------------------------------------------------------------------- *
 * tip.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "tip.h"

#include <list>                     // for _List_iterator

#include "modules.h"

// Tip:
TipConstructionGroup tipConstructionGroup(
    N_("Land Fill"),
     FALSE,                     /* need credit? */
     GROUP_TIP,
     GROUP_TIP_SIZE,
     GROUP_TIP_COLOUR,
     GROUP_TIP_COST_MUL,
     GROUP_TIP_BUL_COST,
     GROUP_TIP_FIREC,
     GROUP_TIP_COST,
     GROUP_TIP_TECH,
     GROUP_TIP_RANGE
);

Construction *TipConstructionGroup::createConstruction() {
  return new Tip(this);
}


void Tip::update()
{
    //the waste is always slowly degrading
    //max degradiation per day is about 42 (10M/240k)
    degration_days += total_waste;
    total_waste -= degration_days / TIP_DEGRADE_TIME;
    degration_days %= TIP_DEGRADE_TIME;

    if ((commodityCount[STUFF_WASTE] >= WASTE_BURRIED)
    && (commodityCount[STUFF_WASTE]*100/TIP_TAKES_WASTE > CRITICAL_WASTE_LEVEL)
    && (total_waste + WASTE_BURRIED <= MAX_WASTE_AT_TIP))
    {
        consumeStuff(STUFF_WASTE, WASTE_BURRIED);
        total_waste += WASTE_BURRIED;
        working_days++;
        sust_dig_ore_coal_tip_flag = 0;
    }
    else if ((commodityCount[STUFF_WASTE] + WASTE_BURRIED <= TIP_TAKES_WASTE)
    && (commodityCount[STUFF_WASTE]*100/TIP_TAKES_WASTE < CRITICAL_WASTE_LEVEL)
    && (total_waste > 0))
    {
        int waste_dug = (WASTE_BURRIED < total_waste)?WASTE_BURRIED:total_waste;
        produceStuff(STUFF_WASTE, waste_dug);
        total_waste -= waste_dug;
        working_days++;
    }
    if ((total_time % 100) == 99)
    {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
    }
}

void Tip::animate() {
  int i = (total_waste /3 * 22) / MAX_WASTE_AT_TIP;
  if (total_waste > 0 && i < 8)
  {   i++;}
  frameIt->frame = i;
}

void Tip::report()
{
    int i = 0;

    mps_store_title(i, constructionGroup->name);
    i++;
    mps_store_sfp(i++,N_("busy"), busy);
    mps_store_sd(i++, N_("Waste"), total_waste);
    mps_store_sfp(i++, N_("Filled"), (float)total_waste*100/MAX_WASTE_AT_TIP);
    // i++;
    list_commodities(&i);
}

void Tip::save(xmlTextWriterPtr xmlWriter) {
  xmlTextWriterWriteFormatElement(xmlWriter, "total_waste",    "%d", total_waste);
  xmlTextWriterWriteFormatElement(xmlWriter, "degration_days", "%d", degration_days);
}

bool Tip::loadMember(xmlpp::TextReader& xmlReader) {
  std::string name = xmlReader.get_name();
  if     (name == "total_waste")    total_waste    = std::stoi(xmlReader.get_inner_xml());
  else if(name == "degration_days") degration_days = std::stoi(xmlReader.get_inner_xml());
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/tip.cpp */
