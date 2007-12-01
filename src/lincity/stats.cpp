/* ---------------------------------------------------------------------- *
 * ldsvguts.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2002.
 * Portions copyright (c) Corey Keasling, 2000-2002.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>

#include "lin-city.h"
#include "engglobs.h"
#include "gui_interface/pbar_interface.h"
#include "stats.h"

/* ---------------------------------------------------------------------- *
 * Public Global Variables
 * ---------------------------------------------------------------------- */

/* Daily accumulators */
int food_in_markets;
int jobs_in_markets;
int coal_in_markets;
int goods_in_markets;
int ore_in_markets;
int steel_in_markets;
int waste_in_markets;

/* Monthly accumulators */
int tfood_in_markets;
int tjobs_in_markets;
int tcoal_in_markets;
int tgoods_in_markets;
int tore_in_markets;
int tsteel_in_markets;
int twaste_in_markets;
int tpopulation;
int tstarving_population;
int tunemployed_population;

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

/* Averaging variables */
int data_last_month;

/* ---------------------------------------------------------------------- *
 * Private Function Prototypes
 * ---------------------------------------------------------------------- */
void inventory_market(int x, int y);

void init_daily(void)
{
    population = 0;
    starving_population = 0;
    unemployed_population = 0;
    food_in_markets = 0;
    jobs_in_markets = 0;
    coal_in_markets = 0;
    goods_in_markets = 0;
    ore_in_markets = 0;
    steel_in_markets = 0;
}

void init_monthly(void)
{
    data_last_month = 0;

    tpopulation = 0;
    tstarving_population = 0;
    tfood_in_markets = 0;
    tjobs_in_markets = 0;
    tcoal_in_markets = 0;
    tgoods_in_markets = 0;
    tore_in_markets = 0;
    tsteel_in_markets = 0;
    tunemployed_population = 0;
    unnat_deaths = 0;
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

void inventory(int x, int y)
{

    switch (get_group_of_type(MP_TYPE(x, y))) {

    case GROUP_MARKET:
        inventory_market(x, y);
        break;

    default:{
            printf("Default in inventory(%d,%d): got %d\n", x, y, get_group_of_type(MP_TYPE(x, y)));
            break;
        }

    }
}

void init_inventory(void)
{
    init_daily();
    init_monthly();
    init_yearly();
    init_lastyear();
}

void inventory_market(int x, int y)
{
    food_in_markets += MP_INFO(x, y).int_1;
    jobs_in_markets += MP_INFO(x, y).int_2;
    coal_in_markets += MP_INFO(x, y).int_3;
    goods_in_markets += MP_INFO(x, y).int_4;
    ore_in_markets += MP_INFO(x, y).int_5;
    steel_in_markets += MP_INFO(x, y).int_6;
    waste_in_markets += MP_INFO(x, y).int_7;
}

/* XXX: WCK:  Why oh why must we divide by arbitrary values, below? */
/* If this is fixed, make sure to fix it in pbar also! */

void add_daily_to_monthly(void)
{
    data_last_month++;

    tpopulation += population;
    tstarving_population += starving_population;
    tfood_in_markets += food_in_markets / 1000;
    tjobs_in_markets += jobs_in_markets / 1000;
    tcoal_in_markets += coal_in_markets / 250;
    tgoods_in_markets += goods_in_markets / 500;
    tore_in_markets += ore_in_markets / 500;
    tsteel_in_markets += steel_in_markets / 25;
    twaste_in_markets += waste_in_markets;
    tunemployed_population += unemployed_population;
}
