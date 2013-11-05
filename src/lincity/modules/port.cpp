/* ---------------------------------------------------------------------- *
 * port.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "port.h"
#include "../transport.h"

// Port:
PortConstructionGroup portConstructionGroup(
    "Port",
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

Construction *PortConstructionGroup::createConstruction(int x, int y) {
    return new Port(x, y, this);
}

int Port::buy_stuff(Commodities stuff)
//fills up a PORT_IMPORT_RATE fraction of the available capacity and returns the cost
//amount to buy must exceed PORT_TRIGGER_RATE
{
    int i = portConstructionGroup.commodityRuleCount[stuff].maxload - commodityCount[stuff];
    i = (i * PORT_IMPORT_RATE) / 1000;
    if (i < (portConstructionGroup.commodityRuleCount[stuff].maxload / PORT_TRIGGER_RATE))
    {   return 0;}
    commodityCount[stuff] += i;
    return (i * portConstructionGroup.commodityRates[stuff]);
}

int Port::sell_stuff(Commodities stuff)
//sells a PORT_IMPORT_RATE fraction of the current load and returns the revenue
//amount to sell must exceed PORT_TRIGGER_RATE
{
    int i = commodityCount[stuff];
    i = (i * PORT_EXPORT_RATE) / 1000;
    if (i < (portConstructionGroup.commodityRuleCount[stuff].maxload / PORT_TRIGGER_RATE))
    {   return 0;}
    commodityCount[stuff] -= i;
    return (i * portConstructionGroup.commodityRates[stuff]);
}

void Port::trade_connection()
{
    //Checks all flags and issues buy_stuff sell_stuff accordingly
    std::map<Commodities, CommodityRule>::iterator stuff_it;
    for(stuff_it = commodityRuleCount.begin() ; stuff_it != commodityRuleCount.end() ; stuff_it++ )
    {
        if (stuff_it->second.take == stuff_it->second.give)
        {   continue;}
        if (stuff_it->second.take)
        {   daily_ic += buy_stuff(stuff_it->first);}
        else if (stuff_it->second.give)
        {   daily_et += sell_stuff(stuff_it->first);}
    }
}

void Port::update()
{
    daily_ic = 0;
    daily_et = 0;

    if (commodityCount[STUFF_JOBS] >= PORT_JOBS)//there is enough workforce
    {
        trade_connection();
        if (daily_ic || daily_et)
        {
            commodityCount[STUFF_JOBS] -= PORT_JOBS;
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
    if (total_time % 100 == 0)
    {
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
    mps_store_sd(i++, constructionGroup->name, ID);
    mps_store_sfp(i++, "busy", busy);
    mps_store_sd(i++, "Export",lastm_et/100);
    mps_store_sd(i++, "Import",lastm_ic/100);
    mps_store_sfp(i++,"Culture exchanged", tech_made * 100.0 / MAX_TECH_LEVEL);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/port.cpp */

