/* ---------------------------------------------------------------------- *
 * pottery.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include <lin-city.h>
#include <lctypes.h>
#include <engglobs.h>
#include <cliglobs.h>
#include <stats.h>
#include <pottery.h>


void
do_pottery (int x, int y)
{
  /*
    // int_1 contains the goods at the pottery
    // int_2 contains the ore at the pottery
    // int_3 contains the coal at the pottery
    // int_4 is the animation trigger time
    // int_5 is the % made so far this month or the close time if negative
    // int_6 is the % capacity last month
    // int_7 contains the jobs stored at the pottery
  */
  if (MP_INFO(x,y).int_5 < 0)
    {
      MP_INFO(x,y).int_5++;
      return;
    }
  if (MP_INFO(x,y).int_1 < (MAX_GOODS_AT_POTTERY - POTTERY_MADE_GOODS))
    {
      if (MP_INFO(x,y).int_2
	  < (MAX_ORE_AT_POTTERY - POTTERY_GET_ORE))
	if (get_ore (x, y, POTTERY_GET_ORE) != 0)
	  MP_INFO(x,y).int_2 += POTTERY_GET_ORE;
      if (MP_INFO(x,y).int_3
	  < (MAX_COAL_AT_POTTERY - POTTERY_GET_COAL))
	if (get_coal (x, y, POTTERY_GET_COAL) != 0)
	  MP_INFO(x,y).int_3 += POTTERY_GET_COAL;
      if (MP_INFO(x,y).int_7
	  < (MAX_JOBS_AT_POTTERY - POTTERY_GET_JOBS))
	if (get_jobs (x, y, POTTERY_GET_JOBS) != 0)
	  MP_INFO(x,y).int_7 += POTTERY_GET_JOBS;

      if (MP_INFO(x,y).int_2 > POTTERY_ORE_MAKE_GOODS
	  && MP_INFO(x,y).int_3 > POTTERY_COAL_MAKE_GOODS
	  && MP_INFO(x,y).int_7 > POTTERY_JOBS)
	{
	  MP_INFO(x,y).int_1 += POTTERY_MADE_GOODS;
	  MP_INFO(x,y).int_2 -= POTTERY_ORE_MAKE_GOODS;
	  MP_INFO(x,y).int_3 -= POTTERY_COAL_MAKE_GOODS;
	  MP_INFO(x,y).int_7 -= POTTERY_JOBS;
	  MP_INFO(x,y).int_5++;
	}
      else
	{
	  MP_TYPE(x,y) = CST_POTTERY_1;
	  MP_INFO(x,y).int_6 = 0;
	  MP_INFO(x,y).int_5 = -POTTERY_CLOSE_TIME;
	  return;
	}
    }
  if (MP_INFO(x,y).int_1 > 0)
    if (put_goods (x, y, MP_INFO(x,y).int_1) != 0)
      MP_INFO(x,y).int_1 = 0;

  if (total_time % 100 == 0)
    {
      MP_INFO(x,y).int_6 = MP_INFO(x,y).int_5;
      MP_INFO(x,y).int_5 = 0;
    }
  if (real_time >= MP_INFO(x,y).int_4 /* && block_anim==0 */ )
    {
      MP_INFO(x,y).int_4 = real_time + POTTERY_ANIM_SPEED;
      switch (MP_TYPE(x,y))
	{
	case (CST_POTTERY_0):
	  MP_TYPE(x,y) = CST_POTTERY_1;
	  break;
	case (CST_POTTERY_1):
	  MP_TYPE(x,y) = CST_POTTERY_2;
	  break;
	case (CST_POTTERY_2):
	  MP_TYPE(x,y) = CST_POTTERY_3;
	  break;
	case (CST_POTTERY_3):
	  MP_TYPE(x,y) = CST_POTTERY_4;
	  break;
	case (CST_POTTERY_4):
	  MP_TYPE(x,y) = CST_POTTERY_5;
	  break;
	case (CST_POTTERY_5):
	  MP_TYPE(x,y) = CST_POTTERY_6;
	  break;
	case (CST_POTTERY_6):
	  MP_TYPE(x,y) = CST_POTTERY_7;
	  break;
	case (CST_POTTERY_7):
	  MP_TYPE(x,y) = CST_POTTERY_8;
	  break;
	case (CST_POTTERY_8):
	  MP_TYPE(x,y) = CST_POTTERY_9;
	  break;
	case (CST_POTTERY_9):
	  MP_TYPE(x,y) = CST_POTTERY_10;
	  break;
	case (CST_POTTERY_10):
	  MP_TYPE(x,y) = CST_POTTERY_1;
	  MP_POL(x,y)++;
	  break;
	}
    }
}
