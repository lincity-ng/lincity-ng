/* ---------------------------------------------------------------------- *
 * engglobs.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __engglobs_h__
#define __engglobs_h__

#include "lin-city.h"
#include "common.h"
#include "geometry.h"


/* GCS -- One of these days I will get this right. */
struct map_struct
{
    short type[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
    short group[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
    int pollution[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
    Map_Point_Info info[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
};
typedef struct map_struct Map;

extern Map map;

#define MP_TYPE(x,y)   map.type[x][y]
#define MP_GROUP(x,y)  map.group[x][y]
#define MP_POL(x,y)    map.pollution[x][y]
#define MP_INFO(x,y)   map.info[x][y]

#define MP_SIZE(x,y)   main_groups[MP_GROUP(x,y)].size
#define MP_COLOR(x,y)  main_groups[MP_GROUP(x,y)].colour
#define MP_GROUP_IS_RESIDENCE(x,y)  (GROUP_IS_RESIDENCE(MP_GROUP(x,y)))



extern int mappoint_array_x[WORLD_SIDE_LEN], mappoint_array_y[WORLD_SIDE_LEN];
extern int numof_shanties, numof_communes;
extern int last_built_x, last_built_y;

extern int sust_dig_ore_coal_tip_flag, sust_port_flag, sustain_flag;
extern int sust_dig_ore_coal_count, sust_port_count, sust_old_money;
extern int sust_old_money_count, sust_old_population, sust_old_population_count;
extern int sust_old_tech, sust_old_tech_count, sust_fire_count;

extern int total_time;    /* game time */

extern int population, starving_population, food_in_markets, jobs_in_markets;
extern int housed_population;
extern int coal_in_markets, goods_in_markets, ore_in_markets, steel_in_markets;
extern int unemployed_population, people_pool;
extern int substationx[MAX_NUMOF_SUBSTATIONS], substationy[MAX_NUMOF_SUBSTATIONS];
extern int numof_substations;
extern int marketx[MAX_NUMOF_MARKETS], markety[MAX_NUMOF_MARKETS], numof_markets;
extern int numof_health_centres, max_pop_ever, total_evacuated, total_births;

extern int income_tax, coal_tax, total_money, income_tax_rate, coal_tax_rate;
extern int unemployment_cost, transport_cost;
extern int other_cost, university_cost, recycle_cost, school_cost, windmill_cost;
extern int dole_rate, transport_cost_rate, cricket_cost;
extern int goods_tax, goods_tax_rate;
extern int export_tax, export_tax_rate, import_cost, import_cost_rate;
extern int tech_level, highest_tech_level, unnat_deaths, deaths_cost;
extern int health_cost, rocket_pad_cost, fire_cost;

extern int ly_income_tax;
extern int ly_coal_tax;
extern int ly_goods_tax;
extern int ly_export_tax;
extern int ly_other_cost;
extern int ly_unemployment_cost;
extern int ly_transport_cost;
extern int ly_import_cost;

extern int ly_fire_cost;
extern int ly_university_cost, ly_recycle_cost, ly_school_cost, ly_deaths_cost;
extern int ly_health_cost, ly_rocket_pad_cost, ly_interest, ly_windmill_cost;
extern int ly_cricket_cost;

extern int total_pollution_deaths, total_starve_deaths, total_unemployed_days;
extern int total_unemployed_years;
extern float pollution_deaths_history, starve_deaths_history;
extern float unemployed_history;

extern int university_intake_rate;
extern int power_made, power_used, coal_made, coal_used;
extern int goods_made, goods_used, ore_made, ore_used;
extern int rockets_launched, rockets_launched_success;
extern int coal_survey_done;

extern int selected_type_cost;

extern int ex_tax_dis[NUMOF_DISCOUNT_TRIGGERS];

#endif /* __engglobs_h__ */
