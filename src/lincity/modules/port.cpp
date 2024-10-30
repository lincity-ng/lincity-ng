/* ---------------------------------------------------------------------- *
 * src/lincity/modules/port.cpp
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

#include "port.h"

#include "modules.h"

// Port:
PortConstructionGroup portConstructionGroup(
    N_("Port"),
     FALSE,                     /* need credit? */
     GROUP_PORT,
     GROUP_PORT_SIZE,
     GROUP_PORT_COLOUR,
     GROUP_PORT_COST_MUL,
     GROUP_PORT_BUL_COST,
     GROUP_PORT_FIREC,
     GROUP_PORT_COST,
     GROUP_PORT_TECH,
     GROUP_PORT_RANGE
);

Construction *PortConstructionGroup::createConstruction() {
  return new Port(this);
}

int Port::buy_stuff(Commodity stuff)
//fills up a PORT_IMPORT_RATE fraction of the available capacity and returns the cost
//amount to buy must exceed PORT_TRIGGER_RATE
{
    if(!portConstructionGroup.tradeRule[stuff].take)
        return 0;
    int i = portConstructionGroup.commodityRuleCount[stuff].maxload - commodityCount[stuff];
    i = (i * PORT_IMPORT_RATE) / 1000;
    if (i < (portConstructionGroup.commodityRuleCount[stuff].maxload / PORT_TRIGGER_RATE))
    {   return 0;}
    produceStuff(stuff, i);
    return (i * portConstructionGroup.commodityRates[stuff]);
}

int Port::sell_stuff(Commodity stuff)
//sells a PORT_IMPORT_RATE fraction of the current load and returns the revenue
//amount to sell must exceed PORT_TRIGGER_RATE
{
    if(!portConstructionGroup.tradeRule[stuff].give)
        return 0;
    int i = commodityCount[stuff];
    i = (i * PORT_EXPORT_RATE) / 1000;
    if (i < (portConstructionGroup.commodityRuleCount[stuff].maxload / PORT_TRIGGER_RATE))
    {   return 0;}
    consumeStuff(stuff, i);
    return (i * portConstructionGroup.commodityRates[stuff]);
}

void Port::trade_connection()
{
    //Checks all flags and issues buy_stuff sell_stuff accordingly
    for(Commodity stuff = STUFF_INIT; stuff < STUFF_COUNT; stuff++ ) {
        CommodityRule& rule = commodityRuleCount[stuff];
        if(!rule.maxload) continue;
        if (rule.take == rule.give)
        {   continue;}
        if (rule.take)
        {   daily_ic += buy_stuff(stuff);}
        else if (rule.give)
        {   daily_et += sell_stuff(stuff);}
    }
}

void Port::update()
{
    daily_ic = 0;
    daily_et = 0;

    if (commodityCount[STUFF_LABOR] >= PORT_LABOR)//there is enough workforce
    {
        trade_connection();
        if (daily_ic || daily_et)
        {
            consumeStuff(STUFF_LABOR, PORT_LABOR);
            world(x,y)->pollution += PORT_POLLUTION;
            sust_port_flag = 0;
            tech_made++;
            tech_level++;
            working_days++;
            if (daily_ic && daily_et)
            {   tech_level++;}
        }
    }
    monthly_ic += daily_ic;
    monthly_et += daily_et;
    //monthly update
    if (total_time % 100 == 99)
    {
        reset_prod_counters();
        busy = working_days;
        working_days = 0;
        lastm_ic = monthly_ic;
        lastm_et = monthly_et;
        monthly_ic = 0;
        monthly_et = 0;
    }

    daily_et += pence;
    export_tax += daily_et / 100;
    pence = daily_et % 100;
    import_cost += daily_ic;
}

void Port::report()
{
    int i = 0;
    mps_store_title(i, constructionGroup->name);
    mps_store_sfp(i++, N_("busy"), busy);
    mps_store_sd(i++, N_("Export"),lastm_et/100);
    mps_store_sd(i++, N_("Import"),lastm_ic/100);
    mps_store_sfp(i++, N_("Culture exchanged"), tech_made * 100.0 / MAX_TECH_LEVEL);
    // i++;
    list_commodities(&i);
}

void Port::save(xmlTextWriterPtr xmlWriter) {
  xmlTextWriterWriteFormatElement(xmlWriter, "tech_made", "%d", tech_made);

  const std::string givePfx("give_");
  const std::string takePfx("take_");
  for(Commodity stuff = STUFF_INIT; stuff < STUFF_COUNT; stuff++) {
    CommodityRule& rule = commodityRuleCount[stuff];
    if(!rule.maxload) continue;
    const char *name = getCommodityStandardName(stuff);
    xmlTextWriterWriteFormatElement(xmlWriter, givePfx + name, "%d", rule.give);
    xmlTextWriterWriteFormatElement(xmlWriter, takePfx + name, "%d", rule.take);
  }
}

bool Port::loadMember(xmlpp::TextReader& xmlReader) {
  std::string tag = xmlReader.get_name();
  bool give;
  if((give = tag.starts_with("give_")) || tag.starts_with("take_")) {
    CommodityRule& rule =
      commodityRuleCount[commodityFromStandardName(tag.substr(5).c_str())];
    give ? rule.give : rule.take = std::stoi(xmlReader.get_inner_xml());
  }
  else if(tag == "tech_made") tech_made = std::stoi(xmlReader.get_inner_xml());
  else return Construction::loadMember(xmlReader);
  return true;
}

/** @file lincity/modules/port.cpp */
