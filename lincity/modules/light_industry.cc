/* ---------------------------------------------------------------------- *
 * light_industry.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "light_industry.h"


void
do_industry_l (int x, int y)
{
  int goods = 0;
  double tmp_pol;
  /*
     // int_1 is the goods produced this month so far
     // int_2 is the amount of goods in store.
     // int_3 is the amount of ore in store.
     // int_4 is the amount of steel in store.
     // int_5 is the jobs stored.
     // int_6 is the percent of capacity last month.
     // int 7 is the next animation frame time.
   */
  /* first get some jobs */
  if (MP_INFO(x,y).int_5 < MAX_JOBS_AT_INDUSTRY_L - INDUSTRY_L_GET_JOBS)
    {
      if (get_jobs (x, y, INDUSTRY_L_GET_JOBS) != 0)
	MP_INFO(x,y).int_5 += INDUSTRY_L_GET_JOBS;
      else if (get_jobs (x, y, INDUSTRY_L_GET_JOBS / 10) != 0)
	MP_INFO(x,y).int_5 += INDUSTRY_L_GET_JOBS / 10;
    }
  /* if we don't have enough jobs we can't do anything */
  if (MP_INFO(x,y).int_5 < MIN_JOBS_AT_INDUSTRY_L)
    return;
  /* get some ore */
  if (MP_INFO(x,y).int_3 < (MAX_ORE_AT_INDUSTRY_L
			      - INDUSTRY_L_GET_ORE))
    {
      if ((MP_INFO(x - 1,y).flags & FLAG_IS_TRANSPORT) != 0
	  && MP_INFO(x - 1,y).int_5 > 0)
	{
	  if (MP_INFO(x - 1,y).int_5 >= INDUSTRY_L_GET_ORE)
	    {
	      MP_INFO(x,y).int_3 += INDUSTRY_L_GET_ORE;
	      MP_INFO(x - 1,y).int_5 -= INDUSTRY_L_GET_ORE;
	    }
	  else
	    {
	      MP_INFO(x,y).int_3 += MP_INFO(x - 1,y).int_5;
	      MP_INFO(x - 1,y).int_5 = 0;
	    }
	  MP_INFO(x,y).int_5 -= INDUSTRY_L_JOBS_LOAD_ORE;
	}
    }
  /* do we still need some ore? */
  if (MP_INFO(x,y).int_3 < (MAX_ORE_AT_INDUSTRY_L
			      - INDUSTRY_L_GET_ORE))
    {
      if ((MP_INFO(x,y - 1).flags & FLAG_IS_TRANSPORT) != 0
	  && MP_INFO(x,y - 1).int_5 > 0)
	{
	  if (MP_INFO(x,y - 1).int_5 >= INDUSTRY_L_GET_ORE)
	    {
	      MP_INFO(x,y).int_3 += INDUSTRY_L_GET_ORE;
	      MP_INFO(x,y - 1).int_5 -= INDUSTRY_L_GET_ORE;
	    }
	  else
	    {
	      MP_INFO(x,y).int_3 += MP_INFO(x,y - 1).int_5;
	      MP_INFO(x,y - 1).int_5 = 0;
	    }
	  MP_INFO(x,y).int_5 -= INDUSTRY_L_JOBS_LOAD_ORE;
	}
    }
  /* then get some steel */
  if (MP_INFO(x,y).int_4 < (MAX_STEEL_AT_INDUSTRY_L
			      - INDUSTRY_L_GET_STEEL))
    {
      if ((MP_INFO(x - 1,y).flags & FLAG_IS_TRANSPORT) != 0
	  && MP_INFO(x - 1,y).int_6 > 0)
	{
	  if (MP_INFO(x - 1,y).int_6 >= INDUSTRY_L_GET_STEEL)
	    {
	      MP_INFO(x,y).int_4 += INDUSTRY_L_GET_STEEL;
	      MP_INFO(x - 1,y).int_6 -= INDUSTRY_L_GET_STEEL;
	    }
	  else
	    {
	      MP_INFO(x,y).int_4
		+= MP_INFO(x - 1,y).int_6;
	      MP_INFO(x - 1,y).int_6 = 0;
	    }
	  MP_INFO(x,y).int_5 -= INDUSTRY_L_JOBS_LOAD_STEEL;
	}
    }
  /* do we still need some steel? */
  if (MP_INFO(x,y).int_4 < (MAX_STEEL_AT_INDUSTRY_L
			      - INDUSTRY_L_GET_STEEL))
    {
      if ((MP_INFO(x,y - 1).flags & FLAG_IS_TRANSPORT) != 0
	  && MP_INFO(x,y - 1).int_6 > 0)
	{
	  if (MP_INFO(x,y - 1).int_6 >= INDUSTRY_L_GET_STEEL)
	    {
	      MP_INFO(x,y).int_4 += INDUSTRY_L_GET_STEEL;
	      MP_INFO(x,y - 1).int_6 -= INDUSTRY_L_GET_STEEL;
	    }
	  else
	    {
	      MP_INFO(x,y).int_4
		+= MP_INFO(x,y - 1).int_6;
	      MP_INFO(x,y - 1).int_6 = 0;
	    }
	  MP_INFO(x,y).int_5 -= INDUSTRY_L_JOBS_LOAD_STEEL;
	}
    }
  /* now make some goods if there is room in inventory*/
  if (MP_INFO(x,y).int_2 < (MAX_GOODS_AT_INDUSTRY_L
		      - (INDUSTRY_L_MAKE_GOODS * 8)) && MP_INFO(x,y).int_3
      >= INDUSTRY_L_ORE_USED)
    {
      goods += INDUSTRY_L_MAKE_GOODS;
      MP_INFO(x,y).int_3 -= INDUSTRY_L_ORE_USED;
      ore_used += INDUSTRY_L_ORE_USED;
      MP_INFO(x,y).int_5 -= INDUSTRY_L_JOBS_USED;
      /* Pollution is now determined by amount of goods made and affected by
	 technology 
	 MP_POL(x,y) += INDUSTRY_L_POLLUTION; */

      /* multiply by 2 if we have steel. */
      if (MP_INFO(x,y).int_4 >= INDUSTRY_L_STEEL_USED)
	{
	  MP_INFO(x,y).int_4 -= INDUSTRY_L_STEEL_USED;
	  goods += goods;
	}
      /* multipy by 4 if we can get power. */

      if (MP_INFO(x,y).int_3 >= INDUSTRY_L_ORE_USED
	  && get_power (x, y, goods * 10, 1) != 0)
	{
	  goods *= 4;
	  MP_INFO(x,y).flags |= FLAG_POWERED;
	  /* and use more ore */
	  MP_INFO(x,y).int_3 -= INDUSTRY_L_ORE_USED;
	  ore_used += INDUSTRY_L_ORE_USED;
	}
      else
	MP_INFO(x,y).flags &= (0xffffffff - FLAG_POWERED);
    }

  MP_INFO(x,y).int_1 += goods;
  MP_INFO(x,y).int_2 += goods;
  goods_made += goods;

  /* Pollution is based on goods produced and is affected by tech level above
     1000 (displayed as 100) whereupon it is reduced by one percent per 10
     tech_level points.  
  */
  /* XXX: it would be nice to convert tech-reduced air pollution into waste
     to be hauled to a tip or recycled, and reduce that above tl 2000 */

  tmp_pol = (float)(INDUSTRY_L_POL_PER_GOOD * goods);
  if (tech_level > 1000) { 
    double d;
    d = (tech_level - 1000);
    if (d > 1000) 
      d = 1000;
    d /= 1000;
    tmp_pol -= (tmp_pol * d);
    if (tmp_pol < 0) 
      tmp_pol == 0;
  }

  MP_POL(x,y) += (int)tmp_pol;

  /* now sell the goods to the road/rail/track */
  if (MP_GROUP(x,y - 1) == GROUP_ROAD 
      && (MAX_GOODS_ON_ROAD - MP_INFO(x,y - 1).int_4) <= MP_INFO(x,y).int_2)
    {
      MP_INFO(x,y).int_2 -= (MAX_GOODS_ON_ROAD - MP_INFO(x,y - 1).int_4);
      MP_INFO(x,y - 1).int_4 = MAX_GOODS_ON_ROAD;
    }
  else if (MP_GROUP(x,y - 1) == GROUP_RAIL 
	   && (MAX_GOODS_ON_RAIL - MP_INFO(x,y - 1).int_4) 
	   <= MP_INFO(x,y).int_2)
    {
      MP_INFO(x,y).int_2 -= (MAX_GOODS_ON_RAIL
			       - MP_INFO(x,y - 1).int_4);
      MP_INFO(x,y - 1).int_4 = MAX_GOODS_ON_RAIL;
    }
  else if (MP_GROUP(x,y - 1) == GROUP_TRACK 
	   && (MAX_GOODS_ON_TRACK - MP_INFO(x,y - 1).int_4)
	   <= MP_INFO(x,y).int_2)
    {
      MP_INFO(x,y).int_2 -= (MAX_GOODS_ON_TRACK
			       - MP_INFO(x,y - 1).int_4);
      MP_INFO(x,y - 1).int_4 = MAX_GOODS_ON_TRACK;
    }


  if (MP_GROUP(x - 1,y) == GROUP_ROAD 
      && (MAX_GOODS_ON_ROAD - MP_INFO(x - 1,y).int_4)
      <= MP_INFO(x,y).int_2)
    {
      MP_INFO(x,y).int_2 -= (MAX_GOODS_ON_ROAD
			       - MP_INFO(x - 1,y).int_4);
      MP_INFO(x - 1,y).int_4 = MAX_GOODS_ON_ROAD;
    }
  else if (MP_GROUP(x - 1,y) == GROUP_RAIL 
	   && (MAX_GOODS_ON_RAIL - MP_INFO(x - 1,y).int_4)
	   <= MP_INFO(x,y).int_2)
    {
      MP_INFO(x,y).int_2 -= (MAX_GOODS_ON_RAIL
			       - MP_INFO(x - 1,y).int_4);
      MP_INFO(x - 1,y).int_4 = MAX_GOODS_ON_RAIL;
    }
  else if (MP_GROUP(x-1,y) == GROUP_TRACK 
	   && (MAX_GOODS_ON_TRACK - MP_INFO(x - 1,y).int_4)
	   <= MP_INFO(x,y).int_2)
    {
      MP_INFO(x,y).int_2 -= (MAX_GOODS_ON_TRACK
			       - MP_INFO(x - 1,y).int_4);
      MP_INFO(x - 1,y).int_4 = MAX_GOODS_ON_TRACK;
    }

  /* now choose a graphic every month */
  if ((total_time % NUMOF_DAYS_IN_MONTH) == NUMOF_DAYS_IN_MONTH - 1)
    {
      MP_INFO(x,y).int_6 = (MP_INFO(x,y).int_1)
	/ (INDUSTRY_L_MAKE_GOODS * 8);
      MP_INFO(x,y).int_1 = 0;
      if (MP_INFO(x,y).int_6 > 80)
	{
	  switch (MP_TYPE(x,y))
	    {
	    case (CST_INDUSTRY_L_H1):
	    case (CST_INDUSTRY_L_H2):
	    case (CST_INDUSTRY_L_H3):
	    case (CST_INDUSTRY_L_H4):
	      break;
	    default:
	      MP_TYPE(x,y) = CST_INDUSTRY_L_H1;
	    }
	}
      else if (MP_INFO(x,y).int_6 > 55)
	{
	  switch (MP_TYPE(x,y))
	    {
	    case (CST_INDUSTRY_L_M1):
	    case (CST_INDUSTRY_L_M2):
	    case (CST_INDUSTRY_L_M3):
	    case (CST_INDUSTRY_L_M4):
	      break;
	    default:
	      MP_TYPE(x,y) = CST_INDUSTRY_L_M1;
	    }
	}
      else if (MP_INFO(x,y).int_6 > 25)
	{
	  switch (MP_TYPE(x,y))
	    {
	    case (CST_INDUSTRY_L_L1):
	    case (CST_INDUSTRY_L_L2):
	    case (CST_INDUSTRY_L_L3):
	    case (CST_INDUSTRY_L_L4):
	      break;
	    default:
	      MP_TYPE(x,y) = CST_INDUSTRY_L_L1;
	    }
	}
      else if (MP_INFO(x,y).int_6 > 0)
	{
	  switch (MP_TYPE(x,y))
	    {
	    case (CST_INDUSTRY_L_Q1):
	    case (CST_INDUSTRY_L_Q2):
	    case (CST_INDUSTRY_L_Q3):
	    case (CST_INDUSTRY_L_Q4):
	      break;
	    default:
	      MP_TYPE(x,y) = CST_INDUSTRY_L_Q1;
	    }
	}
      else
	MP_TYPE(x,y) = CST_INDUSTRY_L_C;
    }
  /* now animate */
  if (real_time >= MP_INFO(x,y).int_7)
    {
      MP_INFO(x,y).int_7 = real_time + INDUSTRY_L_ANIM_SPEED;
      switch (MP_TYPE(x,y))
	{
	case (CST_INDUSTRY_L_Q1):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_Q2;
	  break;
	case (CST_INDUSTRY_L_Q2):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_Q3;
	  break;
	case (CST_INDUSTRY_L_Q3):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_Q4;
	  break;
	case (CST_INDUSTRY_L_Q4):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_Q1;
	  break;
	case (CST_INDUSTRY_L_L1):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_L2;
	  break;
	case (CST_INDUSTRY_L_L2):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_L3;
	  break;
	case (CST_INDUSTRY_L_L3):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_L4;
	  break;
	case (CST_INDUSTRY_L_L4):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_L1;
	  break;
	case (CST_INDUSTRY_L_M1):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_M2;
	  break;
	case (CST_INDUSTRY_L_M2):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_M3;
	  break;
	case (CST_INDUSTRY_L_M3):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_M4;
	  break;
	case (CST_INDUSTRY_L_M4):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_M1;
	  break;
	case (CST_INDUSTRY_L_H1):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_H2;
	  break;
	case (CST_INDUSTRY_L_H2):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_H3;
	  break;
	case (CST_INDUSTRY_L_H3):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_H4;
	  break;
	case (CST_INDUSTRY_L_H4):
	  MP_TYPE(x,y) = CST_INDUSTRY_L_H1;
	  break;
	}
    }
}


void
mps_light_industry (int x, int y)
{
  int i = 0;
  char * p;

  mps_store_title(i++,_("Light"));
  mps_store_title(i++,_("Industry"));

  i++;

  p = ((MP_INFO(x,y).flags & FLAG_POWERED) != 0) ? _("YES") : _("NO");
  mps_store_ss(i++,_("Power"),p);

  mps_store_sd(i++,_("Output"),MP_INFO(x,y).int_1);

  mps_store_sfp(i++,_("Store"),
		MP_INFO(x,y).int_2 * 100.0 / MAX_GOODS_AT_INDUSTRY_L);
  mps_store_sfp(i++,_("Ore"),
		MP_INFO(x,y).int_3 * 100.0 / MAX_ORE_AT_INDUSTRY_L);
  mps_store_sfp(i++,_("Steel"), 
		MP_INFO(x,y).int_4 * 100.0 / MAX_STEEL_AT_INDUSTRY_L);
  mps_store_sfp(i++,_("Capacity"),
		MP_INFO(x,y).int_6);
}
