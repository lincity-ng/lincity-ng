/* ---------------------------------------------------------------------- *
 * heavy_industry.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "heavy_industry.h"


void
do_industry_h (int x, int y)
{
  int rawm, steel = 0;
  /*
     // int_1 is the steel produced this month so far
     // int_2 is the amount of steel in store
     // int_3 is the amount of raw materials in store (ore)
     // int_4 is the coal in store
     // int_5 is the percent max production last month
     // int_6 is the time of the next animation frame.
     // int_7 is whether we get power from coal (1) or elsewhere (0)
   */

  /* See if there's any raw materials (ore) on the road/rail. If so, use some
     jobs to get it.  First get some ore... 
  */
  if (MP_INFO(x,y).int_3 < MAX_ORE_AT_INDUSTRY_H
      && ((MP_INFO(x - 1,y).flags & FLAG_IS_TRANSPORT)
	  != 0) && MP_INFO(x - 1,y).int_5 > 0)
    if (get_jobs (x, y, JOBS_LOAD_ORE) != 0)
      {
	MP_INFO(x,y).int_3 += (MP_INFO(x - 1,y).int_5 / 2
				 + ((MP_INFO(x - 1,y).int_5) % 2));
	MP_INFO(x - 1,y).int_5 /= 2;
      }
  if (MP_INFO(x,y).int_3 < MAX_ORE_AT_INDUSTRY_H
      && ((MP_INFO(x,y - 1).flags & FLAG_IS_TRANSPORT)
	  != 0) && MP_INFO(x,y - 1).int_5 > 0)
    if (get_jobs (x, y, JOBS_LOAD_ORE) != 0)
      {
	MP_INFO(x,y).int_3 += (MP_INFO(x,y - 1).int_5 / 2
				 + ((MP_INFO(x,y - 1).int_5) % 2));
	MP_INFO(x,y - 1).int_5 /= 2;
      }
  /* then get some coal if needed */
  if (MP_INFO(x,y).int_4 < MAX_COAL_AT_INDUSTRY_H
      && ((MP_INFO(x - 1,y).flags & FLAG_IS_TRANSPORT)
	  != 0) && MP_INFO(x - 1,y).int_3 > 0)
    if (get_jobs (x, y, JOBS_LOAD_COAL) != 0)
      {
	MP_INFO(x,y).int_4 += (MP_INFO(x - 1,y).int_3 / 2
				 + ((MP_INFO(x - 1,y).int_3) % 2));
	MP_INFO(x - 1,y).int_3 /= 2;
      }
  if (MP_INFO(x,y).int_4 < MAX_ORE_AT_INDUSTRY_H
      && ((MP_INFO(x,y - 1).flags & FLAG_IS_TRANSPORT)
	  != 0) && MP_INFO(x,y - 1).int_3 > 0)
    if (get_jobs (x, y, JOBS_LOAD_COAL) != 0)
      {
	MP_INFO(x,y).int_4 += (MP_INFO(x,y - 1).int_3 / 2
				 + ((MP_INFO(x,y - 1).int_3) % 2));
	MP_INFO(x,y - 1).int_3 /= 2;
      }

  rawm = MP_INFO(x,y).int_3;
  if (rawm > MAX_MADE_AT_INDUSTRY_H)
    rawm = MAX_MADE_AT_INDUSTRY_H;
  /* turn it into steel */
  if (rawm > 0 && MP_INFO(x,y).int_2 < (MAX_STEEL_ON_RAIL * 10))
    {
      if (get_jobs (x, y, ((rawm / 2) / JOBS_MAKE_STEEL) + 1) != 0)
	{
	  steel = (rawm) / ORE_MAKE_STEEL;
	  MP_POL(x,y) += INDUSTRY_H_POLLUTION / 2;
	}
      else if (get_jobs (x, y, ((rawm / 4) / JOBS_MAKE_STEEL) + 1) != 0)
	{
	  steel = (rawm / 2) / ORE_MAKE_STEEL;
	  MP_POL(x,y) += INDUSTRY_H_POLLUTION / 4;
	}
      else if (get_jobs (x, y, ((rawm / 10) / JOBS_MAKE_STEEL) + 1) != 0)
	{
	  steel = (rawm / 5) / ORE_MAKE_STEEL;
	  MP_POL(x,y) += INDUSTRY_H_POLLUTION / 10;
	}
    }
  /* do this here rather than later 'cos maybe steel/=5 */
  MP_INFO(x,y).int_3 -= steel * ORE_MAKE_STEEL;
  ore_used += steel * ORE_MAKE_STEEL;
  /* check there was enough electricity, or back up to 1/10 of the 
     production. ie same work and material useage for less production. 
     If no real power, see if we have enough coal to generate electricity.
  */
  if (get_power (x, y, steel * POWER_MAKE_STEEL, 1) == 0)
    {
      if (MP_INFO(x,y).int_4 < (steel * 2))
	{
	  MP_INFO(x,y).flags &= (0xffffffff - FLAG_POWERED);
	  steel /= 5;
	}
      else
	{
	  MP_INFO(x,y).int_4 -= (steel * 2);
	  coal_used += (steel * 2);
	  MP_INFO(x,y).flags |= FLAG_POWERED;
	  MP_INFO(x,y).int_7 = 1;
	}
    }
  else 
    {
      MP_INFO(x,y).flags |= FLAG_POWERED;
      MP_INFO(x,y).int_7 = 0;
    }
  MP_INFO(x,y).int_1 += steel;
  MP_INFO(x,y).int_2 += steel;
  /* now sell the steel to the road/rail */
  if (MP_GROUP(x,y-1) == GROUP_ROAD 
      && (MAX_STEEL_ON_ROAD - MP_INFO(x,y - 1).int_6)
      <= MP_INFO(x,y).int_2)
    {
      MP_INFO(x,y).int_2 -= (MAX_STEEL_ON_ROAD
			       - MP_INFO(x,y - 1).int_6);
      MP_INFO(x,y - 1).int_6 = MAX_STEEL_ON_ROAD;
    }
  else if (MP_GROUP(x,y - 1) == GROUP_RAIL 
	   && (MAX_STEEL_ON_RAIL - MP_INFO(x,y - 1).int_6)
	   <= MP_INFO(x,y).int_2)
    {
      MP_INFO(x,y).int_2 -= (MAX_STEEL_ON_RAIL
			       - MP_INFO(x,y - 1).int_6);
      MP_INFO(x,y - 1).int_6 = MAX_STEEL_ON_RAIL;
    }
  else if (MP_GROUP(x,y-1) == GROUP_TRACK 
	   && (MAX_STEEL_ON_TRACK - MP_INFO(x,y - 1).int_6)
	   <= MP_INFO(x,y).int_2)
    {
      MP_INFO(x,y).int_2 -= (MAX_STEEL_ON_TRACK
			       - MP_INFO(x,y - 1).int_6);
      MP_INFO(x,y - 1).int_6 = MAX_STEEL_ON_TRACK;
    }

  if (MP_GROUP(x-1,y) == GROUP_ROAD 
      && (MAX_STEEL_ON_ROAD - MP_INFO(x - 1,y).int_6)
      <= MP_INFO(x,y).int_2)
    {
      MP_INFO(x,y).int_2 -= (MAX_STEEL_ON_ROAD
			       - MP_INFO(x - 1,y).int_6);
      MP_INFO(x - 1,y).int_6 = MAX_STEEL_ON_ROAD;
    }
  else if (MP_GROUP(x-1,y) == GROUP_RAIL 
	   && (MAX_STEEL_ON_RAIL - MP_INFO(x - 1,y).int_6)
	   <= MP_INFO(x,y).int_2)
    {
      MP_INFO(x,y).int_2 -= (MAX_STEEL_ON_RAIL
			       - MP_INFO(x - 1,y).int_6);
      MP_INFO(x - 1,y).int_6 = MAX_STEEL_ON_RAIL;
    }
  else if (MP_GROUP(x - 1,y) == GROUP_TRACK 
	   && (MAX_STEEL_ON_TRACK - MP_INFO(x - 1,y).int_6)
	   <= MP_INFO(x,y).int_2)
    {
      MP_INFO(x,y).int_2 -= (MAX_STEEL_ON_TRACK
			       - MP_INFO(x - 1,y).int_6);
      MP_INFO(x - 1,y).int_6 = MAX_STEEL_ON_TRACK;
    }


  /* now choose a graphic every month */
  if ((total_time % NUMOF_DAYS_IN_MONTH) == NUMOF_DAYS_IN_MONTH - 1)
    {
      MP_INFO(x,y).int_5 = MP_INFO(x,y).int_1
	/ (MAX_MADE_AT_INDUSTRY_H / ORE_MAKE_STEEL);
      MP_INFO(x,y).int_1 = 0;
      if (MP_INFO(x,y).int_5 > 80)
	{
	  switch (MP_TYPE(x,y))
	    {
	    case (CST_INDUSTRY_H_H1):
	    case (CST_INDUSTRY_H_H2):
	    case (CST_INDUSTRY_H_H3):
	    case (CST_INDUSTRY_H_H4):
	    case (CST_INDUSTRY_H_H5):
	    case (CST_INDUSTRY_H_H6):
	    case (CST_INDUSTRY_H_H7):
	    case (CST_INDUSTRY_H_H8):
	      break;
	    default:
	      MP_TYPE(x,y) = CST_INDUSTRY_H_H1;
	    }
	}
      else if (MP_INFO(x,y).int_5 > 30)
	{
	  switch (MP_TYPE(x,y))
	    {
	    case (CST_INDUSTRY_H_M1):
	    case (CST_INDUSTRY_H_M2):
	    case (CST_INDUSTRY_H_M3):
	    case (CST_INDUSTRY_H_M4):
	    case (CST_INDUSTRY_H_M5):
	    case (CST_INDUSTRY_H_M6):
	    case (CST_INDUSTRY_H_M7):
	    case (CST_INDUSTRY_H_M8):
	      break;
	    default:
	      MP_TYPE(x,y) = CST_INDUSTRY_H_M1;
	    }
	}
      else if (MP_INFO(x,y).int_5 > 0)
	{
	  switch (MP_TYPE(x,y))
	    {
	    case (CST_INDUSTRY_H_L1):
	    case (CST_INDUSTRY_H_L2):
	    case (CST_INDUSTRY_H_L3):
	    case (CST_INDUSTRY_H_L4):
	    case (CST_INDUSTRY_H_L5):
	    case (CST_INDUSTRY_H_L6):
	    case (CST_INDUSTRY_H_L7):
	    case (CST_INDUSTRY_H_L8):
	      break;
	    default:
	      MP_TYPE(x,y) = CST_INDUSTRY_H_L1;
	    }
	}
      else
	MP_TYPE(x,y) = CST_INDUSTRY_H_C;
    }
  /* now animate */
  if (real_time >= MP_INFO(x,y).int_6)
    {
      MP_INFO(x,y).int_6 = real_time + INDUSTRY_H_ANIM_SPEED;
      switch (MP_TYPE(x,y))
	{
	case (CST_INDUSTRY_H_L1):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_L2;
	  break;
	case (CST_INDUSTRY_H_L2):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_L3;
	  break;
	case (CST_INDUSTRY_H_L3):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_L4;
	  break;
	case (CST_INDUSTRY_H_L4):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_L5;
	  break;
	case (CST_INDUSTRY_H_L5):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_L6;
	  break;
	case (CST_INDUSTRY_H_L6):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_L7;
	  break;
	case (CST_INDUSTRY_H_L7):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_L8;
	  break;
	case (CST_INDUSTRY_H_L8):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_L1;
	  break;

	case (CST_INDUSTRY_H_M1):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_M2;
	  break;
	case (CST_INDUSTRY_H_M2):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_M3;
	  break;
	case (CST_INDUSTRY_H_M3):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_M4;
	  break;
	case (CST_INDUSTRY_H_M4):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_M5;
	  break;
	case (CST_INDUSTRY_H_M5):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_M6;
	  break;
	case (CST_INDUSTRY_H_M6):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_M7;
	  break;
	case (CST_INDUSTRY_H_M7):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_M8;
	  break;
	case (CST_INDUSTRY_H_M8):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_M1;
	  break;

	case (CST_INDUSTRY_H_H1):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_H2;
	  break;
	case (CST_INDUSTRY_H_H2):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_H3;
	  break;
	case (CST_INDUSTRY_H_H3):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_H4;
	  break;
	case (CST_INDUSTRY_H_H4):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_H5;
	  break;
	case (CST_INDUSTRY_H_H5):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_H6;
	  break;
	case (CST_INDUSTRY_H_H6):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_H7;
	  break;
	case (CST_INDUSTRY_H_H7):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_H8;
	  break;
	case (CST_INDUSTRY_H_H8):
	  MP_TYPE(x,y) = CST_INDUSTRY_H_H1;
	  break;

	}
    }
}


void
mps_heavy_industry (int x, int y)
{
    int i = 0;
    char * p;
  
    mps_store_title(i++,_("Heavy"));
    mps_store_title(i++,_("Industry"));
    i++;

    if ((MP_INFO(x,y).flags & FLAG_POWERED) != 0) {
	if (MP_INFO(x,y).int_7 == 1) {
	    p = _("Coal");
	} else {
	    p = _("Grid");
	}
    } else {
	p = _("NO");
    }

    mps_store_ss(i++,_("Power"),p);

    mps_store_sd(i++,_("Output"), MP_INFO(x,y).int_1);
    mps_store_sfp(i++,_("Store"), 
		 MP_INFO(x,y).int_2 * 100.0 / MAX_STEEL_AT_INDUSTRY_H);
    mps_store_sfp(i++,_("Ore"), 
		 MP_INFO(x,y).int_3 * 100.0 / MAX_ORE_AT_INDUSTRY_H);
    mps_store_sfp(i++,_("Coal"),
		 MP_INFO(x,y).int_4 * 100.0 / MAX_COAL_AT_INDUSTRY_H);
    mps_store_sd(i++,_("Capacity"), MP_INFO(x,y).int_5);
}
