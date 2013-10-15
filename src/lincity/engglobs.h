/* ---------------------------------------------------------------------- *
 * engglobs.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __engglobs_h__
#define __engglobs_h__

#include "lintypes.h"
#include "groups.h"
#include "world.h"
#include "ConstructionCount.h"
#include "UserOperation.h"
// Use permutator to shuffle the simulation order
//#include "lincity-ng/Permutator.hpp"

class World;
class ConstructionCount;
class UserOperation;

extern int pix_double;

extern int use_waterwell, ldsv_version;

extern struct GROUP main_groups[NUM_OF_GROUPS];
extern struct TYPE main_types[NUM_OF_TYPES];

extern ConstructionCount constructionCount;
extern World world;
extern int world_id;
extern UserOperation* userOperation;

/* Vector for visiting neigbours = ( dx[k] , dy[k] )  ; ordered so that diagonal moves are the last 4 */
extern const int dx[8];
extern const int dy[8];
//anti clockwise ordering
extern const int dxo[8];
extern const int dyo[8];

extern bool binary_mode;
extern bool seed_compression;
extern bool refresh_cover;

extern int global_aridity, global_mountainity;
extern int alt_min, alt_max, alt_step;

extern int sust_dig_ore_coal_tip_flag, sust_port_flag, sustain_flag;
extern int sust_dig_ore_coal_count, sust_port_count, sust_old_money;
extern int sust_old_money_count, sust_old_population, sust_old_population_count;
extern int sust_old_tech, sust_old_tech_count, sust_fire_count;

extern int total_time;  // game time
extern int deadline;    // + 10 years to upgrade with waterwell everywhere
extern int flag_warning;// flag to send a message to player.

extern int population, starving_population;
extern int housed_population, total_housing, housing;
extern int unemployed_population, people_pool;
extern int max_pop_ever, total_evacuated, total_births;
extern int total_pollution;

extern int total_money, income_tax_rate, coal_tax_rate;
extern int dole_rate, transport_cost_rate;
extern int goods_tax_rate;
extern int export_tax_rate, import_cost_rate;
extern int tech_level, highest_tech_level, unnat_deaths;

extern int total_pollution_deaths, total_starve_deaths, total_unemployed_days;
extern int total_unemployed_years;
extern float pollution_deaths_history, starve_deaths_history;
extern float unemployed_history;

//extern int university_intake_rate;
extern int power_made, power_used, coal_made, coal_used;
extern int goods_made, goods_used, ore_made, ore_used;
extern int rockets_launched, rockets_launched_success;
extern int coal_survey_done;

extern int selected_type_cost;

extern int ex_tax_dis[NUMOF_DISCOUNT_TRIGGERS];

extern int fast_time_for_year;
#endif /* __engglobs_h__ */

/** @file lincity/engglobs.h */

