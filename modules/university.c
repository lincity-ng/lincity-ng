/* ---------------------------------------------------------------------- *
 * university.c
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
#include <university.h>

void
do_university (int x, int y)
{
  /*
     // You need UNIVERSITY_JOBS and UNIVERSITY_GOODS to add 1 to tech_level
     // int_1 is the stored jobs
     // int_2 is the stored goods
     // int_3 is the total tech points made by this uni.
     // int_4 is the count so far in this 100 days
     // int_5 is the count in the last 100 days to give a percent value
     // get some jobs
   */
  if (MP_INFO(x,y).int_1 < UNIVERSITY_JOBS_STORE)
    {
      if (get_jobs (x, y, UNIVERSITY_JOBS + 10) != 0)
	MP_INFO(x,y).int_1 += UNIVERSITY_JOBS + 10;
    }
  /* get goods */
  if (MP_INFO(x,y).int_2 < UNIVERSITY_GOODS_STORE)
    {
      if (get_goods (x, y, UNIVERSITY_GOODS + 10) != 0)
	MP_INFO(x,y).int_2 += UNIVERSITY_GOODS + 10;
      else if (get_goods (x, y, UNIVERSITY_GOODS / 5) != 0)
	MP_INFO(x,y).int_2 += UNIVERSITY_GOODS / 5;
      else if (get_goods (x, y, UNIVERSITY_GOODS / 20) != 0)
	MP_INFO(x,y).int_2 += UNIVERSITY_GOODS / 20;
    }
  /* now do some research */
  if (MP_INFO(x,y).int_1 >= UNIVERSITY_JOBS
      && MP_INFO(x,y).int_2 >= UNIVERSITY_GOODS)
    {
      MP_INFO(x,y).int_1 -= UNIVERSITY_JOBS;
      MP_INFO(x,y).int_2 -= UNIVERSITY_GOODS;
      goods_used += UNIVERSITY_GOODS;
      if (university_intake_rate >= (total_time % 100))
	{
	  MP_INFO(x,y).int_3 += UNIVERSITY_TECH_MADE;
	  tech_level += UNIVERSITY_TECH_MADE;
	  MP_INFO(x,y).int_4++;
	}
    }
  if (total_time % 100 == 0)
    {
      MP_INFO(x,y).int_5 = MP_INFO(x,y).int_4;
      MP_INFO(x,y).int_4 = 0;
    }
  university_cost += UNIVERSITY_RUNNING_COST;
}

void
mps_university (int x, int y)
{
  int i = 0;
  char s[12];

  mps_store_title(i++,_("University"));
  i++;
  mps_store_title(i++,_("Tech Produced"));
  snprintf (s, sizeof(s), "%6.1f", 
	    MP_INFO(x,y).int_3 * 100.0 / MAX_TECH_LEVEL);  
  mps_store_title(i++,s);
  i++;
  mps_store_title(i++,_("Inventory"));
  mps_store_sfp(i++,_("Jobs"), 
		MP_INFO(x,y).int_1 * 100.0 / UNIVERSITY_JOBS_STORE);
  mps_store_sfp(i++,_("Goods"),
		MP_INFO(x,y).int_2 * 100.0 / UNIVERSITY_GOODS_STORE);
  mps_store_sfp(i++,_("Capacity"), MP_INFO(x,y).int_4);
}
