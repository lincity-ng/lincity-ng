/* ---------------------------------------------------------------------- *
 * engine.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"
#include "lctypes.h"
#include "lin-city.h"
#include "engine.h"
#include "engglobs.h"
#include "cliglobs.h"
#include "simulate.h"
#include "lcintl.h"
#include "power.h"
#include "mouse.h"
#include "module_buttons.h"
#include "pbar.h"
#include "stats.h"
#include "screen.h"
#include "dialbox.h"
#include "mps.h"

extern int selected_type_cost;

int 
adjust_money(int value)
{
    total_money += value;
    print_total_money();
    mps_update();
    update_pbar (PMONEY, total_money, 0);
    refresh_pbars(); /* This could be more specific */
    return total_money;
}

int is_real_river (int x, int y);

int
no_credit_build (int selected_group)
{
  if (total_money >= 0)
    return (0);

#ifdef GROUP_POWER_SOURCE_NO_CREDIT
  if (selected_group == GROUP_POWER_SOURCE) {
    return (1);
  }
#endif
#ifdef GROUP_UNIVERSITY_NO_CREDIT
  if (selected_group == GROUP_UNIVERSITY) {
    return (1);
  }
#endif
#ifdef GROUP_PARKLAND_NO_CREDIT
  if (selected_group == GROUP_PARKLAND) {
    return (1);
  }
#endif
#ifdef GROUP_RECYCLE_NO_CREDIT
  if (selected_group == GROUP_RECYCLE) {
    return (1);
  }
#endif
#ifdef GROUP_ROCKET
  if (selected_group == GROUP_ROCKET) {
    return (1);
  }
#endif

  if (main_groups[selected_group].no_credit == TRUE ) {
    return (1);
  }
  return (0);
}

int 
place_item (int x, int y, short type)
{
    int group;
    int size;

    group = get_group_of_type(type);
    if (group < 0) return -1;

    size = main_groups[group].size;

    /* You can't build because credit not available. */
    if (no_credit_build (group) != 0) {
	return -1;
    }

    /* Not enough slots in the substation array */

    switch (group) {
    case GROUP_SUBSTATION:
    case GROUP_WINDMILL:
    {
	if (add_a_substation (x, y) == 0)
	    return -3;
    } break;
    case GROUP_PORT:
    {
	if (is_real_river (x + 4, y) != 1 
	    || is_real_river (x + 4, y + 1) != 1
	    || is_real_river (x + 4, y + 2) != 1 
	    || is_real_river (x + 4, y + 3) != 1) {
	    return -2;
	}
    } break;
    case GROUP_COMMUNE:
    {
	numof_communes++;
    } break;
    case GROUP_MARKET:
    {
	/* Test for enough slots in the market array */
	if (add_a_market (x, y) == 0)
	    return -3;
	MP_INFO(x,y).flags += (FLAG_MB_FOOD | FLAG_MB_JOBS
			       | FLAG_MB_COAL | FLAG_MB_ORE | FLAG_MB_STEEL
			       | FLAG_MB_GOODS | FLAG_MS_FOOD | FLAG_MS_JOBS
			       | FLAG_MS_COAL | FLAG_MS_GOODS | FLAG_MS_ORE
			       | FLAG_MS_STEEL);
    } break;
    case GROUP_TIP:
    {
	/* Don't build a tip if there has already been one.  If we succeed,
	   mark the spot permanently by "doubling" the ore reserve */
	int i,j;
	int prev_tip = 0;
	for (i=0;i<3;i++) {
	    for (j=0;j<3;j++) {
		if (MP_INFO(x+i,y+j).ore_reserve > ORE_RESERVE) {
		    prev_tip = 1;
		    break;
		}
	    }
	}
	if (prev_tip) {
	    dialog_box(red(12),3,
		       0,0,_("You can't build a tip here"),
		       0,0,_("This area was once a landfill"),
		       2,' ',_("OK"));
	    return -4;
	} else {
	    for (i=0;i<3;i++) {
		for (j=0;j<3;j++) {
		    MP_INFO(x+i,y+j).ore_reserve = ORE_RESERVE * 2;
		}
	    }
	}
    } break;
    case GROUP_OREMINE:
    {
	/* Don't allow new mines on old mines or old tips */
	/* GCS: mines over old mines is OK if there is enough remaining 
	        ore, as is the case when there is partial overlap. */
	int i,j;
	int prev_tip = 0;
	int total_ore = 0;
	for (i=0;i<3;i++) {
	    for (j=0;j<3;j++) {
		total_ore += MP_INFO(x+i,y+j).ore_reserve;
		if (MP_INFO(x+i,y+j).ore_reserve > ORE_RESERVE) {
		    prev_tip = 1;
		    break;
		}
	    }
	}
	if (prev_tip) {
	    dialog_box(red(12),3,
		       0,0,_("You can't build a mine here"),
		       0,0,_("This area was once a landfill"),
		       2,' ',_("OK"));
	    return -4;
	}
	if (total_ore < MIN_ORE_RESERVE_FOR_MINE) {
	    dialog_box(red(12),3,
		       0,0,_("You can't build a mine here"),
		       0,0,_("There is no ore left at this site"),
		       2,' ',_("OK"));
	    return -4;
	}
    }
    } /* end case */

    /* Store last_built for refund on "mistakes" */
    last_built_x = x;
    last_built_y = y;

    /* Make sure that the correct windmill graphic shows up */
    if (group == GROUP_WINDMILL) {
	if (tech_level > MODERN_WINDMILL_TECH) {
	    type = CST_WINDMILL_1_R;
	} else {
	    type = CST_WINDMILL_1_W;
	}
    }

    if (group == GROUP_SOLAR_POWER || group == GROUP_WINDMILL) {
	MP_INFO(x,y).int_2 = tech_level;
	let_one_through = 1;
    }
    else if (group == GROUP_RECYCLE || group == GROUP_COAL_POWER)
	MP_INFO(x,y).int_4 = tech_level;
    else if (group == GROUP_ORGANIC_FARM)
	MP_INFO(x,y).int_1 = tech_level;
    else if (group == GROUP_TRACK
	     || group == GROUP_ROAD
	     || group == GROUP_RAIL)
	MP_INFO(x,y).flags |= FLAG_IS_TRANSPORT;
    else if (group == GROUP_COALMINE
	     || group == GROUP_OREMINE)
	let_one_through = 1;

    set_mappoint (x, y, type);

    update_tech_dep (x, y);

    if (group == GROUP_RIVER)
	connect_rivers ();

    connect_transport (x-2,y-2,x+size+1,y+size+1);

    adjust_money(-selected_module_cost);
    map_power_grid();
    return 0;
}

