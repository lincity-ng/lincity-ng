/* ---------------------------------------------------------------------- *
 * coalmine.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include <lin-city.h>
#include <lctypes.h>
#include <lcintl.h>
#include <lcconfig.h>
#include <engglobs.h>
#include <cliglobs.h>
#include <stats.h>
#include <mps.h>
#include <coalmine.h>



void
do_coalmine (int x, int y)
{
  /*
     // int_1 is the coal at the surface
     // int_2 is the coal reserve under the ground. More than one mine can
     // int_3 is the jobs collected.
   */
  /* claim the coal under ground! */
  int xx, yy, xs, ys, xe, ye, cr;
  if (MP_INFO(x,y).int_1 < (DIG_MORE_COAL_TRIGGER - 1000))
    {
      if (MP_INFO(x,y).int_2 < 0)
	return;			/* run out of reserves */

      xs = x - COAL_RESERVE_SEARCH_LEN;
      if (xs < 0)
	xs = 0;
      ys = y - COAL_RESERVE_SEARCH_LEN;
      if (ys < 0)
	ys = 0;
      xe = x + COAL_RESERVE_SEARCH_LEN;
      if (xe > WORLD_SIDE_LEN)
	xe = WORLD_SIDE_LEN;
      ye = y + COAL_RESERVE_SEARCH_LEN;
      if (ye > WORLD_SIDE_LEN)
	ye = WORLD_SIDE_LEN;
      cr = 0;
      for (yy = ys; yy < ye; yy++)
	for (xx = xs; xx < xe; xx++)
	  cr += MP_INFO(xx,yy).coal_reserve;
      MP_INFO(x,y).int_2 = cr;
      if (cr > 0)
	{
	  if (get_jobs (x, y, JOBS_DIG_COAL
			- MP_INFO(x,y).int_3) != 0)
	    {
	      MP_INFO(x,y).int_3 = 0;
	      for (yy = ys; yy < ye; yy++)
		for (xx = xs; xx < xe; xx++)
		  if (MP_INFO(xx,yy).coal_reserve > 0)
		    {
		      MP_INFO(xx,yy).coal_reserve--;
		      MP_INFO(x,y).int_1 += 1000;
		      coal_made += 1000;
		      coal_tax += 1000;
		      sust_dig_ore_coal_tip_flag = 0;
		      MP_POL(x,y) += COALMINE_POLLUTION;
		      yy = ye;
		      xx = xe;	/* break out */

		    }
	    }
	  else if (get_jobs (x, y, JOBS_DIG_COAL / 10) != 0)
	    MP_INFO(x,y).int_3 += JOBS_DIG_COAL / 10;
	  else if (get_jobs (x, y, JOBS_DIG_COAL / 50) != 0)
	    MP_INFO(x,y).int_3 += JOBS_DIG_COAL / 50;
	}
      else
	{
	  MP_INFO(x,y).int_1 = 0;
	  MP_INFO(x,y).int_2 = -1;
	}
    }
  /* put it on the railway */
  if (MP_GROUP(x - 1,y) == GROUP_RAIL
      && MP_INFO(x - 1,y).int_3 < MAX_COAL_ON_RAIL
      && MP_INFO(x,y).int_1 >= (MAX_COAL_ON_RAIL
				  - MP_INFO(x - 1,y).int_3))
    {
      if (get_jobs (x, y, JOBS_LOAD_COAL) != 0)
	{
	  MP_INFO(x,y).int_1
	    -= (MAX_COAL_ON_RAIL - MP_INFO(x - 1,y).int_3);
	  MP_INFO(x - 1,y).int_3 = MAX_COAL_ON_RAIL;
	}
    }
  if (MP_GROUP(x,y-1) == GROUP_RAIL
      && MP_INFO(x,y - 1).int_3 < MAX_COAL_ON_RAIL
      && MP_INFO(x,y).int_1 >= (MAX_COAL_ON_RAIL
				  - MP_INFO(x,y - 1).int_3))
    {
      if (get_jobs (x, y, JOBS_LOAD_COAL) != 0)
	{
	  MP_INFO(x,y).int_1
	    -= (MAX_COAL_ON_RAIL - MP_INFO(x,y - 1).int_3);
	  MP_INFO(x,y - 1).int_3 = MAX_COAL_ON_RAIL;
	}
    }
  /* put it on the road */
  if (MP_GROUP(x-1,y) == GROUP_ROAD
      && MP_INFO(x - 1,y).int_3 < MAX_COAL_ON_ROAD
      && MP_INFO(x,y).int_1 >= (MAX_COAL_ON_ROAD
				  - MP_INFO(x - 1,y).int_3))
    {
      if (get_jobs (x, y, JOBS_LOAD_COAL) != 0)
	{
	  MP_INFO(x,y).int_1
	    -= (MAX_COAL_ON_ROAD - MP_INFO(x - 1,y).int_3);
	  MP_INFO(x - 1,y).int_3 = MAX_COAL_ON_ROAD;
	}
    }
  if (MP_GROUP(x,y-1) == GROUP_ROAD
      && MP_INFO(x,y - 1).int_3 < MAX_COAL_ON_ROAD
      && MP_INFO(x,y).int_1 >= (MAX_COAL_ON_ROAD
				  - MP_INFO(x,y - 1).int_3))
    {
      if (get_jobs (x, y, JOBS_LOAD_COAL) != 0)
	{
	  MP_INFO(x,y).int_1
	    -= (MAX_COAL_ON_ROAD - MP_INFO(x,y - 1).int_3);
	  MP_INFO(x,y - 1).int_3 = MAX_COAL_ON_ROAD;
	}
    }
  /* put it on the tracks */
  if (MP_GROUP(x-1,y) == GROUP_TRACK
      && MP_INFO(x - 1,y).int_3 < MAX_COAL_ON_TRACK
      && MP_INFO(x,y).int_1 >= (MAX_COAL_ON_TRACK
				  - MP_INFO(x - 1,y).int_3))
    {
      if (get_jobs (x, y, JOBS_LOAD_COAL) != 0)
	{
	  MP_INFO(x,y).int_1
	    -= (MAX_COAL_ON_TRACK - MP_INFO(x - 1,y).int_3);
	  MP_INFO(x - 1,y).int_3 = MAX_COAL_ON_TRACK;
	}
    }
  if (MP_GROUP(x,y-1) == GROUP_TRACK
      && MP_INFO(x,y - 1).int_3 < MAX_COAL_ON_TRACK
      && MP_INFO(x,y).int_1 >= (MAX_COAL_ON_TRACK
				  - MP_INFO(x,y - 1).int_3))
    {
      if (get_jobs (x, y, JOBS_LOAD_COAL) != 0)
	{
	  MP_INFO(x,y).int_1
	    -= (MAX_COAL_ON_TRACK - MP_INFO(x,y - 1).int_3);
	  MP_INFO(x,y - 1).int_3 = MAX_COAL_ON_TRACK;
	}
    }

  /* choose a graphic */
  if (MP_INFO(x,y).int_1 > (MAX_COAL_AT_MINE - (MAX_COAL_AT_MINE / 5)))
    MP_TYPE(x,y) = CST_COALMINE_FULL;
  else if (MP_INFO(x,y).int_1 > (MAX_COAL_AT_MINE / 2))
    MP_TYPE(x,y) = CST_COALMINE_MED;
  else if (MP_INFO(x,y).int_1 > 0)
    MP_TYPE(x,y) = CST_COALMINE_LOW;
  else
    MP_TYPE(x,y) = CST_COALMINE_EMPTY;
}

void
mps_coalmine (int x, int y)
{
  int i = 0;

  mps_store_title(i++,_("Coal Mine"));
  i++;

  mps_store_sfp(i++,_("Stock"),
		MP_INFO(x,y).int_1 * 100 / MAX_COAL_AT_MINE);

  if (MP_INFO(x,y).int_2 > 0) {
      mps_store_sd(i++, _("Reserve"), MP_INFO(x,y).int_2);
  } else {
      mps_store_ss(i++, _("Reserve"), _("EMPTY"));
  }
}
