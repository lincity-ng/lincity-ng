/* ---------------------------------------------------------------------- *
 * recycle.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "recycle.h"

void
do_recycle (int x, int y)
{
  int i;
  /*
     // int_1 is the ore made and waiting to go out
     // int_2 is the used goods in store
     // int_3 is the used steel in store       NOT USED at this time
     // int_4 is the tech level when built
     // int_5 is the recycling done so far this month
     // int_6 is the percent of max recycling last month
     // int_7 is the waste in store
     // cost
   */
  recycle_cost += RECYCLE_RUNNING_COST;

  /*
     // let these go through, even if we're full of waste. It's a waste of time
     // checking.
   */
  if (x > 0 && (MP_INFO(x - 1,y).flags & FLAG_IS_TRANSPORT) != 0)
    {
      i = MP_INFO(x - 1,y).int_7;
      if (i > MAX_WASTE_AT_RECYCLE - MP_INFO(x,y).int_2)
	i = MAX_WASTE_AT_RECYCLE - MP_INFO(x,y).int_2;
      MP_INFO(x,y).int_2 += i;
      MP_INFO(x - 1,y).int_7 -= i;
    }
  if (y > 0 && (MP_INFO(x,y - 1).flags & FLAG_IS_TRANSPORT) != 0)
    {
      i = MP_INFO(x,y - 1).int_7;
      if (i > MAX_WASTE_AT_RECYCLE - MP_INFO(x,y).int_2)
	i = MAX_WASTE_AT_RECYCLE - MP_INFO(x,y).int_2;
      MP_INFO(x,y).int_2 += i;
      MP_INFO(x,y - 1).int_7 -= i;
    }

  /* get some startup power if not powered yet */
  if ((MP_INFO(x,y).flags & FLAG_POWERED) == 0)
    if (get_power (x, y, GOODS_RECYCLED, 1) != 0)
      MP_INFO(x,y).flags |= FLAG_POWERED;

  /* no steel recycling yet - no point, it's only used to make goods.
     // recycle to ore.
   */
  if (MP_INFO(x,y).int_1 < MAX_ORE_AT_RECYCLE
      && MP_INFO(x,y).int_2 > GOODS_RECYCLED
      && (MP_INFO(x,y).flags & FLAG_POWERED) != 0)
    if (get_jobs (x, y, RECYCLE_GOODS_JOBS) != 0)
      {
	if (get_power (x, y, GOODS_RECYCLED / 2, 1) == 0)
	  MP_INFO(x,y).flags
	    &= (0xffffffff - FLAG_POWERED);
	else
	  MP_INFO(x,y).flags |= FLAG_POWERED;
	MP_INFO(x,y).int_2 -= GOODS_RECYCLED;
	i = (GOODS_RECYCLED * (10 + ((50 * MP_INFO(x,y).int_4)
				     / MAX_TECH_LEVEL))) / 100;
	if (i > (GOODS_RECYCLED * 8) / 10)
	  i = (GOODS_RECYCLED * 8) / 10;
	MP_INFO(x,y).int_1 += i;
	ore_made += i;
	MP_INFO(x,y).int_5++;
      }
  if (total_time % 100 == 0)
    {
      MP_INFO(x,y).int_6 = MP_INFO(x,y).int_5;
      MP_INFO(x,y).int_5 = 0;
    }
  /* now bung the ore out */
  /* put it on the railway */
  if (x > 0 && MP_GROUP(x-1,y) == GROUP_RAIL
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
  if (y > 0 && MP_GROUP(x,y-1) == GROUP_RAIL
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
  /* put it on the road */
  if (x > 0 && MP_GROUP(x-1,y) == GROUP_ROAD
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
  if (y > 0 && MP_GROUP(x,y-1) == GROUP_ROAD
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
  /* put it on the tracks */
  if (x > 0 && MP_GROUP(x-1,y) == GROUP_TRACK
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
  if (y > 0 && MP_GROUP(x,y-1) == GROUP_TRACK
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
  /* if we've still got >90% ore and waste in stock, burn some waste cleanly. 
   */
  if (MP_INFO(x,y).int_1 > (MAX_ORE_AT_RECYCLE * 9 / 10)
      && MP_INFO(x,y).int_2 > (MAX_WASTE_AT_RECYCLE * 9 / 10))
    MP_INFO(x,y).int_2 -= BURN_WASTE_AT_RECYCLE;
}

void
mps_recycle (int x, int y)
{
  int i = 0;
  char * p;

  mps_store_title(i++,_("Recycling"));
  mps_store_title(i++,_("Center"));
  i++;

  mps_store_sfp(i++,_("Capacity"), MP_INFO(x,y).int_6);

  p = ((MP_INFO(x,y).flags & FLAG_POWERED) != 0) ? _("YES") : _("NO");
  mps_store_ss(i++,_("Power"),p);

  mps_store_sfp(i++,_("Tech"),
		MP_INFO(x,y).int_4 * 100.0 / MAX_TECH_LEVEL);
  i++;
  mps_store_title(i++,_("Inventory"));
  mps_store_sfp(i++,_("Ore"),
		MP_INFO(x,y).int_1 * 100.0 / MAX_ORE_AT_RECYCLE);
  mps_store_sfp(i++,_("Waste"),
		MP_INFO(x,y).int_2 * 100.0 / MAX_WASTE_AT_RECYCLE);

}