int 
bulldoze_item (int x, int y)
{
    int g, size;

    if (MP_TYPE(x,y) == CST_USED) {
	/* This is considered "improper" input.  Silently ignore. */
	return -1;
    }

    size = MP_SIZE(x,y);
    g = MP_GROUP(x,y);

    if (g == GROUP_BARE) {
	/* Nothing to do. */
	return -1;
    }
    else if (g == GROUP_SHANTY) {
	fire_area (x, y);
	adjust_money(-GROUP_SHANTY_BUL_COST);
    }
    else if (g == GROUP_FIRE) {
	if (MP_INFO(x,y).int_2 >= FIRE_LENGTH)
	    return -1;  /* Can't bulldoze ? */
	MP_INFO(x,y).int_2 = FIRE_LENGTH + 1;
	MP_TYPE(x,y) = CST_FIRE_DONE1;
	MP_GROUP(x,y) = GROUP_BURNT;
	adjust_money(-GROUP_BURNT_BUL_COST);
    }
    else {
	adjust_money(-main_groups[g].bul_cost);
	do_bulldoze_area (CST_GREEN, x, y);
	if (g == GROUP_OREMINE)
	{
	    int i, j;
	    for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++)
		    if (MP_INFO(x + i,y + j).ore_reserve < ORE_RESERVE / 2)
			do_bulldoze_area (CST_WATER, x + i, y + j);
	}
    }
    return size;  /* No longer used... */
}

void
init_mappoint_array (void)
{
    int x;
    for (x = 0; x < WORLD_SIDE_LEN; x++) {
	mappoint_array_x[x] = x;
	mappoint_array_y[x] = x;
    }
}

void
shuffle_mappoint_array (void)
{
  int i, x, a;
  for (i = 0; i < SHUFFLE_MAPPOINT_COUNT; i++)
    {
      x = rand () % WORLD_SIDE_LEN;
      a = mappoint_array_x[i];
      mappoint_array_x[i] = mappoint_array_x[x];
      mappoint_array_x[x] = a;
      x = rand () % WORLD_SIDE_LEN;
      a = mappoint_array_y[i];
      mappoint_array_y[i] = mappoint_array_y[x];
      mappoint_array_y[x] = a;
    }
}


