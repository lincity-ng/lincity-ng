/* ---------------------------------------------------------------------- *
 * simulate.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "lcstring.h"
#include <sys/types.h>
#include <fcntl.h>

#include "shared_globals.h"
/*
#if defined (WIN32)
#include <winsock.h>
#include <io.h>
#include <direct.h>
#include <process.h>
#endif
*/
#include <ctype.h>
//#include "common.h"

/*
#ifdef LC_X11
#include <X11/cursorfont.h>
#endif
*/

#include "lctypes.h"
#include "lin-city.h"
//#include "cliglobs.h"
#include "engglobs.h"
#include "screen_interface.h"
#include "power.h"
#include "stats.h"
#include "pbar_interface.h"
//#include "module_buttons.h"

/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */
static void do_periodic_events (void);
static void end_of_month_update (void);
static void start_of_year_update (void);
static void end_of_year_update (void);
static void random_start (int* originx, int* originy);
static void simulate_mappoints (void);
static void quick_start_add (int x, int y, short type, int size);

/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
void
do_time_step (void)
{
    /* Increment game time */
    total_time++;
#ifdef DEBUG_ENGINE
    printf ("In do_time_step (%d)\n", total_time);
#endif

    /* Initialize daily accumulators */
    init_daily();

    /* Initialize monthly accumulators */
    if (total_time % NUMOF_DAYS_IN_MONTH == 0) {
	init_monthly();
    }

    /* Initialize yearly accumulators */
    if ((total_time % NUMOF_DAYS_IN_YEAR) == 0) {
	init_yearly();
    }

    /* Clear the power grid */
    power_time_step ();

    /* Run through simulation equations for each farm, residence, etc. */
    simulate_mappoints ();

    /* Now do the stuff that happens once a year, once a month, etc. */
    do_periodic_events ();
}

void 
clear_mappoint (short fill, int x, int y)
{
    MP_TYPE(x,y) = fill;
    MP_GROUP(x,y) = get_group_of_type(fill);
    if (MP_GROUP(x,y) < 0) MP_GROUP(x,y) = GROUP_BARE;
    MP_INFO(x,y).population = 0;
    MP_INFO(x,y).flags = 0;
    MP_INFO(x,y).int_1 = 0;
    MP_INFO(x,y).int_2 = 0;
    MP_INFO(x,y).int_3 = 0;
    MP_INFO(x,y).int_4 = 0;
    MP_INFO(x,y).int_5 = 0;
    MP_INFO(x,y).int_6 = 0;
    MP_INFO(x,y).int_7 = 0;
}

