/* ---------------------------------------------------------------------- *
 * port.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include <lin-city.h>
#include <lctypes.h>
#include <lcintl.h>
#include <lcconfig.h>
#include <lclib.h>
#include <engglobs.h>
#include <cliglobs.h>
#include <stats.h>
#include <mps.h>
#include <port.h>


void
do_port (int x, int y)
{
  /*
     // int_1 is the money made so far this month
     // int_2 is the money made last month
     // int_3 holds the 'pence/pennies/bits' to add next time round.
     // int_4 is the import costs so far this month
     // int_5 is the import costs for last month
     // Use int_3 to int_7 of (x+1,y) to hold the individual buy values
     //                       (x,y+1) is last month's
     // Use int_3 to int_7 of (x+2,y) to hold the individual sell values
     //                       (x,y+2) is last month's
   */
  int i, et = 0, ic = 0, flags, *b1, *b2, *s1, *s2;
  /* left connection first */
  flags = MP_INFO(x,y).flags;
  if (x > 0 && (MP_INFO(x - 1,y).flags
		& FLAG_IS_TRANSPORT) != 0)
    {
      if ((flags & FLAG_MB_FOOD) != 0)
	{
	  i = buy_food (x - 1, y);
	  ic += i;
	  MP_INFO(x + 1,y).int_3 += i;
	}
      if ((flags & FLAG_MS_FOOD) != 0)
	{
	  i = sell_food (x - 1, y);
	  et += i;
	  MP_INFO(x + 2,y).int_3 += i;
	}
      if ((flags & FLAG_MB_COAL) != 0)
	{
	  i = buy_coal (x - 1, y);
	  ic += i;
	  MP_INFO(x + 1,y).int_4 += i;
	}
      if ((flags & FLAG_MS_COAL) != 0)
	{
	  i = sell_coal (x - 1, y);
	  et += i;
	  MP_INFO(x + 2,y).int_4 += i;
	}
      if ((flags & FLAG_MB_ORE) != 0)
	{
	  i = buy_ore (x - 1, y);
	  ic += i;
	  MP_INFO(x + 1,y).int_5 += i;
	}
      if ((flags & FLAG_MS_ORE) != 0)
	{
	  i = sell_ore (x - 1, y);
	  et += i;
	  MP_INFO(x + 2,y).int_5 += i;
	}
      if ((flags & FLAG_MB_GOODS) != 0)
	{
	  i = buy_goods (x - 1, y);
	  ic += i;
	  MP_INFO(x + 1,y).int_6 += i;
	}
      if ((flags & FLAG_MS_GOODS) != 0)
	{
	  i = sell_goods (x - 1, y);
	  et += i;
	  MP_INFO(x + 2,y).int_6 += i;
	}
      if ((flags & FLAG_MB_STEEL) != 0)
	{
	  i = buy_steel (x - 1, y);
	  ic += i;
	  MP_INFO(x + 1,y).int_7 += i;
	}
      if ((flags & FLAG_MS_STEEL) != 0)
	{
	  i = sell_steel (x - 1, y);
	  et += i;
	  MP_INFO(x + 2,y).int_7 += i;
	}
    }
  /* upper gate next */
  if (y > 0 && (MP_INFO(x,y - 1).flags
		& FLAG_IS_TRANSPORT) != 0)
    {
      if ((flags & FLAG_MB_FOOD) != 0)
	{
	  i = buy_food (x, y - 1);
	  ic += i;
	  MP_INFO(x + 1,y).int_3 += i;
	}
      if ((flags & FLAG_MS_FOOD) != 0)
	{
	  i = sell_food (x, y - 1);
	  et += i;
	  MP_INFO(x + 2,y).int_3 += i;
	}
      if ((flags & FLAG_MB_COAL) != 0)
	{
	  i = buy_coal (x, y - 1);
	  ic += i;
	  MP_INFO(x + 1,y).int_4 += i;
	}
      if ((flags & FLAG_MS_COAL) != 0)
	{
	  i = sell_coal (x, y - 1);
	  et += i;
	  MP_INFO(x + 2,y).int_4 += i;
	}
      if ((flags & FLAG_MB_ORE) != 0)
	{
	  i = buy_ore (x, y - 1);
	  ic += i;
	  MP_INFO(x + 1,y).int_5 += i;
	}
      if ((flags & FLAG_MS_ORE) != 0)
	{
	  i = sell_ore (x, y - 1);
	  et += i;
	  MP_INFO(x + 2,y).int_5 += i;
	}
      if ((flags & FLAG_MB_GOODS) != 0)
	{
	  i = buy_goods (x, y - 1);
	  ic += i;
	  MP_INFO(x + 1,y).int_6 += i;
	}
      if ((flags & FLAG_MS_GOODS) != 0)
	{
	  i = sell_goods (x, y - 1);
	  et += i;
	  MP_INFO(x + 2,y).int_6 += i;
	}
      if ((flags & FLAG_MB_STEEL) != 0)
	{
	  i = buy_steel (x, y - 1);
	  ic += i;
	  MP_INFO(x + 1,y).int_7 += i;
	}
      if ((flags & FLAG_MS_STEEL) != 0)
	{
	  i = sell_steel (x, y - 1);
	  et += i;
	  MP_INFO(x + 2,y).int_7 += i;
	}
    }
  MP_INFO(x,y).int_1 += et;
  MP_INFO(x,y).int_4 += ic;
  if (total_time % 100 == 0)
    {
      MP_INFO(x,y).int_2 = MP_INFO(x,y).int_1;
      MP_INFO(x,y).int_1 = 0;
      MP_INFO(x,y).int_5 = MP_INFO(x,y).int_4;
      MP_INFO(x,y).int_4 = 0;
      b1 = &(MP_INFO(x + 1,y).int_3);
      s1 = &(MP_INFO(x + 2,y).int_3);
      b2 = &(MP_INFO(x,y + 1).int_3);
      s2 = &(MP_INFO(x,y + 2).int_3);
      /* GCS FIX -- This obfuscation is unnecessary. */
      for (i = 0; i < 5; i++)
	{
	  *(b2++) = *b1;
	  *(s2++) = *s1;
	  *(b1++) = 0;
	  *(s1++) = 0;
	}
    }
  if (et > 0)
    {
      sust_port_flag = 0;
      tech_level++;
    }
  if (ic > 0)
    {
      sust_port_flag = 0;
      tech_level++;
    }
  et += MP_INFO(x,y).int_3;	/* int_3 holds the 'pence' */

  export_tax += et / 100;
  MP_INFO(x,y).int_3 = et % 100;
  import_cost += ic;
}

