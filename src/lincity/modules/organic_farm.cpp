/* ---------------------------------------------------------------------- *
 * organic_farm.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "organic_farm.h"

#include <list>                     // for _List_iterator

#include "modules.h"


Organic_farmConstructionGroup organic_farmConstructionGroup(
    N_("Farm"),
    FALSE,                     /* need credit? */
    GROUP_ORGANIC_FARM,
    GROUP_ORGANIC_FARM_SIZE,
    GROUP_ORGANIC_FARM_COLOUR,
    GROUP_ORGANIC_FARM_COST_MUL,
    GROUP_ORGANIC_FARM_BUL_COST,
    GROUP_ORGANIC_FARM_FIREC,
    GROUP_ORGANIC_FARM_COST,
    GROUP_ORGANIC_FARM_TECH,
    GROUP_ORGANIC_FARM_RANGE
);

Construction *Organic_farmConstructionGroup::createConstruction() {
  return new Organic_farm(this);
}


void Organic_farm::update()
{
    int used_labor = 0;
    int used_power = 0;
    int used_water = 0;
    int foodprod = 0;

    max_foodprod = 0;
    /* check labor */
    used_labor = (FARM_LABOR_USED<commodityCount[STUFF_LABOR]?FARM_LABOR_USED:commodityCount[STUFF_LABOR]);
    flags &= ~(FLAG_POWERED);
    /* check for power */
    if (commodityCount[STUFF_LOVOLT] >= ORG_FARM_POWER_REC)
    {
        used_power = ORG_FARM_POWER_REC;
        flags |= FLAG_POWERED;
        if (commodityCount[STUFF_WASTE] >= 3 * ORG_FARM_WASTE_GET)
        {   consumeStuff(STUFF_WASTE, ORG_FARM_WASTE_GET);}
        used_water = commodityCount[STUFF_WATER] / WATER_FARM;
        if (used_water > (16 - ugwCount))
        {   used_water = (16 - ugwCount);}

        foodprod = (ORGANIC_FARM_FOOD_OUTPUT + tech_bonus) * (ugwCount+used_water) * used_labor / (16 * FARM_LABOR_USED);
        max_foodprod = (ORGANIC_FARM_FOOD_OUTPUT + tech_bonus) * (ugwCount+used_water)  / (16);
    }
    else
    {
        foodprod = (ORGANIC_FARM_FOOD_OUTPUT) * ugwCount * used_labor / (4 * 16 * FARM_LABOR_USED);
        max_foodprod = (ORGANIC_FARM_FOOD_OUTPUT) * ugwCount / (4 * 16);
    }
    if (foodprod < 30)
    {   foodprod = 30;}
    if (max_foodprod < 30) //that could only matter if Fertiliy = 0
    {   max_foodprod = 30;}

    if (commodityCount[STUFF_FOOD] + foodprod > MAX_ORG_FARM_FOOD)
    {   //we would produce too much so use less power, labor and water
        used_labor = used_labor * (MAX_ORG_FARM_FOOD - commodityCount[STUFF_FOOD]) / foodprod;
        used_power = used_power * (MAX_ORG_FARM_FOOD - commodityCount[STUFF_FOOD]) / foodprod;
        if ((MAX_ORG_FARM_FOOD - commodityCount[STUFF_FOOD])*16 < ugwCount * foodprod)
        {   used_water = 0;}
        foodprod = MAX_ORG_FARM_FOOD - commodityCount[STUFF_FOOD];
    }
    /* Now apply changes */
    if (foodprod >= 30)
    {
        consumeStuff(STUFF_LABOR, used_labor);
        produceStuff(STUFF_FOOD, foodprod);
        consumeStuff(STUFF_LOVOLT, used_power);
        consumeStuff(STUFF_WATER, used_water * WATER_FARM);
        food_this_month += 100 * foodprod / max_foodprod;
    }
    // monthly update
    if (total_time % 100 == 99) {
        reset_prod_counters();
        food_last_month = food_this_month;
        food_this_month = 0;
    }
}

void Organic_farm::animate() {
  int i = (total_time + crop_rotation_key * 1200 + month_stagger) % 4800;
  //Every three month
  if (i % 300 == 0) {
    i /= 300;
    if ( food_last_month > MIN_FOOD_SOLD_FOR_ANIM) {
      //Every year
      if (i % 4 == 0)
        month_stagger = rand() % 100;
      frameIt->frame = 1+i/4;
    }
    else {
      frameIt->frame = 0;
    }
  }
}

void Organic_farm::report()
{
    int i = 0;

    mps_store_title(i, constructionGroup->name);
    i++;
    mps_store_sddp(i++, N_("Fertility"), ugwCount, 16);
    mps_store_sfp(i++, N_("Tech"), tech * 100.0 / MAX_TECH_LEVEL);
    mps_store_sfp(i++, N_("busy"), (float)food_last_month / 100.0);
    mps_store_sd(i++, N_("Output"), max_foodprod);
    // i++;
    list_commodities(&i);
}

void Organic_farm::save(xmlTextWriterPtr xmlWriter) {
  xmlTextWriterWriteFormatElement(xmlWriter, "tech", "%d", tech);
}

bool Organic_farm::loadMember(xmlpp::TextReader& xmlReader) {
  std::string tag = xmlReader.get_name();
  if(tag == "tech") tech = std::stoi(xmlReader.get_inner_xml());
  else if(tag == "tech_bonus");
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/organic_farm.cpp */