static void 
simulate_mappoints (void)
{
    int xx, yy;
    shuffle_mappoint_array ();
    for (yy = 0; yy < WORLD_SIDE_LEN; yy++) {
	/* indirection to rand array to stop lots of linear effects */
	int y = mappoint_array_y[yy];
	for (xx = 0; xx < WORLD_SIDE_LEN; xx++) {
	    int x = mappoint_array_x[xx];
	    short grp = MP_GROUP(x,y);
	    if (grp == GROUP_USED || grp == GROUP_BARE)
		continue;
	    switch (grp) {
	    case GROUP_TRACK:
		do_track (x, y);
		break;
	    case GROUP_RAIL:
		do_rail (x, y);
		break;
	    case GROUP_ROAD:
		do_road (x, y);
		break;
	    case GROUP_ORGANIC_FARM:
		do_organic_farm (x, y);
		break;
	    case GROUP_MARKET:
		do_market (x, y);
		break;
	    case GROUP_RESIDENCE_LL:
		do_residence (x, y);
		break;
	    case GROUP_RESIDENCE_ML:
		do_residence (x, y);
		break;
	    case GROUP_RESIDENCE_HL:
		do_residence (x, y);
		break;
	    case GROUP_RESIDENCE_LH:
		do_residence (x, y);
		break;
	    case GROUP_RESIDENCE_MH:
		do_residence (x, y);
		break;
	    case GROUP_RESIDENCE_HH:
		do_residence (x, y);
		break;
	    case GROUP_POWER_LINE:
	        do_power_line (x, y);
		break;
	    case GROUP_SOLAR_POWER:
		do_power_source (x, y);
		break;
	    case GROUP_SUBSTATION:
		do_power_substation (x, y);
		break;
	    case GROUP_COALMINE:
		do_coalmine (x, y);
		break;
	    case GROUP_COAL_POWER:
		do_power_source_coal (x, y);
		break;
	    case GROUP_INDUSTRY_L:
		do_industry_l (x, y);
		break;
	    case GROUP_INDUSTRY_H:
		do_industry_h (x, y);
		break;
	    case GROUP_COMMUNE:
		do_commune (x, y);
		break;
	    case GROUP_OREMINE:
		do_oremine (x, y);
		break;
	    case GROUP_PORT:
		do_port (x, y);
		break;
	    case GROUP_TIP:
		do_tip (x, y);
		break;
	    case GROUP_PARKLAND:
		do_parkland (x, y);
		break;
	    case GROUP_UNIVERSITY:
		do_university (x, y);
		break;
	    case GROUP_RECYCLE:
		do_recycle (x, y);
		break;
	    case GROUP_HEALTH:
		do_health_centre (x, y);
		break;
	    case GROUP_ROCKET:
		do_rocket_pad (x, y);
		break;
	    case GROUP_WINDMILL:
		do_windmill (x, y);
		break;
	    case GROUP_MONUMENT:
		do_monument (x, y);
		break;
	    case GROUP_SCHOOL:
		do_school (x, y);
		break;
	    case GROUP_BLACKSMITH:
		do_blacksmith (x, y);
		break;
	    case GROUP_MILL:
		do_mill (x, y);
		break;
	    case GROUP_POTTERY:
		do_pottery (x, y);
		break;
	    case GROUP_FIRESTATION:
		do_firestation (x, y);
		break;
	    case GROUP_CRICKET:
		do_cricket (x, y);
		break;
	    case GROUP_FIRE:
		do_fire (x, y);
		break;
	    case GROUP_SHANTY:
		do_shanty (x, y);
		break;
	    }
	}
    }
}

/* ---------------------------------------------------------------------- *
 * Private Functions
 * ---------------------------------------------------------------------- */
static void
do_periodic_events (void)
{
  add_daily_to_monthly();


  if ((total_time % NUMOF_DAYS_IN_YEAR) == 0) {
    start_of_year_update ();
  }
  if ((total_time % DAYS_BETWEEN_FIRES) == 9
      && tech_level > (GROUP_FIRESTATION_TECH * MAX_TECH_LEVEL / 1000)) {
    do_random_fire (-1, -1, 1);
  }
  if ((total_time % DAYS_BETWEEN_COVER) == 75) {
    clear_fire_health_and_cricket_cover ();
    do_fire_health_and_cricket_cover ();
  }
  if ((total_time % DAYS_BETWEEN_SHANTY) == 85
      && tech_level > (GROUP_HEALTH_TECH * MAX_TECH_LEVEL / 1000)) {
   update_shanty ();
  }
  if (total_time % NUMOF_DAYS_IN_MONTH == (NUMOF_DAYS_IN_MONTH - 1)) {
    end_of_month_update ();
  }
  if (total_time % NUMOF_DAYS_IN_YEAR == (NUMOF_DAYS_IN_YEAR - 1)) {
    end_of_year_update ();
  }
  if ((total_time % DAYS_PER_POLLUTION) == 3) {
    do_pollution ();
  }
}


