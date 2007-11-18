/* ---------------------------------------------------------------------- *
 * organic_farm.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "../lcconfig.h"
#include "organic_farm.h"
#include "lin-city.h"

#include <stdlib.h>

void
do_organic_farm (int x, int y)
{
  /* // MP_INFO(x,y)
     // int_1 is the tech level of the farm when built
     // int_2 is a flag so we don't create a farm with nearly ripe crops.
      *      unused in NG1.1	
     // int_3 is the food sold count so far this year.
     // int_4 is the food made last year.
     // int_5 is the random crop rotation key.
     // int_6 is the random month stagger, so they don't all flash at once
     // int_7 is the jobs stored at the farm 
     // 	up to NG-1.1.0 int_7 was the tech-level dependent output of a
     // 	 powered farm with full workforce. 
     // 	= duplicate with int_1.  see below tech_bonus.
      * 
      * MP_INFO(x+1,y) stores additional info
      *    int_1 reserved (=x)
      *    int_2 reserved (=y)
      *    int_3 max possible production (assuming 100% water and power)
      *    int_4 number of 1x1 tiles with underground water inside the farm
      *    int_5 current production
  */
  int i;
  int has_power = false;
  int tech_bonus = (int)(((double) MP_INFO(x,y).int_1
			      * ORGANIC_FARM_FOOD_OUTPUT) / MAX_TECH_LEVEL);
  MP_INFO(x+1,y).int_3 = ORGANIC_FARM_FOOD_OUTPUT + tech_bonus;
  /* Animation */
  if (MP_INFO(x,y).int_5 == 0) {
      /* this should be done when we create the area! */
      MP_INFO(x,y).int_5 = (rand () % 4) + 1;
      MP_INFO(x,y).int_6 = rand () % 300; /* AL1 will be sooner or later redefined as %100. see below */
  }

  /* check jobs */
  if (MP_INFO(x,y).int_7 < FARM_JOBS_USED) {
  	if (get_jobs (x, y, FARM_JOBS_USED) != 0)
		MP_INFO(x,y).int_7 += FARM_JOBS_USED;
	/* adding if (get_jobs ... /2) would allow to have some jobs stored at farm,
	 * so would smooth the behavior and make farms more resistant to job penury.
	 * Currently keep previous behavior.
	 */
  	else if (get_jobs (x, y, FARM_JOBS_USED / 4) != 0)
		MP_INFO(x,y).int_7 += FARM_JOBS_USED / 4;
  	else if (get_jobs (x, y, 1) != 0)
		MP_INFO(x,y).int_7 += 1;
  }

  /* check power */
  MP_INFO(x,y).flags &= (0xffffffff - FLAG_POWERED);
  if (MP_INFO(x,y).int_7 >= 1) {
  	/* There are jobs to do some production, so check for power */
	if (get_power (x, y, ORG_FARM_POWER_REC, 0) != 0) {
		MP_INFO(x,y).flags |= FLAG_POWERED;
		has_power = true;
	}
  }

  /* Produce some food */
  int prod = 0;
  if (MP_INFO(x,y).int_7 >= FARM_JOBS_USED) {
      if (has_power) {
	      	 prod = ORGANIC_FARM_FOOD_OUTPUT + tech_bonus;
      } else {
	      	 prod = ORGANIC_FARM_FOOD_OUTPUT / 4;
      }
  } else if (MP_INFO(x,y).int_7 >= FARM_JOBS_USED / 4) {
      if (has_power) {
	      	 prod = ORGANIC_FARM_FOOD_OUTPUT + tech_bonus / 4;
      } else {
	      	 prod = ORGANIC_FARM_FOOD_OUTPUT / (4 * 4);
      }
  } else if (MP_INFO(x,y).int_7 >= 1) {
      /* got 1 job */
      if (has_power) {
	     	prod = ORGANIC_FARM_FOOD_OUTPUT + tech_bonus / 8;
      } else {
	        /* AL1 "small ouch":
	         * without power output with 1 job is bigger than output with 3 !
	         * 3 = FARMS_JOBS_USED / 4 
	         * ORGANIC_FARM_FOOD_OUTPUT = 550 currently (ng_1.1)
	         */
	         prod = 30 + ORGANIC_FARM_FOOD_OUTPUT / (4 * 8);
      }
  } else {
            /* AL1 : the farm gives very small amount of food without job. 
             *  ? Probably needed for start ?
             *  ? Useful to prevent starvation when no jobs ? 
             *  The various buildings are "done" in random order,
             *  so it should be ok without this.
             */
             put_food (x, y, 30);
             /* note that this does not generate revenu int_3) */
  }
  /* Check underground water, and reduce production accordingly */
  if (use_waterwell) {
      int w = 0;
      int n = 0;
      for (int i = 0; i < MP_SIZE(x,y); i++) {
          for (int j = 0; j < MP_SIZE(x,y); j++) {
              n++;
              if (HAS_UGWATER(x+i,y+j))
                  w++;
          }
      }
      prod = (prod * w)/n;
      MP_INFO(x + 1, y).int_4 = w;
  }
  MP_INFO(x + 1, y).int_5 = prod;
                 	 
  if (prod != 0) {
     if (put_food (x, y, prod) != 0) {
	    	MP_INFO(x,y).int_3++;
		    MP_INFO(x,y).int_7 -= 1;
     }
  }

  if ((total_time & 0x7f) == 0)
    if ((MP_INFO(x,y).flags & FLAG_POWERED) != 0)
      get_waste (x, y, 0x80 * ORG_FARM_WASTE_GET);

  if ((total_time % 1200) == 0) {
      MP_INFO(x,y).int_4 = MP_INFO(x,y).int_3;
      MP_INFO(x,y).int_3 = 0;
  }

  i = (total_time + MP_INFO(x,y).int_5 * 1200 + MP_INFO(x,y).int_6) % 4800;

  if (i % 300 == 0)
    {
      i /= 300;
      if ( /* MP_INFO(x,y).int_2!=0 &&  */ MP_INFO(x,y).int_4
	  > MIN_FOOD_SOLD_FOR_ANIM)
	{
	  if (i % 4 == 0)
	    {
	      MP_INFO(x,y).int_6 = rand () % 100; /* AL1: initially defined as %300 */
	    }
	  switch (i)
	    {
	    case (0):
	      MP_TYPE(x,y) = CST_FARM_O3;
	      break;
	    case (1):
	      MP_TYPE(x,y) = CST_FARM_O3;
	      break;
	    case (2):
	      MP_TYPE(x,y) = CST_FARM_O3;
	      break;
	    case (3):
	      MP_TYPE(x,y) = CST_FARM_O3;
	      break;
	    case (4):
	      MP_TYPE(x,y) = CST_FARM_O7;
	      break;
	    case (5):
	      MP_TYPE(x,y) = CST_FARM_O7;
	      break;
	    case (6):
	      MP_TYPE(x,y) = CST_FARM_O7;
	      break;
	    case (7):
	      MP_TYPE(x,y) = CST_FARM_O7;
	      break;
	    case (8):
	      MP_TYPE(x,y) = CST_FARM_O11;
	      break;
	    case (9):
	      MP_TYPE(x,y) = CST_FARM_O11;
	      break;
	    case (10):
	      MP_TYPE(x,y) = CST_FARM_O11;
	      break;
	    case (11):
	      MP_TYPE(x,y) = CST_FARM_O11;
	      break;
	    case (12):
	      MP_TYPE(x,y) = CST_FARM_O15;
	      break;
	    case (13):
	      MP_TYPE(x,y) = CST_FARM_O15;
	      break;
	    case (14):
	      MP_TYPE(x,y) = CST_FARM_O15;
	      break;
	    case (15):
	      MP_TYPE(x,y) = CST_FARM_O15;
	      break;

	    }
	}
      else
	{
	  MP_TYPE(x,y) = CST_FARM_O0;
	}
    }
}