int
buy_food (int xt, int yt)
{
  int i = 0;
  if (MP_GROUP(xt,yt) == GROUP_TRACK)
    {
      if (MP_INFO(xt,yt).int_1 < MAX_FOOD_ON_TRACK)
	i = MAX_FOOD_ON_TRACK - MP_INFO(xt,yt).int_1;
    }
  else if (MP_GROUP(xt,yt) == GROUP_ROAD)
    {
      if (MP_INFO(xt,yt).int_1 < MAX_FOOD_ON_ROAD)
	i = MAX_FOOD_ON_ROAD - MP_INFO(xt,yt).int_1;
    }
  else if (MP_GROUP(xt,yt) == GROUP_RAIL)
    {
      if (MP_INFO(xt,yt).int_1 < MAX_FOOD_ON_RAIL)
	i = MAX_FOOD_ON_RAIL - MP_INFO(xt,yt).int_1;
    }
  i = (i * PORT_IMPORT_RATE) / 1000;
  MP_INFO(xt,yt).int_1 += i;
  return (i * PORT_FOOD_RATE);
}

int
buy_coal (int xt, int yt)
{
  int i = 0;
  if (MP_GROUP(xt,yt) == GROUP_TRACK)
    {
      if (MP_INFO(xt,yt).int_3 < MAX_COAL_ON_TRACK)
	i = MAX_COAL_ON_TRACK - MP_INFO(xt,yt).int_3;
    }
  else if (MP_GROUP(xt,yt) == GROUP_ROAD)
    {
      if (MP_INFO(xt,yt).int_3 < MAX_COAL_ON_ROAD)
	i = MAX_COAL_ON_ROAD - MP_INFO(xt,yt).int_3;
    }
  else if (MP_GROUP(xt,yt) == GROUP_RAIL)
    {
      if (MP_INFO(xt,yt).int_3 < MAX_COAL_ON_RAIL)
	i = MAX_COAL_ON_RAIL - MP_INFO(xt,yt).int_3;
    }
  i = (i * PORT_IMPORT_RATE) / 1000;
  MP_INFO(xt,yt).int_3 += i;
  return (i * PORT_COAL_RATE);
}

int
buy_ore (int xt, int yt)
{
  int i = 0;
  if (MP_GROUP(xt,yt) == GROUP_TRACK)
    {
      if (MP_INFO(xt,yt).int_5 < MAX_ORE_ON_TRACK)
	i = MAX_ORE_ON_TRACK - MP_INFO(xt,yt).int_5;
    }
  else if (MP_GROUP(xt,yt) == GROUP_ROAD)
    {
      if (MP_INFO(xt,yt).int_5 < MAX_ORE_ON_ROAD)
	i = MAX_ORE_ON_ROAD - MP_INFO(xt,yt).int_5;
    }
  else if (MP_GROUP(xt,yt) == GROUP_RAIL)
    {
      if (MP_INFO(xt,yt).int_5 < MAX_ORE_ON_RAIL)
	i = MAX_ORE_ON_RAIL - MP_INFO(xt,yt).int_5;
    }
  i = (i * PORT_IMPORT_RATE) / 1000;
  MP_INFO(xt,yt).int_5 += i;
  return (i * PORT_ORE_RATE);
}

int
buy_goods (int xt, int yt)
{
  int i = 0;
  if (MP_GROUP(xt,yt) == GROUP_TRACK)
    {
      if (MP_INFO(xt,yt).int_4 < MAX_GOODS_ON_TRACK)
	i = MAX_GOODS_ON_TRACK - MP_INFO(xt,yt).int_4;
    }
  else if (MP_GROUP(xt,yt) == GROUP_ROAD)
    {
      if (MP_INFO(xt,yt).int_4 < MAX_GOODS_ON_ROAD)
	i = MAX_GOODS_ON_ROAD - MP_INFO(xt,yt).int_4;
    }
  else if (MP_GROUP(xt,yt) == GROUP_RAIL)
    {
      if (MP_INFO(xt,yt).int_4 < MAX_GOODS_ON_RAIL)
	i = MAX_GOODS_ON_RAIL - MP_INFO(xt,yt).int_4;
    }
  i = (i * PORT_IMPORT_RATE) / 1000;
  MP_INFO(xt,yt).int_4 += i;
  return (i * PORT_GOODS_RATE);
}


