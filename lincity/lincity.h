#ifndef _LINCITY_LINCITY_H_
#define _LINCITY_LINCITY_H_

/* simulate.c */
void do_time_step (void);
void count_all_groups (int* group_count);

/* daily */
extern int food_in_markets; 
extern int jobs_in_markets; 
extern int coal_in_markets; 
extern int goods_in_markets; 
extern int ore_in_markets; 
extern int steel_in_markets; 
extern int waste_in_markets; 

/* monthly */
extern int tfood_in_markets; 
extern int tjobs_in_markets; 
extern int tcoal_in_markets; 
extern int tgoods_in_markets; 
extern int tore_in_markets; 
extern int tsteel_in_markets; 
extern int twaste_in_markets;
extern int tpopulation; 
extern int tstarving_population; 
extern int tunemployed_population; 
extern int twaste_in_markets;

/* yearly */
extern int income_tax; 
extern int coal_tax; 
extern int goods_tax; 
extern int export_tax; 
extern int import_cost;
extern int unemployment_cost; 
extern int transport_cost; 
extern int windmill_cost; 
extern int university_cost; 
extern int recycle_cost; 
extern int deaths_cost; 
extern int health_cost; 
extern int rocket_pad_cost; 
extern int school_cost; 
extern int fire_cost; 
extern int cricket_cost; 
extern int other_cost;

/* yearly */
extern int ly_income_tax; 
extern int ly_coal_tax; 
extern int ly_goods_tax; 
extern int ly_export_tax; 
extern int ly_import_cost; 
extern int ly_other_cost; 
extern int ly_unemployment_cost; 
extern int ly_transport_cost; 
extern int ly_fire_cost; 
extern int ly_university_cost; 
extern int ly_recycle_cost; 
extern int ly_school_cost; 
extern int ly_deaths_cost; 
extern int ly_health_cost; 
extern int ly_rocket_pad_cost; 
extern int ly_interest; 
extern int ly_windmill_cost;
extern int ly_cricket_cost;

/* Averaging variables */
extern int data_last_month;

#endif
