/* ---------------------------------------------------------------------- *
 * rail.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include <lin-city.h>
#include <lctypes.h>
#include <engglobs.h>
#include <cliglobs.h>
#include <stats.h>
#include <transport.h>
#include <rail.h>

/* ---------------------------------------------------------------------
   For track, road and rail:
  
   int_1 contains the amount of food
   int_2 contains the amount of jobs
   int_3 contains the amount of coal
   int_4 contains the amount of goods
   int_5 contains the amount of ore
   int_6 contains the amount of steel
   int_7 contains the amount of waste
  --------------------------------------------------------------------- */

void
do_rail (int x, int y)
{
    static int wb_count = 0;
    int *pol = &MP_POL(x,y);
    Map_Point_Info *minfo = &MP_INFO(x,y);
    transport_cost += 3;
    if (total_time % DAYS_PER_RAIL_POLLUTION == 0)
	*pol += RAIL_POLLUTION;
    if ((total_time & RAIL_GOODS_USED_MASK) == 0 && minfo->int_4 > 0) {
	--minfo->int_4;
	++minfo->int_7;
    }
    if ((total_time & RAIL_STEEL_USED_MASK) == 0 && minfo->int_6 > 0) {
	--minfo->int_6;
	++minfo->int_7;
    }
    general_transport (minfo, pol, MAX_WASTE_ON_RAIL, &wb_count);
}
