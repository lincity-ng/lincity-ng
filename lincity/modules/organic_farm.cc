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
  /* 
     // int_1 is the tech level of the farm when built
     // int_2 is a flag so we don't create a farm with nearly ripe crops.
     // int_3 is the food sold count so far this year.
     // int_4 is the food made last year.
     // int_5 is the random crop rotation key.
     // int_6 is the random month stagger, so they don't all flash at once
     // int_7 is the tech-level dependent output of a powered farm with a full
     // workforce.
  */
  int i;
  if (MP_INFO(x,y).int_5 == 0)	/* this should be done when we create */

    {				/* the area! */

      MP_INFO(x,y).int_5 = (rand () % 4) + 1;
      MP_INFO(x,y).int_6 = rand () % 300;
    }
  MP_INFO(x,y).flags &= (0xffffffff - FLAG_POWERED);
  if (get_jobs (x, y, 1) == 0)
    put_food (x, y, 30);
  else if (get_jobs (x, y, FARM_JOBS_USED) != 0)
    {
      if (get_power (x, y, ORG_FARM_POWER_REC, 0) != 0)
	{
	  if (put_food (x, y, (ORGANIC_FARM_FOOD_OUTPUT
			       + MP_INFO(x,y).int_7)) == 0)
	    put_jobs (x, y, FARM_JOBS_USED);
	  else
	    MP_INFO(x,y).int_3++;
	  MP_INFO(x,y).flags |= FLAG_POWERED;
	}
      else
	{
	  if (put_food (x, y, (ORGANIC_FARM_FOOD_OUTPUT / 4)) == 0)
	    put_jobs (x, y, FARM_JOBS_USED);
	  else
	    MP_INFO(x,y).int_3++;
	}
    }
  else if (get_jobs (x, y, FARM_JOBS_USED / 4) != 0)
    {
      if (get_power (x, y, ORG_FARM_POWER_REC, 0) != 0)
	{
	  if (put_food (x, y, (ORGANIC_FARM_FOOD_OUTPUT
			       + (MP_INFO(x,y).int_7 / 4))) == 0)
	    put_jobs (x, y, FARM_JOBS_USED / 4);
	  else
	    MP_INFO(x,y).int_3++;
	  MP_INFO(x,y).flags |= FLAG_POWERED;
	}
      else
	{
	  if (put_food (x, y, (ORGANIC_FARM_FOOD_OUTPUT / (4 * 4))) == 0)
	    put_jobs (x, y, FARM_JOBS_USED / 4);
	  else
	    MP_INFO(x,y).int_3++;
	}
    }
  else
    {
      if (get_power (x, y, ORG_FARM_POWER_REC, 0) != 0)
	{
	  if (put_food (x, y, (ORGANIC_FARM_FOOD_OUTPUT
			       + (MP_INFO(x,y).int_7 / 8))) != 0)
	    MP_INFO(x,y).int_3++;
	  MP_INFO(x,y).flags |= FLAG_POWERED;
	}
      else if (put_food (x, y, 30
			 + (ORGANIC_FARM_FOOD_OUTPUT / (4 * 8))) != 0)
	MP_INFO(x,y).int_3++;
    }
  if ((total_time & 0x7f) == 0)
    if ((MP_INFO(x,y).flags & FLAG_POWERED) != 0)
      get_waste (x, y, 0x80 * ORG_FARM_WASTE_GET);
  if ((total_time % 1200) == 0)
    {
      MP_INFO(x,y).int_4 = MP_INFO(x,y).int_3;
      MP_INFO(x,y).int_3 = 0;
    }
  i = ((total_time + (MP_INFO(x,y).int_5 * 1200)
	+ MP_INFO(x,y).int_6) % 4800);
  if (i % 300 == 0)
    {
      i /= 300;
      if ( /* MP_INFO(x,y).int_2!=0 &&  */ MP_INFO(x,y).int_4
	  > MIN_FOOD_SOLD_FOR_ANIM)
	{
	  if (i % 4 == 0)
	    {
	      MP_INFO(x,y).int_6 = rand () % 100;
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
#define MPS_INFO_CHARS 30

void
mps_organic_farm (int x, int y)
{
  int i = 0;
  char *p;

  /** removed depency on mps_info */
  mps_store_title(i++,_("Organic Farm"));

  char text[MPS_INFO_CHARS+1];

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


