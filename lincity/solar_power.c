/* ---------------------------------------------------------------------- *
 * solar_power.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "power.h"
#include "solar_power.h"

/*** Solar Power ***/

/*
  int_1 is the power to give away,  this must be >= POWER_LINE_CAPACITY
  for it to pass it to a power line. 
  Not added to if > POWER_LINE_CAPACITY
  int_2 is the tech level when it was built.
  int_3 is the rated output
  int_5 is the current output
  int_6 is the grid it's on
  int_7 is a grid mapping timestamp
*/

void
do_power_source (int x, int y)
{
    if (get_jobs(x, y, SOLAR_POWER_JOBS)) {
	MP_INFO(x,y).int_5 = MP_INFO(x,y).int_3;
	grid[MP_INFO(x,y).int_6]->avail_power += MP_INFO(x,y).int_3;
    } else {
	MP_INFO(x,y).int_5 = 0;
    }
}


void
mps_solar_power (int x, int y)
{
  int i = 0;

  char s[12];

  mps_store_title(i++,_("Solar"));
  mps_store_title(i++,_("Power Station"));
  i++;

  format_power (s, sizeof(s), MP_INFO(x,y).int_3);
  mps_store_title(i++,_("Max Output"));
  mps_store_title(i++,s);
  i++;

  format_power (s, sizeof(s), MP_INFO(x,y).int_5);
  mps_store_title(i++,_("Current Output"));
  mps_store_title(i++,s);
  i++;

  mps_store_sfp(i++,_("Tech"),
		MP_INFO(x,y).int_2 * 100.0 / MAX_TECH_LEVEL);  
  mps_store_sd(i++,_("Grid ID"), MP_INFO(x,y).int_6);
}
