/* ---------------------------------------------------------------------- *
 * engglobs.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "engglobs.h"
#include "common.h"


Map map;

int mappoint_array_x[WORLD_SIDE_LEN];
int mappoint_array_y[WORLD_SIDE_LEN];
int numof_shanties;
int numof_communes;
int last_built_x, last_built_y;
int main_screen_originx, main_screen_originy;

int pix_double = 0;

#if defined (WIN32)
char windowsfontfile[256];
#endif

int cheat_flag;
char given_scene[256];
int select_button_help_flag[NUMOF_SELECT_BUTTONS];
int modern_windmill_flag = 0;

int sbut[NUMOF_SELECT_BUTTONS];
short selected_type;	/* the type associated with button selected */
int selected_button;	/* the button selected. So we can work out the cost */
int old_selected_button = 0;

int askdir_lines;
char *askdir_path[4];
char colour_pal_file[256], opening_pic[256], graphic_path[256], fontfile[256];
char opening_path[256];
char help_path[256], message_path[256];


int monthgraph_size = 0;
int *monthgraph_pop;
int *monthgraph_starve;
int *monthgraph_nojobs;
int *monthgraph_ppool;

#if defined (commentout)
int monthgraph_pop[MAPPOINT_STATS_W], monthgraph_starve[MAPPOINT_STATS_W];
int monthgraph_nojobs[MAPPOINT_STATS_W], monthgraph_ppool[MAPPOINT_STATS_W];
#endif

#if defined (commentout)
int diffgraph_power[MAPPOINT_STATS_W], diffgraph_coal[MAPPOINT_STATS_W];
int diffgraph_goods[MAPPOINT_STATS_W], diffgraph_ore[MAPPOINT_STATS_W];
int diffgraph_population[MAPPOINT_STATS_W];
int diff_old_population;
#endif

int pbar_pop[12], pbar_pop_oldtot, pop_diff, pbar_pop_olddiff;
int pbar_tech[12], pbar_tech_oldtot, tech_diff, pbar_tech_olddiff;
int pbar_food[12], pbar_food_oldtot, food_diff, pbar_food_olddiff;
int pbar_jobs[12], pbar_jobs_oldtot, jobs_diff, pbar_jobs_olddiff;
int pbar_coal[12], pbar_coal_oldtot, coal_diff, pbar_coal_olddiff;
int pbar_goods[12], pbar_goods_oldtot, goods_diff, pbar_goods_olddiff;
int pbar_ore[12], pbar_ore_oldtot, ore_diff, pbar_ore_olddiff;
int pbar_steel[12], pbar_steel_oldtot, steel_diff, pbar_steel_olddiff;
int pbar_money[12], pbar_money_oldtot, money_diff, pbar_money_olddiff;

int sust_dig_ore_coal_tip_flag = 0, sust_port_flag = 0, sustain_flag = 0;
int sust_dig_ore_coal_count = 0, sust_port_count = 0, sust_old_money = 0;
int sust_old_money_count = 0, sust_old_population = 0, sust_old_population_count = 0;
int sust_old_tech = 0, sust_old_tech_count = 0, sust_fire_count = 0;

int total_time = 0;    /* game time */

int population, starving_population, food_in_markets, jobs_in_markets;
int housed_population;
int coal_in_markets, goods_in_markets, ore_in_markets, steel_in_markets;
int unemployed_population, people_pool;
int substationx[MAX_NUMOF_SUBSTATIONS], substationy[MAX_NUMOF_SUBSTATIONS];
int numof_substations = 0;
int marketx[MAX_NUMOF_MARKETS], markety[MAX_NUMOF_MARKETS], numof_markets = 0;
int numof_health_centres, max_pop_ever = 0, total_evacuated = 0, total_births = 0;

int income_tax, coal_tax, total_money = 0, income_tax_rate, coal_tax_rate;
int unemployment_cost, transport_cost;
int other_cost, university_cost, recycle_cost, school_cost, windmill_cost;
int dole_rate, transport_cost_rate, cricket_cost;
int goods_tax, goods_tax_rate;
int export_tax, export_tax_rate, import_cost, import_cost_rate;
int tech_level, highest_tech_level, unnat_deaths, deaths_cost;
int health_cost, rocket_pad_cost, fire_cost, ly_fire_cost;

int total_pollution_deaths = 0, total_starve_deaths = 0, total_unemployed_days = 0;
int total_unemployed_years = 0;
float pollution_deaths_history = 0.0, starve_deaths_history = 0.0;
float unemployed_history = 0.0;

int university_intake_rate;
int power_made, power_used, coal_made, coal_used;
int goods_made, goods_used, ore_made, ore_used;
int rockets_launched, rockets_launched_success;
int coal_survey_done;

int selected_type_cost;

int ex_tax_dis[NUMOF_DISCOUNT_TRIGGERS] =
{
  EX_DISCOUNT_TRIGGER_1,
  EX_DISCOUNT_TRIGGER_2,
  EX_DISCOUNT_TRIGGER_3,
  EX_DISCOUNT_TRIGGER_4,
  EX_DISCOUNT_TRIGGER_5,
  EX_DISCOUNT_TRIGGER_6,
};