void
mps_port (int x, int y)
{
    int i = 0;
    char buy[12], sell[12];

    mps_store_title(i++,_("Port"));
    i++;

    num_to_ansi(buy, sizeof(buy), MP_INFO(x,y+1).int_3 / 100);
    num_to_ansi(sell, sizeof(sell), MP_INFO(x,y+2).int_3 / 100);
    mps_store_sss(i++,_("Food"),buy,sell);

    num_to_ansi(buy, sizeof(buy), MP_INFO(x,y+1).int_4 / 100);
    num_to_ansi(sell, sizeof(sell), MP_INFO(x,y+2).int_4 / 100);
    mps_store_sss(i++,_("Coal"),buy,sell);

    num_to_ansi(buy, sizeof(buy), MP_INFO(x,y+1).int_5 / 100);
    num_to_ansi(sell, sizeof(sell), MP_INFO(x,y+2).int_5 / 100);
    mps_store_sss(i++,_("Ore"),buy,sell);

    num_to_ansi(buy, sizeof(buy), MP_INFO(x,y+1).int_6 / 100);
    num_to_ansi(sell, sizeof(sell), MP_INFO(x,y+2).int_6 / 100);
    mps_store_sss(i++,_("Goods"),buy,sell);

    num_to_ansi(buy, sizeof(buy), MP_INFO(x,y+1).int_7 / 100);
    num_to_ansi(sell, sizeof(sell), MP_INFO(x,y+2).int_7 / 100);
    mps_store_sss(i++,_("Steel"),buy,sell);

    num_to_ansi(buy, sizeof(buy), MP_INFO(x,y).int_2 / 100);
    num_to_ansi(sell, sizeof(sell), MP_INFO(x,y).int_5 / 100);
    mps_store_sss(i++,_("Total"),buy,sell);

}