int
buy_steel (int xt, int yt)
{
  int i = 0;
  if (MP_GROUP(xt,yt) == GROUP_TRACK)
    {
      if (MP_INFO(xt,yt).int_6 < MAX_STEEL_ON_TRACK)
	i = MAX_STEEL_ON_TRACK - MP_INFO(xt,yt).int_6;
    }
  else if (MP_GROUP(xt,yt) == GROUP_ROAD)
    {
      if (MP_INFO(xt,yt).int_6 < MAX_STEEL_ON_ROAD)
	i = MAX_STEEL_ON_ROAD - MP_INFO(xt,yt).int_6;
    }
  else if (MP_GROUP(xt,yt) == GROUP_RAIL)
    {
      if (MP_INFO(xt,yt).int_6 < MAX_STEEL_ON_RAIL)
	i = MAX_STEEL_ON_RAIL - MP_INFO(xt,yt).int_6;
    }
  i = (i * PORT_IMPORT_RATE) / 1000;
  MP_INFO(xt,yt).int_6 += i;
  return (i * PORT_STEEL_RATE);
}

int
sell_food (int xt, int yt)
{
  int i = 0;
  i = (MP_INFO(xt,yt).int_1 * PORT_EXPORT_RATE) / 1000;
  MP_INFO(xt,yt).int_1 -= i;
  return (i * PORT_FOOD_RATE);
}

int
sell_coal (int xt, int yt)
{
  int i = 0;
  i = (MP_INFO(xt,yt).int_3 * PORT_EXPORT_RATE) / 1000;
  MP_INFO(xt,yt).int_3 -= i;
  return (i * PORT_COAL_RATE);
}

int
sell_ore (int xt, int yt)
{
  int i = 0;
  i = (MP_INFO(xt,yt).int_5 * PORT_EXPORT_RATE) / 1000;
  MP_INFO(xt,yt).int_5 -= i;
  return (i * PORT_ORE_RATE);
}

int
sell_goods (int xt, int yt)
{
  int i = 0;
  i = (MP_INFO(xt,yt).int_4 * PORT_EXPORT_RATE) / 1000;
  MP_INFO(xt,yt).int_4 -= i;
  return (i * PORT_GOODS_RATE);
}

int
sell_steel (int xt, int yt)
{
  int i = 0;
  i = (MP_INFO(xt,yt).int_6 * PORT_EXPORT_RATE) / 1000;
  MP_INFO(xt,yt).int_6 -= i;
  return (i * PORT_STEEL_RATE);
}

void
do_pollution ()
{
  int x, p;
  int* pol = &map.pollution[0][0];

  /* Kill pollution from top edge of map */
  do {
    if (*pol > 0)
      *pol /= POL_DIV;
  } while (++pol < &map.pollution[1][0]);


  x= 1;
  do
    {
      /* Kill some pollution from left edge of map */
      if (*pol++ > 0)
        *(pol-1) /= POL_DIV;
      do {
        if (*pol > 10) {
	  p = *pol / 16;
	  *pol -= p;
	  switch ( rand() % 11)
	    {         /* prevailing wind is *from* SW ie right down */
	    case 0:
	    case 1: /* up */
	    case 2:
	      *(pol - 1) += p;
	      break;
	    case 3:
	    case 4: /* right */
	    case 5:
	      *(pol + WORLD_SIDE_LEN) += p;
	      break;
	    case 6: /* down */
	    case 7:
	      *(pol + 1) += p;
	      break;
	    case 8: /* left */
	    case 9:
	      *(pol - WORLD_SIDE_LEN) += p;
	      break;
	    case 10:
	      *pol += p- 2;
	      break;
	    }
	}
      } while (++pol < &map.pollution[x][WORLD_SIDE_LEN-1]);
      /* Kill some pollution from right edge of map */
      if (*pol > 0)
        *pol /= POL_DIV;
      ++x;
    }
  while (++pol < &map.pollution[WORLD_SIDE_LEN-1][0]);

  /* Kill pollution from bottom edge of map */
  do {
    if (*pol > 0)
      *pol /= POL_DIV;
  } while (++pol < &map.pollution[WORLD_SIDE_LEN][0]);
}


/* XXX: remove_people is only used by rocket_pad, perhaps it should go there */