static void 
end_of_month_update (void)
{
  /* GCS FIX -- seems to be a bit of engine code embedded in 
     do_monthgraph(), such as coal_made, coal_used, etc.
     Check it out soon... */
  housed_population = (tpopulation / NUMOF_DAYS_IN_MONTH);
  if ((housed_population + people_pool) > max_pop_ever)
    max_pop_ever = housed_population + people_pool;

  if (people_pool > 100) {
    if (rand () % 1000 < people_pool)
      people_pool -= 10;
  }
  if (people_pool < 0)
    people_pool = 0;

  if (tech_level > TECH_LEVEL_LOSS_START)
    {
      tech_level-=(int)(tech_level*(1./TECH_LEVEL_LOSS)
	*(1+(tpopulation
	     *(1./NUMOF_DAYS_IN_MONTH/120
	       /(TECH_LEVEL_LOSS-200)))));

    }
  else
    tech_level += TECH_LEVEL_UNAIDED;
  /* we can go over 100, but it's even more difficult */
  if (tech_level > MAX_TECH_LEVEL)
    tech_level-=(int)((tech_level-MAX_TECH_LEVEL)
      *(1./TECH_LEVEL_LOSS)
      *(1+(tpopulation
	   *(1./NUMOF_DAYS_IN_MONTH/120
	     /(TECH_LEVEL_LOSS-100)))));

  if (highest_tech_level < tech_level)
    highest_tech_level = tech_level;

  deaths_cost += unnat_deaths * UNNAT_DEATHS_COST;

}


static void 
start_of_year_update (void)
{
  int u;

  sustainability_test ();

  pollution_deaths_history
    -= pollution_deaths_history / 100.0;
  starve_deaths_history
    -= starve_deaths_history / 100.0;
  unemployed_history
    -= unemployed_history / 100.0;
  u = count_groups (GROUP_UNIVERSITY);
  if (u > 0) {
    university_intake_rate = (count_groups (GROUP_SCHOOL) * 20) / u;
    if (university_intake_rate > 100)
      university_intake_rate = 100;
  } else {
    university_intake_rate = 50;
  }

  map_power_grid();
}


static void 
end_of_year_update (void)
{
    income_tax = (income_tax * income_tax_rate) / 100;
    ly_income_tax = income_tax;
    total_money += income_tax;

    coal_tax = (coal_tax * coal_tax_rate) / 100;
    ly_coal_tax = coal_tax;
    total_money += coal_tax;

    goods_tax = (goods_tax * goods_tax_rate) / 100;
    goods_tax += (int) ((float) (goods_tax * goods_tax_rate)
			   * (float) tech_level / 2000000.0);
    ly_goods_tax = goods_tax;
    total_money += goods_tax;

    /* The price of exports on the world market drops as you export more.
       The exporters have to discount there wares, therefore the 
       tax take is less.
    */
    if (export_tax > ex_tax_dis[0])
    {
	int discount, disi;
	discount = 0;
	for (disi = 0; disi < NUMOF_DISCOUNT_TRIGGERS
		     && export_tax > ex_tax_dis[disi]; disi++)
	    discount += (export_tax - ex_tax_dis[disi]) / 10;
	export_tax -= discount;
    }
    ly_export_tax = export_tax;
    total_money += export_tax;

    ly_university_cost = university_cost;
    ly_recycle_cost = recycle_cost;
    ly_deaths_cost = deaths_cost;
    ly_health_cost = (health_cost * (tech_level / 10000)
		      * HEALTH_RUNNING_COST_MUL) / (MAX_TECH_LEVEL / 10000);
    ly_rocket_pad_cost = rocket_pad_cost;
    ly_school_cost = school_cost;
    ly_windmill_cost = windmill_cost;
    ly_fire_cost = (fire_cost * (tech_level / 10000)
		    * FIRESTATION_RUNNING_COST_MUL) / (MAX_TECH_LEVEL / 10000);
    ly_cricket_cost = cricket_cost;
    if (total_money < 0)
    {
	ly_interest = ((-total_money / 1000) * INTEREST_RATE);
	if (ly_interest > 1000000)
	    ly_interest = 1000000;
    }
    else
	ly_interest = 0;

    other_cost = university_cost + recycle_cost + deaths_cost
	    + ly_health_cost + rocket_pad_cost + school_cost
	    + ly_interest + windmill_cost + ly_fire_cost
	    + ly_cricket_cost;
    ly_other_cost = other_cost;
    total_money -= other_cost;

    unemployment_cost = (unemployment_cost * dole_rate) / 100;
    ly_unemployment_cost = unemployment_cost;
    total_money -= unemployment_cost;

    transport_cost = (transport_cost * transport_cost_rate) / 100;
    ly_transport_cost = transport_cost;
    total_money -= transport_cost;

    import_cost = (import_cost * import_cost_rate) / 100;
    ly_import_cost = import_cost;
    total_money -= import_cost;

    if (total_money > 2000000000)
	total_money = 2000000000;
    else if (total_money < -2000000000)
	total_money = -2000000000;

    print_total_money ();
}


