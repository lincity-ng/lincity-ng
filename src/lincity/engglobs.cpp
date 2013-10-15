/* ---------------------------------------------------------------------- *
 * engglobs.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "engglobs.h"
#include "lin-city.h"
#include "fileutil.h"

/* load/save version for compatibility with(out) waterwell */
int ldsv_version;
int use_waterwell = true;

ConstructionCount constructionCount = ConstructionCount();
World world = World(WORLD_SIDE_LEN);
int world_id = 0;
UserOperation* userOperation = NULL;

/* Vector for visiting neigbours = ( dx[k] , dy[k] )  ; ordered so that diagonal moves are the last 4 */
const int dx[8] = { -1, 0, 1, 0, 1, 1, -1, -1};
const int dy[8] = { 0, -1, 0, 1, 1, -1, 1, -1};
//anti clockwise ordering
const int dxo[8] ={ -1, -1,  0,  1,  1,  1,  0, -1};
const int dyo[8] ={  0, -1, -1, -1,  0,  1,  1,  1};

bool binary_mode = true;
bool seed_compression = true;
//You may want to set these to false for easier debugging

//These have to be decalred as extern in lintypes.h after class Construction
std::map<Construction::Commodities, int> tstat_capacities;
std::map<Construction::Commodities, int> tstat_census;

int main_screen_originx, main_screen_originy;

int pix_double = 0;

/* graph stuff from src/gui_interface/shared_globals.h */
int cheat_flag;
int modern_windmill_flag = 0;

int monthgraph_size = 0;
int *monthgraph_pop;
int *monthgraph_starve;
int *monthgraph_nojobs;
int *monthgraph_ppool;

/* end of gui_interface.... */

int sust_dig_ore_coal_tip_flag = 0, sust_port_flag = 0, sustain_flag = 0;
int sust_dig_ore_coal_count = 0, sust_port_count = 0, sust_old_money = 0;
int sust_old_money_count = 0, sust_old_population = 0, sust_old_population_count = 0;
int sust_old_tech = 0, sust_old_tech_count = 0, sust_fire_count = 0;

bool refresh_cover = false;
int total_time = 0;             /* game time */
int deadline = 0;

int population, starving_population;
int housed_population, total_housing, housing;
int unemployed_population, people_pool;
int max_pop_ever = 0, total_evacuated = 0, total_births = 0;
int total_population = 0;

int total_money = 0, income_tax_rate, coal_tax_rate;
int dole_rate, transport_cost_rate;
int goods_tax_rate;
int export_tax_rate, import_cost_rate;
int tech_level, highest_tech_level, unnat_deaths;
int total_pollution = 0;

int total_pollution_deaths = 0, total_starve_deaths = 0, total_unemployed_days = 0;
int total_unemployed_years = 0;
float pollution_deaths_history = 0.0, starve_deaths_history = 0.0;
float unemployed_history = 0.0;

//int university_intake_rate;
int power_made, power_used, coal_made, coal_used;
int goods_made, goods_used, ore_made, ore_used;
int rockets_launched, rockets_launched_success;
int coal_survey_done;

int selected_type_cost;

int ex_tax_dis[NUMOF_DISCOUNT_TRIGGERS] = {
    EX_DISCOUNT_TRIGGER_1,
    EX_DISCOUNT_TRIGGER_2,
    EX_DISCOUNT_TRIGGER_3,
    EX_DISCOUNT_TRIGGER_4,
    EX_DISCOUNT_TRIGGER_5,
    EX_DISCOUNT_TRIGGER_6,
};


int  global_aridity;
int  global_mountainity;
//TODO These and their updating should go to class World
int alt_min, alt_max, alt_step;

int fast_time_for_year;

/** @file lincity/engglobs.cpp */

