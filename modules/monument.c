/* ---------------------------------------------------------------------- *
 * monument.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "monument.h"

void
do_monument (int x, int y)
{
  /*
     // int_1 holds the jobs used
     // int_2 holds the tech points made
     // int_3 holds the tail off count
   */
  if (MP_INFO(x,y).int_1 < BUILD_MONUMENT_JOBS)
    if (get_jobs (x, y, MONUMENT_GET_JOBS) != 0)
      MP_INFO(x,y).int_1 += MONUMENT_GET_JOBS;

  /* now choose a graphic */
  if (MP_INFO(x,y).int_1 >= BUILD_MONUMENT_JOBS)
    {
      MP_TYPE(x,y) = CST_MONUMENT_5;
      /* inc tech level only if fully built and tech less 
         than MONUMENT_TECH_EXPIRE */
      if (tech_level < (MONUMENT_TECH_EXPIRE * 1000)
	  && (total_time % MONUMENT_DAYS_PER_TECH) == 1)
	{
	  if (MP_INFO(x,y).int_3++ > (tech_level / 10000) - 2)
	    {
	      tech_level++;
	      MP_INFO(x,y).int_2++;
	      MP_INFO(x,y).int_3 = 0;
	    }
	}
    }
  else if (MP_INFO(x,y).int_1 >= ((BUILD_MONUMENT_JOBS * 4) / 5))
    MP_TYPE(x,y) = CST_MONUMENT_4;
  else if (MP_INFO(x,y).int_1 >= ((BUILD_MONUMENT_JOBS * 3) / 5))
    MP_TYPE(x,y) = CST_MONUMENT_3;
  else if (MP_INFO(x,y).int_1 >= ((BUILD_MONUMENT_JOBS * 2) / 5))
    MP_TYPE(x,y) = CST_MONUMENT_2;
  else if (MP_INFO(x,y).int_1 >= (BUILD_MONUMENT_JOBS / 20))
    MP_TYPE(x,y) = CST_MONUMENT_1;
  else
    MP_TYPE(x,y) = CST_MONUMENT_0;
}

void
mps_monument (int x, int y)
{
  int i = 0;

  mps_store_title(i++,_("Monument"));
  i++;
  i++;

  /* Display tech contribution only after monument is complete */
  if ((MP_INFO(x,y).int_1 * 100 / BUILD_MONUMENT_JOBS) >= 100) {
      mps_store_title(i++,_("Wisdom Bestowed"));
      i++;
      mps_store_f(i++, MP_INFO(x,y).int_2 * 100.0 / MAX_TECH_LEVEL);
  } else {
      mps_store_title(i++,_("% Complete"));
      i++;
      mps_store_fp(i++, MP_INFO(x,y).int_1 * 100.0 /
		   BUILD_MONUMENT_JOBS);
  }
}
