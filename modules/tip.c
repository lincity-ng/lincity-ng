/* ---------------------------------------------------------------------- *
 * tip.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include <lin-city.h>
#include <lctypes.h>
#include <engglobs.h>
#include <cliglobs.h>
#include <stats.h>
#include <tip.h>



void
do_tip (int x, int y)
{
  /*
     // int_1 is the amount of waste on the site.
     // int_2 if the amount that has flowed in so far this month
     // int_3 is the amount stored last month.
     // int_4 counts up starting when tip fills, controlling how
              long until the land is useful again.
   */
  int i;

/* XXX: put this in a header somewhere */

/* If the tip is full, age it until it degrades into useful soil */

  if (MP_TYPE(x,y) == CST_TIP_8) {
      MP_INFO(x,y).int_4++;
      if (MP_INFO(x,y).int_4 >= TIP_DEGRADE_TIME) {
	  do_bulldoze_area(CST_GREEN,x,y);
      }
      return;
  }

  /* just grab as much as we can from transport */
  if (x > 0 && (MP_INFO(x - 1,y).flags & FLAG_IS_TRANSPORT) != 0)
    {
      i = MP_INFO(x - 1,y).int_7 / 10;
      MP_INFO(x,y).int_1 += i;
      MP_INFO(x,y).int_2 += i;
      MP_INFO(x - 1,y).int_7 -= i * 10;
      sust_dig_ore_coal_tip_flag = 0;
    }
  if (y > 0 && (MP_INFO(x,y - 1).flags & FLAG_IS_TRANSPORT) != 0)
    {
      i = MP_INFO(x,y - 1).int_7 / 10;
      MP_INFO(x,y).int_1 += i;
      MP_INFO(x,y).int_2 += i;
      MP_INFO(x,y - 1).int_7 -= i * 10;
      sust_dig_ore_coal_tip_flag = 0;
    }

#if defined (commentout)
  /* Increment the "ore" reserve; this prevents a new tip from being
     built on top of a degraded one. */
  MP_INFO(x,y).ore_reserve++;
#endif

  /* now choose an icon. */
  if ((total_time % NUMOF_DAYS_IN_MONTH) == 0)
    {
      i = (MP_INFO(x,y).int_1 * 7) / MAX_WASTE_AT_TIP;
      if (MP_INFO(x,y).int_1 > 0)
	i++;
      switch (i)
	{
	case (0):
	  MP_TYPE(x,y) = CST_TIP_0;
	  break;
	case (1):
	  MP_TYPE(x,y) = CST_TIP_1;
	  break;
	case (2):
	  MP_TYPE(x,y) = CST_TIP_2;
	  break;
	case (3):
	  MP_TYPE(x,y) = CST_TIP_3;
	  break;
	case (4):
	  MP_TYPE(x,y) = CST_TIP_4;
	  break;
	case (5):
	  MP_TYPE(x,y) = CST_TIP_5;
	  break;
	case (6):
	  MP_TYPE(x,y) = CST_TIP_6;
	  break;
	case (7):
	  MP_TYPE(x,y) = CST_TIP_7;
	  break;
	case (8):
	  MP_TYPE(x,y) = CST_TIP_8;
	  MP_INFO(x,y).int_2 = 0;
	  MP_INFO(x,y).int_4 = 0;
	  break;

	}
      MP_INFO(x,y).int_3 = MP_INFO(x,y).int_2;
      MP_INFO(x,y).int_2 = 0;
    }
}