void
remove_people (int num)
{
#if defined (commentout)
  int x, y, f;
  time_t t;
  f = 1;
  t = time (0);
  while (f && (num > 0)) {
      for (y = 0; y < WORLD_SIDE_LEN; y++)
	for (x = 0; x < WORLD_SIDE_LEN; x++)
	  if (MP_GROUP_IS_RESIDENCE(x,y) && MP_INFO(x,y).population > 0)
	    {
	      MP_INFO(x,y).population--;
	      // f = 1;
	      f |= (MP_INFO(x,y).population > 0);
	      num--;
	      total_evacuated++;
	    }
  }
  while (num > 0 && people_pool > 0) {
      num--;
      total_evacuated++;
      people_pool--;
  }
#endif

  int x, y;
  /* reset housed population so that we can display it correctly */
  housed_population = 1;
  while (housed_population && (num > 0)) {
      housed_population = 0;
      for (y = 0; y < WORLD_SIDE_LEN; y++)
	for (x = 0; x < WORLD_SIDE_LEN; x++)
	  if (MP_GROUP_IS_RESIDENCE(x,y) && MP_INFO(x,y).population > 0) {
	      MP_INFO(x,y).population--;
	      housed_population += MP_INFO(x,y).population;
	      num--;
	      total_evacuated++;
	  }
  }
  while (num > 0 && people_pool > 0) {
      num--;
      total_evacuated++;
      people_pool--;
  }

  refresh_population_text ();

#if defined (commentout)
/* last ship wasn't full so everyone has gone. */
  if (num > 0)
    {
      if (t > HOF_START && t < HOF_STOP)
	ok_dial_box ("launch-gone-mail.mes", GOOD, 0L);
      else
	ok_dial_box ("launch-gone.mes", GOOD, 0L);
      housed_population = 0;
    }
#endif

  /* Note that the previous test was inaccurate.  There could be 
     exactly 1000 people left. */
  if (!housed_population && !people_pool) {
    ok_dial_box ("launch-gone.mes", GOOD, 0L);
  }
}



void
clear_fire_health_and_cricket_cover (void)
{
  int x, y, m;
  m = 0xffffffff - (FLAG_FIRE_COVER | FLAG_HEALTH_COVER
		    | FLAG_CRICKET_COVER);
  for (y = 0; y < WORLD_SIDE_LEN; y++)
    for (x = 0; x < WORLD_SIDE_LEN; x++)
      MP_INFO(x,y).flags &= m;
  /* Wow... chache misses or what! */
}

void
do_fire_health_and_cricket_cover (void)
{
  int x, y;
  for (y = 0; y < WORLD_SIDE_LEN; y++)
    for (x = 0; x < WORLD_SIDE_LEN; x++)
      {
	/*  The next few lines need changing to test for */
	/*  the group if these areas are animated. */

	if (MP_GROUP(x,y) == GROUP_FIRESTATION)
	  do_fire_cover (x, y);
	else if (MP_TYPE(x,y) == CST_HEALTH)
	  do_health_cover (x, y);
	else if (MP_GROUP(x,y) == GROUP_CRICKET)
	  do_cricket_cover (x, y);
      }
}