static void
clear_game (void)
{
    int x, y;
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    clear_mappoint (CST_GREEN, x, y);
	    MP_POL(x,y) = 0;
	}
    }
    total_time = 0;
    coal_survey_done = 0;
    numof_shanties = 0;
    numof_communes = 0;
    numof_substations = 0;
    numof_health_centres = 0;
    numof_markets = 0;
    max_pop_ever = 0;
    total_evacuated = 0;
    total_births = 0;
    total_money = 0;
    tech_level = 0;
    init_inventory();
    update_avail_modules(0);
}

void
new_city (int* originx, int* originy, int random_village)
{
    clear_game ();
    coal_reserve_setup ();
    setup_river ();
    ore_reserve_setup ();
    init_pbars ();

    /* Initial population is 100 for empty board or 200 
       for random village (100 are housed). */
    people_pool = 100;

    if (random_village != 0) {
	random_start (originx, originy);
	update_pbar(PPOP,200,1); /* So pbars don't flash */
    } else {
	*originx = *originy = WORLD_SIDE_LEN/2 ;
	update_pbar(PPOP,100,1);
    }
    connect_transport (1,1,WORLD_SIDE_LEN-2,WORLD_SIDE_LEN-2);
    refresh_pbars ();
}

void
coal_reserve_setup (void)
{
  int i, j, x, y, xx, yy;
  for (i = 0; i < NUMOF_COAL_RESERVES / 5; i++)
    {
      x = (rand () % (WORLD_SIDE_LEN - 12)) + 6;
      y = (rand () % (WORLD_SIDE_LEN - 10)) + 6;
      do
	{
	  xx = (rand () % 3) - 1;
	  yy = (rand () % 3) - 1;
	}
      while (xx == 0 && yy == 0);
      for (j = 0; j < 5; j++)
	{
	  MP_INFO(x,y).coal_reserve
	    += rand () % COAL_RESERVE_SIZE;
	  x += xx;
	  y += yy;
	}
    }
}

void
ore_reserve_setup (void)
{
    int x, y;
    for (y = 0; y < WORLD_SIDE_LEN; y++)
	for (x = 0; x < WORLD_SIDE_LEN; x++)
	    MP_INFO(x,y).ore_reserve = ORE_RESERVE;
}

void
setup_river (void)
{
    int x, y, i, j;
    x = WORLD_SIDE_LEN / 2;
    y = WORLD_SIDE_LEN - 1;
    i = (rand () % 12) + 6;
    for (j = 0; j < i; j++) {
	x += (rand () % 3) - 1;
	MP_TYPE(x,y) = CST_WATER;
	MP_GROUP(x,y) = GROUP_WATER;
	MP_INFO(x,y).flags |= FLAG_IS_RIVER;
	MP_TYPE(x+1,y) = CST_WATER;
	MP_GROUP(x+1,y) = GROUP_WATER;
	MP_INFO(x+1,y).flags |= FLAG_IS_RIVER;
	MP_TYPE(x-1,y) = CST_WATER;
	MP_GROUP(x-1,y) = GROUP_WATER;
	MP_INFO(x-1,y).flags |= FLAG_IS_RIVER;
	y--;
    }
    MP_TYPE(x,y) = CST_WATER;
    MP_GROUP(x,y) = GROUP_WATER;
    MP_INFO(x,y).flags |= FLAG_IS_RIVER;
    MP_TYPE(x+1,y) = CST_WATER;
    MP_GROUP(x+1,y) = GROUP_WATER;
    MP_INFO(x+1,y).flags |= FLAG_IS_RIVER;
    MP_TYPE(x-1,y) = CST_WATER;
    MP_GROUP(x-1,y) = GROUP_WATER;
    MP_INFO(x-1,y).flags |= FLAG_IS_RIVER;

    setup_river2 (x - 1, y, -1);	/* left tributary */
    setup_river2 (x + 1, y, 1);	/* right tributary */
}

