/* ---------------------------------------------------------------------- *
 * oremine.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "../screen.h"
#include "oremine.h"



void
do_oremine (int x, int y)
{
  /*
     // int_1 is the ore at in stock
     // int_2 is the ore reserve under the ground or at the surface really.
   */
  int xx, yy, xs, ys, xe, ye, cr;
  if (MP_INFO(x,y).int_1 < DIG_MORE_ORE_TRIGGER - 5000)
    {
      xs = x;
      ys = y;
      xe = x + 4;
      ye = y + 4;
      cr = 0;
      for (yy = ys; yy < ye; yy++)
	for (xx = xs; xx < xe; xx++)
	  cr += MP_INFO(xx,yy).ore_reserve;
      MP_INFO(x,y).int_2 = cr;
      if (cr > 0)
	if (get_jobs (x, y, JOBS_DIG_ORE) != 0)
	  for (yy = ys; yy < ye; yy++)
	    for (xx = xs; xx < xe; xx++)
	      if (MP_INFO(xx,yy).ore_reserve > 0)
		{
		  MP_INFO(xx,yy).ore_reserve--;
		  MP_INFO(x,y).int_1 += 5000;
		  ore_made += 5000;
		  sust_dig_ore_coal_tip_flag = 0;
		  /* maybe want an ore tax? */
		  yy = ye;
		  xx = xe;	/* break out */
		}
    }

  if ((MP_INFO(x - 1,y).flags & FLAG_IS_TRANSPORT) != 0)
    {
      if (MP_GROUP(x-1,y) == GROUP_RAIL
	  && MP_INFO(x - 1,y).int_5 < MAX_ORE_ON_RAIL
	  && MP_INFO(x,y).int_1 >= (MAX_ORE_ON_RAIL
				      - MP_INFO(x - 1,y).int_5))
	{
	  if (get_jobs (x, y, JOBS_LOAD_ORE) != 0)
	    {
	      MP_INFO(x,y).int_1
		-= (MAX_ORE_ON_RAIL - MP_INFO(x - 1,y).int_5);
	      MP_INFO(x - 1,y).int_5 = MAX_ORE_ON_RAIL;
	    }
	}
      else if (MP_GROUP(x-1,y) == GROUP_ROAD
	       && MP_INFO(x - 1,y).int_5 < MAX_ORE_ON_ROAD
	       && MP_INFO(x,y).int_1 >= (MAX_ORE_ON_ROAD
					   - MP_INFO(x - 1,y).int_5))
	{
	  if (get_jobs (x, y, JOBS_LOAD_ORE) != 0)
	    {
	      MP_INFO(x,y).int_1
		-= (MAX_ORE_ON_ROAD - MP_INFO(x - 1,y).int_5);
	      MP_INFO(x - 1,y).int_5 = MAX_ORE_ON_ROAD;
	    }
	}
      else if (MP_GROUP(x - 1,y) == GROUP_TRACK
	       && MP_INFO(x - 1,y).int_5 < MAX_ORE_ON_TRACK
	       && MP_INFO(x,y).int_1 >= (MAX_ORE_ON_TRACK
					   - MP_INFO(x - 1,y).int_5))
	{
	  if (get_jobs (x, y, JOBS_LOAD_ORE) != 0)
	    {
	      MP_INFO(x,y).int_1
		-= (MAX_ORE_ON_TRACK - MP_INFO(x - 1,y).int_5);
	      MP_INFO(x - 1,y).int_5 = MAX_ORE_ON_TRACK;
	    }
	}
    }

  if ((MP_INFO(x,y - 1).flags & FLAG_IS_TRANSPORT) != 0)
    {
      if (MP_GROUP(x,y-1) == GROUP_RAIL
	  && MP_INFO(x,y - 1).int_5 < MAX_ORE_ON_RAIL
	  && MP_INFO(x,y).int_1 >= (MAX_ORE_ON_RAIL
				      - MP_INFO(x,y - 1).int_5))
	{
	  if (get_jobs (x, y, JOBS_LOAD_ORE) != 0)
	    {
	      MP_INFO(x,y).int_1
		-= (MAX_ORE_ON_RAIL - MP_INFO(x,y - 1).int_5);
	      MP_INFO(x,y - 1).int_5 = MAX_ORE_ON_RAIL;
	    }
	}
      else if (MP_GROUP(x,y-1) == GROUP_ROAD
	       && MP_INFO(x,y - 1).int_5 < MAX_ORE_ON_ROAD
	       && MP_INFO(x,y).int_1 >= (MAX_ORE_ON_ROAD
					   - MP_INFO(x,y - 1).int_5))
	{
	  if (get_jobs (x, y, JOBS_LOAD_ORE) != 0)
	    {
	      MP_INFO(x,y).int_1
		-= (MAX_ORE_ON_ROAD - MP_INFO(x,y - 1).int_5);
	      MP_INFO(x,y - 1).int_5 = MAX_ORE_ON_ROAD;
	    }
	}
      else if (MP_GROUP(x,y-1) == GROUP_TRACK
	       && MP_INFO(x,y - 1).int_5 < MAX_ORE_ON_TRACK
	       && MP_INFO(x,y).int_1 >= (MAX_ORE_ON_TRACK
					   - MP_INFO(x,y - 1).int_5))
	{
	  if (get_jobs (x, y, JOBS_LOAD_ORE) != 0)
	    {
	      MP_INFO(x,y).int_1
		-= (MAX_ORE_ON_TRACK - MP_INFO(x,y - 1).int_5);
	      MP_INFO(x,y - 1).int_5 = MAX_ORE_ON_TRACK;
	    }
	}
    }


  /* choose a graphic */
  if ((total_time & 0x7f) == 0)
    {
      xx = 7 * (MP_INFO(x,y).int_2 + (3 * ORE_RESERVE / 2))
	/ (16 * ORE_RESERVE);
      switch (xx)
	{
	case (0):
	  MP_TYPE(x,y) = CST_OREMINE_8;
	  break;
	case (1):
	  MP_TYPE(x,y) = CST_OREMINE_7;
	  break;
	case (2):
	  MP_TYPE(x,y) = CST_OREMINE_6;
	  break;
	case (3):
	  MP_TYPE(x,y) = CST_OREMINE_5;
	  break;
	case (4):
	  MP_TYPE(x,y) = CST_OREMINE_4;
	  break;
	case (5):
	  MP_TYPE(x,y) = CST_OREMINE_3;
	  break;
	case (6):
	  MP_TYPE(x,y) = CST_OREMINE_2;
	  break;
	case (7):
	  MP_TYPE(x,y) = CST_OREMINE_1;
	  break;
	}
	if (MP_INFO(x,y).int_2 <= 0) {
#if defined (commentout)
	    do_bulldoze_area (CST_GREEN, x, y);
	    place_item(x,y,CST_TIP_0);
#endif
	    int i,j;
	    for (j = 0; j < 4; j++) {
		for (i = 0; i < 4; i++) {
		    do_bulldoze_area (CST_WATER, x+i, y+j);
		}
	    }
	    connect_rivers ();
	    refresh_main_screen ();
	}
    }
}

void
mps_oremine (int x, int y)
{
  int i = 0;

  mps_store_title(i++,_("Ore Mine"));
  i++;

  mps_store_sfp(i++,_("Stock"), 
		MP_INFO(x,y).int_1 * 100.0 / DIG_MORE_ORE_TRIGGER);
  i++;

  mps_store_sfp(i++,_("Reserve"),
		MP_INFO(x,y).int_2 * 100.0 / (ORE_RESERVE * 16));
}

