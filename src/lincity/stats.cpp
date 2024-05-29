/* ---------------------------------------------------------------------- *
 * ldsvguts.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2002.
 * Portions copyright (c) Corey Keasling, 2000-2002.
 * ---------------------------------------------------------------------- */
#include <algorithm>        // for copy, max
#include <map>              // for map

#include "commodities.hpp"  // for Commodity
#include "engglobs.h"       // for housing, population, starving_population
#include "lintypes.h"       // for tstat_capacities, tstat_census
#include "stats.h"

/* ---------------------------------------------------------------------- *
 * Public Global Variables
 * ---------------------------------------------------------------------- */

/* Daily accumulators */
//int food_in_markets;
//int labor_in_markets;
//int coal_in_markets;
//int goods_in_markets;
//int ore_in_markets;
//int steel_in_markets;
//int waste_in_markets;
int dbirths, ddeaths;

/* Monthly accumulators */
int tfood_in_markets;
int tlabor_in_markets;
int tcoal_in_markets;
int tgoods_in_markets;
int tore_in_markets;
int tsteel_in_markets;
int twaste_in_markets;
int tpopulation;
int thousing;
int tstarving_population;
int tunemployed_population;
int tbirths, tdeaths, tunnat_deaths;
int ltbirths = 0;
int ltdeaths = 0;
int ltunnat_deaths = 0;
std::deque<int> birthq, deathq, unnatdeathq;

/* yearly */
int income_tax;
int coal_tax;
int goods_tax;
int export_tax;
int import_cost;
int unemployment_cost;
int transport_cost;
int windmill_cost;
int university_cost;
int recycle_cost;
int deaths_cost;
int health_cost;
int rocket_pad_cost;
int school_cost;
int fire_cost;
int cricket_cost;
int other_cost;

/* Last Year's totals */
int ly_income_tax;
int ly_coal_tax;
int ly_goods_tax;
int ly_export_tax;
int ly_import_cost;
int ly_other_cost;
int ly_unemployment_cost;
int ly_transport_cost;
int ly_university_cost;
int ly_recycle_cost;
int ly_school_cost;
int ly_deaths_cost;
int ly_health_cost;
int ly_rocket_pad_cost;
int ly_interest;
int ly_windmill_cost;
int ly_cricket_cost;
int ly_fire_cost;
int ly_births = 0;
int ly_deaths = 0;
int ly_unnatdeaths = 0;

/* Averaging variables */
int data_last_month;


void init_daily(void)
{
    population = 0;
    housing = 0;
    starving_population = 0;
    unemployed_population = 0;
    ddeaths = 0;
    dbirths = 0;
}

void init_monthly(void)
{
    data_last_month = 0;
    tpopulation = 0;
    thousing = 0;
    tstarving_population = 0;
    tunemployed_population = 0;
    init_census();
    birthq.push_back(tbirths);
    deathq.push_back(tdeaths);
    unnatdeathq.push_back(tunnat_deaths);
    ly_births += tbirths;
    ly_deaths += tdeaths;
    ly_unnatdeaths += tunnat_deaths;
    int s = birthq.size();
    if(s>12)
    {
        ly_births -= birthq.front();
        ly_deaths -= deathq.front();
        ly_unnatdeaths -= unnatdeathq.front();
        --s;
        birthq.pop_front();
        deathq.pop_front();
        unnatdeathq.pop_front();
        ltbirths = ly_births;
        ltdeaths = ly_deaths;
        ltunnat_deaths = ly_unnatdeaths;
    }
    else
    {
        ltbirths = 12*ly_births/s;
        ltdeaths = 12*ly_deaths/s;
        ltunnat_deaths = 12*ly_unnatdeaths/s;
    }

    tbirths = 0;
    tdeaths = 0;
    tunnat_deaths = 0;
}

void init_census()
{
    tstat_census[STUFF_FOOD] = 0;
    tstat_census[STUFF_LABOR] = 0;
    tstat_census[STUFF_COAL] = 0;
    tstat_census[STUFF_ORE] = 0;
    tstat_census[STUFF_WATER] = 0;
    tstat_census[STUFF_GOODS] = 0;
    tstat_census[STUFF_WASTE] = 0;
    tstat_census[STUFF_STEEL] = 0;
    tstat_census[STUFF_LOVOLT] = 0;
    tstat_census[STUFF_HIVOLT] = 0;

    tstat_capacities[STUFF_FOOD] = 1;
    tstat_capacities[STUFF_LABOR] = 1;
    tstat_capacities[STUFF_COAL] = 1;
    tstat_capacities[STUFF_ORE] = 1;
    tstat_capacities[STUFF_STEEL] = 1;
    tstat_capacities[STUFF_WATER] = 1;
    tstat_capacities[STUFF_GOODS] = 1;
    tstat_capacities[STUFF_WASTE] = 1;
    tstat_capacities[STUFF_LOVOLT] = 1;
    tstat_capacities[STUFF_HIVOLT] = 1;
}

void init_yearly(void)
{
    income_tax = 0;
    coal_tax = 0;
    unemployment_cost = 0;
    transport_cost = 0;
    goods_tax = 0;
    export_tax = 0;
    import_cost = 0;
    windmill_cost = 0;
    university_cost = 0;
    recycle_cost = 0;
    deaths_cost = 0;
    health_cost = 0;
    rocket_pad_cost = 0;
    school_cost = 0;
    fire_cost = 0;
    cricket_cost = 0;
}

void init_lastyear(void)
{
    ly_income_tax = 0;
    ly_coal_tax = 0;
    ly_goods_tax = 0;
    ly_export_tax = 0;
    ly_import_cost = 0;
    ly_other_cost = 0;
    ly_unemployment_cost = 0;
    ly_transport_cost = 0;
    ly_university_cost = 0;
    ly_recycle_cost = 0;
    ly_school_cost = 0;
    ly_deaths_cost = 0;
    ly_health_cost = 0;
    ly_rocket_pad_cost = 0;
    ly_interest = 0;
    ly_windmill_cost = 0;
    ly_cricket_cost = 0;
    ly_fire_cost = 0;
}

void init_inventory(void)
{
    init_census();
    init_daily();
    init_monthly();
    init_yearly();
    init_lastyear();
}

void add_daily_to_monthly(void)
{
    ++data_last_month;

    tpopulation += population;
    thousing += housing;
    tstarving_population += starving_population;
    tunemployed_population += unemployed_population;
    tbirths += dbirths;
    tdeaths += ddeaths;
}

/** @file lincity/stats.cpp */