void
setup_river2 (int x, int y, int d)
{
    int i, j, r;
    i = (rand () % 55) + 15;
    for (j = 0; j < i; j++)
    {
	r = (rand () % 3) - 1 + (d * (rand () % 3));
	if (r < -1)
	    r = -1;
	else if (r > 1)
	    r = 1;
	x += r;
	if (MP_TYPE(x+(d+d),y) != 0
	    || MP_TYPE(x+(d+d+d),y) != 0)
	    return;
	if (x > 5 && x < WORLD_SIDE_LEN - 5)
	{
	    MP_TYPE(x,y) = CST_WATER;
	    MP_GROUP(x,y) = GROUP_WATER;
	    MP_INFO(x,y).flags |= FLAG_IS_RIVER;
	    MP_TYPE(x + d,y) = CST_WATER;
	    MP_GROUP(x+d,y) = GROUP_WATER;
	    MP_INFO(x+d,y).flags |= FLAG_IS_RIVER;
	}
	if (--y < 10 || x < 5 || x > WORLD_SIDE_LEN - 5)
	    break;
    }
    if (y > 20)
    {
	if (x > 5 && x < WORLD_SIDE_LEN - 5)
	    setup_river2 (x, y, -1);
	if (x > 5 && x < WORLD_SIDE_LEN - 5)
	    setup_river2 (x, y, 1);
    }
}

int
count_groups (int g)
{
  int x, y, i;
  i = 0;
  for (y = 0; y < WORLD_SIDE_LEN; y++)
    for (x = 0; x < WORLD_SIDE_LEN; x++)
      if (MP_GROUP(x,y) == g)
	i++;
  return (i);
}

void 
count_all_groups (int* group_count)
{
    int x, y;
    unsigned short t, g;
    for (x = 0; x < NUM_OF_GROUPS; x++)
	group_count[x] = 0;
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    t = MP_TYPE(x,y);
	    if (t != CST_USED && t != CST_GREEN) {
		g = get_group_of_type(t);
		group_count[g]++;
	    }
	}
    }
}