void
do_random_fire (int x, int y, int pwarning)	/* well random if x=y=-1 */
{
  int xx, yy;
  if (x == -1 && y == -1)
    {
      x = rand () % WORLD_SIDE_LEN;
      y = rand () % WORLD_SIDE_LEN;
    }
  else
    {
      if (x < 0 || x >= WORLD_SIDE_LEN || y < 0 || y >= WORLD_SIDE_LEN)
	return;
    }
  if (MP_TYPE(x,y) == CST_USED)
    {
      xx = MP_INFO(x,y).int_1;
      yy = MP_INFO(x,y).int_2;
      x = xx;
      y = yy;
    }
  xx = rand () % 100;
  if (xx >= (main_groups[MP_GROUP(x,y)].fire_chance))
    return;
  if ((MP_INFO(x,y).flags & FLAG_FIRE_COVER) != 0)
    return;
  if (pwarning)
    {
      if (MP_GROUP(x,y) == GROUP_POWER_LINE)
	ok_dial_box ("fire.mes", BAD, _("It's at a power line."));
      else if (MP_GROUP(x,y) == GROUP_SOLAR_POWER)
	ok_dial_box ("fire.mes", BAD, _("It's at a solar power station."));
      else if (MP_GROUP(x,y) == GROUP_SUBSTATION)
	ok_dial_box ("fire.mes", BAD, _("It's at a substation."));
      else if (MP_GROUP_IS_RESIDENCE(x,y))
	ok_dial_box ("fire.mes", BAD, _("It's at a residential area."));
      else if (MP_GROUP(x,y) == GROUP_ORGANIC_FARM)
	ok_dial_box ("fire.mes", BAD, _("It's at a farm."));
      else if (MP_GROUP(x,y) == GROUP_MARKET)
	ok_dial_box ("fire.mes", BAD, _("It's at a market."));
      else if (MP_GROUP(x,y) == GROUP_TRACK)
	ok_dial_box ("fire.mes", BAD, _("It's at a track."));
      else if (MP_GROUP(x,y) == GROUP_COALMINE)
	ok_dial_box ("fire.mes", BAD, _("It's at a coal mine."));
      else if (MP_GROUP(x,y) == GROUP_RAIL)
	ok_dial_box ("fire.mes", BAD, _("It's at a railway."));
      else if (MP_GROUP(x,y) == GROUP_COAL_POWER)
	ok_dial_box ("fire.mes", BAD, _("It's at a coal power station."));
      else if (MP_GROUP(x,y) == GROUP_ROAD)
	ok_dial_box ("fire.mes", BAD, _("It's at a road."));
      else if (MP_GROUP(x,y) == GROUP_INDUSTRY_L)
	ok_dial_box ("fire.mes", BAD, _("It's at light industry."));
      else if (MP_GROUP(x,y) == GROUP_UNIVERSITY)
	ok_dial_box ("fire.mes", BAD, _("It's at a university."));
      else if (MP_GROUP(x,y) == GROUP_COMMUNE)
	ok_dial_box ("fire.mes", BAD, _("It's at a commune."));
      else if (MP_GROUP(x,y) == GROUP_TIP)
	ok_dial_box ("fire.mes", BAD, _("It's at a tip."));
      else if (MP_GROUP(x,y) == GROUP_PORT)
	ok_dial_box ("fire.mes", BAD, _("It's at a port."));
      else if (MP_GROUP(x,y) == GROUP_INDUSTRY_H)
	ok_dial_box ("fire.mes", BAD, _("It's at a steel works."));
      else if (MP_GROUP(x,y) == GROUP_RECYCLE)
	ok_dial_box ("fire.mes", BAD, _("It's at a recycle centre."));
      else if (MP_GROUP(x,y) == GROUP_HEALTH)
	ok_dial_box ("fire.mes", BAD, _("It's at a health centre."));
      else if (MP_GROUP(x,y) == GROUP_ROCKET)
	ok_dial_box ("fire.mes", BAD, _("It's at a rocket site."));
      else if (MP_GROUP(x,y) == GROUP_WINDMILL)
	ok_dial_box ("fire.mes", BAD, _("It's at a windmill."));
      else if (MP_GROUP(x,y) == GROUP_SCHOOL)
	ok_dial_box ("fire.mes", BAD, _("It's at a school."));
      else if (MP_GROUP(x,y) == GROUP_BLACKSMITH)
	ok_dial_box ("fire.mes", BAD, _("It's at a blacksmith."));
      else if (MP_GROUP(x,y) == GROUP_MILL)
	ok_dial_box ("fire.mes", BAD, _("It's at a mill."));
      else if (MP_GROUP(x,y) == GROUP_POTTERY)
	ok_dial_box ("fire.mes", BAD, _("It's at a pottery."));
      else if (MP_GROUP(x,y) == GROUP_FIRESTATION)
	ok_dial_box ("fire.mes", BAD, _("It's at a fire station!!!."));
      else if (MP_GROUP(x,y) == GROUP_CRICKET)
	ok_dial_box ("fire.mes", BAD, _("It's at a cricket pitch!!!."));
      else if (MP_GROUP(x,y) == GROUP_SHANTY)
	ok_dial_box ("fire.mes", BAD, _("It's at a shanty town."));
      else
	ok_dial_box ("fire.mes", BAD, _("UNKNOWN!"));
    }
  fire_area (x, y);
}

/*
   // spiral round from startx,starty until we hit something of group group.
   // return the x y coords encoded as x+y*WORLD_SIDE_LEN
   // return -1 if we don't find one.
 */