/* FIXME: this isn't correct */
#ifdef MPS_INFO_CHARS
#undef MPS_INFO_CHARS
#endif
#define MPS_INFO_CHARS 30

void
mps_organic_farm (int x, int y)
{
  int i = 0;
  const char *p;
  char text[MPS_INFO_CHARS+1];

  /** removed depency on mps_info */
  mps_store_title(i++,_("Organic Farm"));
  i++;

  if ((MP_INFO(x,y).flags & FLAG_POWERED) != 0)
    p = _("YES");
  else
    p = _("NO ");
  
  snprintf(text, MPS_INFO_CHARS, "%s %s", _("Power"), p);
  mps_store_title(i++,text);

  snprintf(text, MPS_INFO_CHARS, "%s  %5.1f%%", _("Tech"), 
	   MP_INFO(x,y).int_1 * 100.0 / MAX_TECH_LEVEL);
  mps_store_title(i++,text);

  snprintf(text, MPS_INFO_CHARS, "%s  %5.1f%%", _("Prod"),
	   MP_INFO(x,y).int_4 * 100.0 / 1200.0);
  mps_store_title(i++,text);

  if (use_waterwell) {
      i++;
      mps_store_title(i++,_("Debug info"));
      mps_store_sd(i++,_(" max with power & water"), MP_INFO(x + 1, y).int_3);
      mps_store_sd(i++,_(" number of tile with water"), MP_INFO(x + 1, y).int_4);
      mps_store_sd(i++,_(" current production"), MP_INFO(x + 1, y).int_5);
  }

  /*
  char * p;

  snprintf(mps_info[i++], MPS_INFO_CHARS, _("Organic Farm"));
  i++;

  if ((MP_INFO(x,y).flags & FLAG_POWERED) != 0)
    p = _("YES");
  else
    p = _("NO ");

  snprintf(mps_info[i++], MPS_INFO_CHARS, "%s %s", _("Power"), p);

  snprintf(mps_info[i++], MPS_INFO_CHARS, "%s  %5.1f%%", _("Tech"), 
	   MP_INFO(x,y).int_1 * 100.0 / MAX_TECH_LEVEL);

  snprintf(mps_info[i++], MPS_INFO_CHARS, "%s  %5.1f%%", _("Prod"),
	   MP_INFO(x,y).int_4 * 100.0 / 1200.0);

  */
}