static void
random_start (int* originx, int* originy)
{
    int x, y, xx, yy, flag, watchdog;

    /* first find a place that has some water. */
    watchdog = 90;        /* if too many tries, random placement. */
    do {
	do {
	    xx = rand () % (WORLD_SIDE_LEN - 25);
	    yy = rand () % (WORLD_SIDE_LEN - 25);
	    flag = 0;
	    for (y = yy + 2; y < yy + 23; y++)
		for (x = xx + 2; x < xx + 23; x++)
		    if (MP_GROUP(x,y) == GROUP_WATER)
		    {
			flag = 1;
			x = xx + 23;   /* break out of loop */
			y = yy + 23;   /* break out of loop */
		    }
	} while (flag == 0 || (--watchdog) < 1);
	for (y = yy + 4; y < yy + 22; y++)
	    for (x = xx + 4; x < xx + 22; x++)
		if (MP_GROUP(x,y) != GROUP_BARE) {
		    flag = 0;
		    x = xx + 22;   /* break out of loop */
		    y = yy + 22;   /* break out of loop */
		}
    } while (flag == 0 || (--watchdog) < 1);

    /* These are going to be the main_screen_origin? vars */
    *originx = xx;
    *originy = yy;

    /*  Draw the start scene. */
    quick_start_add (xx + 5, yy + 5, CST_FARM_O0, 4);
    quick_start_add (xx + 9, yy + 6, CST_RESIDENCE_ML, 3);
    MP_INFO(xx + 9,yy + 6).population = 50;
    MP_INFO(xx + 9,yy + 6).flags |= (FLAG_FED + FLAG_EMPLOYED);
    quick_start_add (xx + 7, yy + 9, CST_MARKET_EMPTY, 2);
    marketx[numof_markets] = xx + 7;
    markety[numof_markets] = yy + 9;
    numof_markets++;
    /* Bootstap markets with some stuff. */
    MP_INFO(xx + 7,yy + 9).int_1 = 2000;
    MP_INFO(xx + 7,yy + 9).int_2 = 10000;
    MP_INFO(xx + 7,yy + 9).int_3 = 100;
    MP_INFO(xx + 7,yy + 9).int_5 = 10000;
    MP_INFO(xx + 7,yy + 9).flags 
	    |= (FLAG_MB_FOOD + FLAG_MS_FOOD + FLAG_MB_JOBS
		+ FLAG_MS_JOBS + FLAG_MB_COAL + FLAG_MS_COAL + FLAG_MB_ORE
		+ FLAG_MS_ORE + FLAG_MB_GOODS + FLAG_MS_GOODS + FLAG_MB_STEEL
		+ FLAG_MS_STEEL);


    quick_start_add (xx + 14, yy + 6, CST_RESIDENCE_ML, 3);
    MP_INFO(xx + 14,yy + 6).population = 50;
    MP_INFO(xx + 14,yy + 6).flags |= (FLAG_FED + FLAG_EMPLOYED);
    quick_start_add (xx + 17, yy + 5, CST_FARM_O0, 4);
    quick_start_add (xx + 17, yy + 9, CST_MARKET_EMPTY, 2);
    marketx[numof_markets] = xx + 17;
    markety[numof_markets] = yy + 9;
    numof_markets++;
    MP_INFO(xx + 17,yy + 9).int_1 = 2000;
    MP_INFO(xx + 17,yy + 9).int_2 = 8000;
    MP_INFO(xx + 17,yy + 9).flags 
	    |= (FLAG_MB_FOOD + FLAG_MS_FOOD + FLAG_MB_JOBS
		+ FLAG_MS_JOBS + FLAG_MB_COAL + FLAG_MS_COAL + FLAG_MB_ORE
		+ FLAG_MS_ORE + FLAG_MB_GOODS + FLAG_MS_GOODS + FLAG_MB_STEEL
		+ FLAG_MS_STEEL);

    for (x = 5; x < 19; x++)
    {
	quick_start_add (xx + x, yy + 11, CST_TRACK_LR, 1);
	MP_INFO(xx + x,yy + 11).flags |= FLAG_IS_TRANSPORT;
    }
    for (y = 12; y < 18; y++)
    {
	quick_start_add (xx + 5, yy + y, CST_TRACK_LR, 1);
	MP_INFO(xx + 5,yy + y).flags |= FLAG_IS_TRANSPORT;
    }
    quick_start_add (xx + 6, yy + 12, CST_COMMUNE_1, 4);
    quick_start_add (xx + 6, yy + 17, CST_COMMUNE_1, 4);
    quick_start_add (xx + 11, yy + 12, CST_COMMUNE_1, 4);
    quick_start_add (xx + 11, yy + 17, CST_COMMUNE_1, 4);
    quick_start_add (xx + 16, yy + 12, CST_COMMUNE_1, 4);
    quick_start_add (xx + 16, yy + 17, CST_COMMUNE_1, 4);
    for (x = 6; x < 17; x++)
    {
	quick_start_add (xx + x, yy + 16, CST_TRACK_LR, 1);
	MP_INFO(xx + x,yy + 16).flags |= FLAG_IS_TRANSPORT;
    }
    for (y = 12; y < 16; y++)
    {
	quick_start_add (xx + 10, yy + y, CST_TRACK_LR, 1);
	MP_INFO(xx + 10,yy + y).flags |= FLAG_IS_TRANSPORT;
	quick_start_add (xx + 15, yy + y, CST_TRACK_LR, 1);
	MP_INFO(xx + 15,yy + y).flags |= FLAG_IS_TRANSPORT;
    }
    quick_start_add (xx + 10, yy + 17, CST_TRACK_LR, 1);
    MP_INFO(xx + 10,yy + 17).flags |= FLAG_IS_TRANSPORT;
    quick_start_add (xx + 15, yy + 17, CST_TRACK_LR, 1);
    MP_INFO(xx + 15,yy + 17).flags |= FLAG_IS_TRANSPORT;

    quick_start_add (xx + 9, yy + 9, CST_POTTERY_0, 2);
}