int
spiral_find_group (int startx, int starty, int group)
{
  int i, j, x, y;
  x = startx;
  y = starty;
  /* let's just do a complete spiral for now, work out the bounds later */
  for (i = 1; i < (WORLD_SIDE_LEN + WORLD_SIDE_LEN); i++)
    {
      for (j = 0; j < i; j++)
	{
	  x--;
	  if (x > 0 && x < WORLD_SIDE_LEN && y > 0 && y < WORLD_SIDE_LEN)
	    if (MP_GROUP(x,y) == group)
	      return (x + y * WORLD_SIDE_LEN);
	}
      for (j = 0; j < i; j++)
	{
	  y--;
	  if (x > 0 && x < WORLD_SIDE_LEN && y > 0 && y < WORLD_SIDE_LEN)
	    if (MP_GROUP(x,y) == group)
	      return (x + y * WORLD_SIDE_LEN);
	}
      i++;
      for (j = 0; j < i; j++)
	{
	  x++;
	  if (x > 0 && x < WORLD_SIDE_LEN && y > 0 && y < WORLD_SIDE_LEN)
	    if (MP_GROUP(x,y) == group)
	      return (x + y * WORLD_SIDE_LEN);
	}
      for (j = 0; j < i; j++)
	{
	  y++;
	  if (x > 0 && x < WORLD_SIDE_LEN && y > 0 && y < WORLD_SIDE_LEN)
	    if (MP_GROUP(x,y) == group)
	      return (x + y * WORLD_SIDE_LEN);
	}
    }
  return (-1);
}

/*
   // spiral round from startx,starty until we hit a 2x2 space.
   // return the x y coords encoded as x+y*WORLD_SIDE_LEN
   // return -1 if we don't find one.
 */
int
spiral_find_2x2 (int startx, int starty)
{
  int i, j, x, y;
  x = startx;
  y = starty;
  /* let's just do a complete spiral for now, work out the bounds later */
  for (i = 1; i < (WORLD_SIDE_LEN + WORLD_SIDE_LEN); i++)
    {
      for (j = 0; j < i; j++)
	{
	  x--;
	  if (x > 1 && x < WORLD_SIDE_LEN - 2 && y > 1
	      && y < WORLD_SIDE_LEN - 2)
	    if (MP_TYPE(x,y) == CST_GREEN
		&& MP_TYPE(x + 1,y) == CST_GREEN
		&& MP_TYPE(x,y + 1) == CST_GREEN
		&& MP_TYPE(x + 1,y + 1) == CST_GREEN)
	      return (x + y * WORLD_SIDE_LEN);
	}
      for (j = 0; j < i; j++)
	{
	  y--;
	  if (x > 1 && x < WORLD_SIDE_LEN - 2 && y > 1
	      && y < WORLD_SIDE_LEN - 2)
	    if (MP_TYPE(x,y) == CST_GREEN
		&& MP_TYPE(x + 1,y) == CST_GREEN
		&& MP_TYPE(x,y + 1) == CST_GREEN
		&& MP_TYPE(x + 1,y + 1) == CST_GREEN)
	      return (x + y * WORLD_SIDE_LEN);
	}
      i++;
      for (j = 0; j < i; j++)
	{
	  x++;
	  if (x > 1 && x < WORLD_SIDE_LEN - 2 && y > 1
	      && y < WORLD_SIDE_LEN - 2)
	    if (MP_TYPE(x,y) == CST_GREEN
		&& MP_TYPE(x + 1,y) == CST_GREEN
		&& MP_TYPE(x,y + 1) == CST_GREEN
		&& MP_TYPE(x + 1,y + 1) == CST_GREEN)
	      return (x + y * WORLD_SIDE_LEN);
	}
      for (j = 0; j < i; j++)
	{
	  y++;
	  if (x > 1 && x < WORLD_SIDE_LEN - 2 && y > 1
	      && y < WORLD_SIDE_LEN - 2)
	    if (MP_TYPE(x,y) == CST_GREEN
		&& MP_TYPE(x + 1,y) == CST_GREEN
		&& MP_TYPE(x,y + 1) == CST_GREEN
		&& MP_TYPE(x + 1,y + 1) == CST_GREEN)
	      return (x + y * WORLD_SIDE_LEN);
	}
    }
  return (-1);
}



