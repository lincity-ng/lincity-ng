/* ---------------------------------------------------------------------- *
 * commune.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include <lin-city.h>
#include <lctypes.h>
#include <engglobs.h>
#include <cliglobs.h>
#include <stats.h>
#include <commune.h>


void
do_commune (int x, int y)
{
  /*
    // int_1 is the animation trigger time
    // int_2 is the steelflag/trackflag
    // int_3 is the coal sold in the last 100 days 200 units is 100%
    // steel adds more.
    // int_4 is the months without selling much coal,steel,ore
    // int_5 is the coal, ore, steel waste flags for last month
    // int_6 is the coal, ore, steel waste flags so far this month
  */
  /* GCS -- I folded the trackflag into int_2, changing the logic slightly.
     This change only affects the animation. */
  int trackflag = 0;
  /* stick coal and ore on tracks, in SMALL doses. */
  if (put_coal (x, y, 2) != 0)
    {
      trackflag = 1;
      MP_INFO(x,y).int_3++;
      MP_INFO(x,y).int_6 |= 1;
    }
  if (put_ore (x, y, 6) != 0)
    {
      trackflag = 1;
      MP_INFO(x,y).int_3++;
      MP_INFO(x,y).int_6 |= 2;
    }
  /* recycle a bit of waste */
  if (get_waste (x, y, 20) != 0)
    {
      trackflag = 1;
      MP_INFO(x,y).int_3++;
      MP_INFO(x,y).int_6 |= 8;
    }
  if (total_time % 10 == 0)
    {
      MP_INFO(x,y).int_2 = 1;
      if (put_steel (x, y, 2) != 0) {
	  MP_INFO(x,y).int_3++;
	  MP_INFO(x,y).int_6 |= 4;
      } else {
	MP_INFO(x,y).int_2 = 0;
      }
      if (trackflag) {
	MP_INFO(x,y).int_2 = 0;
      }
    }
  if (total_time % 100 == 48)
    {
      MP_INFO(x,y).int_5 = MP_INFO(x,y).int_6;
      MP_INFO(x,y).int_6 = 0;
      if (MP_INFO(x,y).int_2 == 0 || trackflag == 0)
	{
	  if (MP_TYPE(x,y) > CST_COMMUNE_7)
	    MP_TYPE(x,y) -= 7;
	}
      else if (MP_TYPE(x,y) <= CST_COMMUNE_7)
	MP_TYPE(x,y) += 7;
      if (MP_INFO(x,y).int_3 > 0)	/*  >0% */
	{
	  MP_INFO(x,y).int_3 = 0;
	  if (--MP_INFO(x,y).int_4 < 0)
	    MP_INFO(x,y).int_4 = 0;
	}
      else
	{
	  MP_INFO(x,y).int_3 = 0;
	  MP_INFO(x,y).int_4++;
	  if (MP_INFO(x,y).int_4 > 120)	/* 10 years */
	    {
	      do_bulldoze_area (CST_PARKLAND_PLANE, x, y);
	      return;
	    }
	}
    }
  /* animate */
  if (real_time >= MP_INFO(x,y).int_1)
    {
      MP_INFO(x,y).int_1 = real_time + COMMUNE_ANIM_SPEED;
      switch (MP_TYPE(x,y))
	{
	case (CST_COMMUNE_1):
	  MP_TYPE(x,y) = CST_COMMUNE_2;
	  break;
	case (CST_COMMUNE_2):
	  MP_TYPE(x,y) = CST_COMMUNE_3;
	  break;
	case (CST_COMMUNE_3):
	  MP_TYPE(x,y) = CST_COMMUNE_4;
	  break;
	case (CST_COMMUNE_4):
	  MP_TYPE(x,y) = CST_COMMUNE_5;
	  break;
	case (CST_COMMUNE_5):
	  MP_TYPE(x,y) = CST_COMMUNE_6;
	  break;
	case (CST_COMMUNE_6):
	  MP_TYPE(x,y) = CST_COMMUNE_1;
	  break;
	case (CST_COMMUNE_8):
	  MP_TYPE(x,y) = CST_COMMUNE_9;
	  break;
	case (CST_COMMUNE_9):
	  MP_TYPE(x,y) = CST_COMMUNE_10;
	  break;
	case (CST_COMMUNE_10):
	  MP_TYPE(x,y) = CST_COMMUNE_11;
	  break;
	case (CST_COMMUNE_11):
	  MP_TYPE(x,y) = CST_COMMUNE_12;
	  break;
	case (CST_COMMUNE_12):
	  MP_TYPE(x,y) = CST_COMMUNE_13;
	  break;
	case (CST_COMMUNE_13):
	  MP_TYPE(x,y) = CST_COMMUNE_8;
	  break;

	}
    }

}