/* XXX: WCK: What is up with this?  Why not just use set_mappoint?! */
static void
quick_start_add (int x, int y, short type, int size)
{
  int xx, yy;
  if (size == 1) {
      MP_TYPE(x,y) = type;
      MP_GROUP(x,y) = get_group_of_type(type);
      return;
  }
  for (yy = 0; yy < size; yy++) {
    for (xx = 0; xx < size; xx++) {
	if (xx == 0 && yy == 0)
	  continue;
	set_mappoint_used (x, y, x + xx, y + yy);
      }
  }
  MP_TYPE(x,y) = type;
  MP_GROUP(x,y) = get_group_of_type(type);
}

void
sustainability_test (void)
{
  int i;
  if (sust_dig_ore_coal_tip_flag == 0)
    {
      sust_dig_ore_coal_tip_flag = 1;
      sust_dig_ore_coal_count = 0;
    }
  else
    sust_dig_ore_coal_count++;

  if (sust_port_flag == 0)
    {
      sust_port_flag = 1;
      sust_port_count = 0;
    }
  else
    sust_port_count++;

  /* Money must be going up or the same. (ie can't build.) */
  if (sust_old_money > total_money)
    sust_old_money_count = 0;
  else
    sust_old_money_count++;
  sust_old_money = total_money;

  /* population must be withing 2% of when it started. */
  i = (housed_population + people_pool) - sust_old_population;
  if (abs (i) > (sust_old_population / 40)	/* 2.5%  */
      || (housed_population + people_pool) < SUST_MIN_POPULATION)
    {
      sust_old_population = (housed_population + people_pool);
      sust_old_population_count = 0;
    }
  else
    sust_old_population_count++;

  /* tech level must be going up or not fall more than 0.5% from it's
     highest during the sus count
  */
  i = tech_level - sust_old_tech;
  if (i < 0 || tech_level < SUST_MIN_TECH_LEVEL)
    {
      i = -i;
      if ((i > sust_old_tech / 100) || tech_level < SUST_MIN_TECH_LEVEL)
	{
	  sust_old_tech_count = 0;
	  sust_old_tech = tech_level;
	}
      else
	sust_old_tech_count++;
    }
  else
    {
      sust_old_tech_count++;
      sust_old_tech = tech_level;
    }

  /* check fire cover only every three years */
  if (total_time % (NUMOF_DAYS_IN_YEAR * 3) == 0)
    {
      if (sust_fire_cover () != 0)
	sust_fire_count += 3;
      else
	sust_fire_count = 0;

    }
}

int
sust_fire_cover (void)
{
  int x, y;
  for (x = 0; x < WORLD_SIDE_LEN; x++)
    for (y = 0; y < WORLD_SIDE_LEN; y++)
      {
	if (MP_GROUP(x,y) == GROUP_BARE
	    || MP_TYPE(x,y) == CST_USED
	    || MP_GROUP(x,y) == GROUP_WATER
	    || MP_GROUP(x,y) == GROUP_POWER_LINE
	    || MP_GROUP(x,y) == GROUP_OREMINE
	    || MP_GROUP(x,y) == GROUP_ROCKET
	    || MP_GROUP(x,y) == GROUP_MONUMENT
	    || MP_GROUP(x,y) == GROUP_BURNT)
	  ;			/* do nothing */

	else if ((MP_INFO(x,y).flags & FLAG_FIRE_COVER) == 0)
	  return (0);
      }
  return (1);
}

void 
debug_mappoints (void)
{
  int x, y;
  for (x = 0; x < WORLD_SIDE_LEN; x++) {
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
      if ((MP_TYPE(x,y) < 0) || (MP_TYPE(x,y) > 400)) {
	printf ("Error in mappoint %d %d (%d)\n", x, y, MP_TYPE(x,y));
	exit(-1);
      }
    }
  }
}

void
initialize_tax_rates (void)
{
  income_tax_rate = INCOME_TAX_RATE;
  coal_tax_rate = COAL_TAX_RATE;
  goods_tax_rate = GOODS_TAX_RATE;
  dole_rate = DOLE_RATE;
  transport_cost_rate = TRANSPORT_COST_RATE;
  import_cost_rate = IM_PORT_COST_RATE;
}