void
do_bulldoze_area (short fill, int xx, int yy)
{
  int size, x, y;
  if (MP_TYPE(xx,yy) == CST_USED)
    {
      x = MP_INFO(xx,yy).int_1;
      y = MP_INFO(xx,yy).int_2;
    }
  else
    {
      x = xx;
      y = yy;
    }
  size = MP_SIZE(x,y);
  if (MP_GROUP(x,y) == GROUP_SUBSTATION
      || MP_GROUP(x,y) == GROUP_WINDMILL)
    remove_a_substation (x, y);
  else if (MP_GROUP(x,y) == GROUP_MARKET)
    remove_a_market (x, y);
  else if (MP_GROUP(x,y) == GROUP_SHANTY)
    numof_shanties--;
  else if (MP_GROUP(x,y) == GROUP_COMMUNE)
    numof_communes--;

  people_pool += MP_INFO(x,y).population;
  clear_mappoint (fill, x, y);
  if (size > 1)			/* do size 2 */
    {
      clear_mappoint (fill, x + 1, y);
      clear_mappoint (fill, x, y + 1);
      clear_mappoint (fill, x + 1, y + 1);
    }
  if (size > 2)			/* do size 3 */
    {
      clear_mappoint (fill, x + 2, y);
      clear_mappoint (fill, x + 2, y + 1);
      clear_mappoint (fill, x + 2, y + 2);
      clear_mappoint (fill, x, y + 2);
      clear_mappoint (fill, x + 1, y + 2);
    }
  if (size > 3)			/* do size 4 */
    {
      clear_mappoint (fill, x + 3, y);
      clear_mappoint (fill, x + 3, y + 1);
      clear_mappoint (fill, x + 3, y + 2);
      clear_mappoint (fill, x + 3, y + 3);
      clear_mappoint (fill, x, y + 3);
      clear_mappoint (fill, x + 1, y + 3);
      clear_mappoint (fill, x + 2, y + 3);
    }
}


void
update_tech_dep (int x, int y)
{
  switch (MP_GROUP(x,y))
    {
    case (GROUP_ORGANIC_FARM):
      MP_INFO(x,y).int_7 = ((double) MP_INFO(x,y).int_1
			      * ORGANIC_FARM_FOOD_OUTPUT) / MAX_TECH_LEVEL;
      break;
    case (GROUP_WINDMILL):
#ifdef OLD_POWER_CODE
      MP_INFO(x,y).int_5 = WINDMILL_POWER
#else
      MP_INFO(x,y).int_1 = WINDMILL_POWER
#endif
	+ (((double) MP_INFO(x,y).int_2 * WINDMILL_POWER) / MAX_TECH_LEVEL);
      break;
    case (GROUP_COAL_POWER):
#ifdef OLD_POWER_CODE
      MP_INFO(x,y).int_5 = POWERS_COAL_OUTPUT
#else
      MP_INFO(x,y).int_1 = POWERS_COAL_OUTPUT
#endif
	+ (((double) MP_INFO(x,y).int_4 * POWERS_COAL_OUTPUT)
	   / MAX_TECH_LEVEL);
      break;
    case (GROUP_SOLAR_POWER):
      MP_INFO(x,y).int_3 = POWERS_SOLAR_OUTPUT
	+ (((double) MP_INFO(x,y).int_2 * POWERS_SOLAR_OUTPUT)
	   / MAX_TECH_LEVEL);
      break;
    }
}

void
connect_rivers (void)
{
  int x, y, count;
  count = 1;
  while (count > 0)
    {
      count = 0;
      for (y = 0; y < WORLD_SIDE_LEN; y++)
	for (x = 0; x < WORLD_SIDE_LEN; x++)
	  {
	    if (is_real_river (x, y) == 1)
	      {
		if (is_real_river (x - 1, y) == -1)
		  {
		    MP_INFO(x - 1,y).flags |= FLAG_IS_RIVER;
		    count++;
		  }
		if (is_real_river (x, y - 1) == -1)
		  {
		    MP_INFO(x,y - 1).flags |= FLAG_IS_RIVER;
		    count++;
		  }
		if (is_real_river (x + 1, y) == -1)
		  {
		    MP_INFO(x + 1,y).flags |= FLAG_IS_RIVER;
		    count++;
		  }
		if (is_real_river (x, y + 1) == -1)
		  {
		    MP_INFO(x,y + 1).flags |= FLAG_IS_RIVER;
		    count++;
		  }
	      }
	  }
    }
}

int
is_real_river (int x, int y)
{
  /* returns zero if not water at all or if out of bounds. */
  if (x < 0 || x >= WORLD_SIDE_LEN || y < 0 || y >= WORLD_SIDE_LEN)
    return (0);
  if (MP_GROUP(x,y) != GROUP_WATER)
    return (0);
  if (MP_INFO(x,y).flags & FLAG_IS_RIVER)
    return (1);
  return (-1);
}

/* Feature: coal survey should vary in price and accuracy with technology */
void 
do_coal_survey (void)
{
    if (coal_survey_done == 0) {
	adjust_money(-1000000);
	coal_survey_done = 1;
    }
}
