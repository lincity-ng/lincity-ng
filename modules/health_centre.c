/* ---------------------------------------------------------------------- *
 * health_centre.c
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
#include <health_centre.h>


void
do_health_centre (int x, int y)
{
  /*
     // int_1 is the jobs stored at the health centre
     // int_2 is the goods stored at the health centre
   */
  if (MP_INFO(x,y).int_1
      < (MAX_JOBS_AT_HEALTH_CENTRE - HEALTH_CENTRE_GET_JOBS))
    if (get_jobs (x, y, HEALTH_CENTRE_GET_JOBS) != 0)
      MP_INFO(x,y).int_1 += HEALTH_CENTRE_GET_JOBS;
  if (MP_INFO(x,y).int_2
      < (MAX_GOODS_AT_HEALTH_CENTRE - HEALTH_CENTRE_GET_GOODS))
    if (get_goods (x, y, HEALTH_CENTRE_GET_GOODS) != 0)
      MP_INFO(x,y).int_2 += HEALTH_CENTRE_GET_GOODS;

  /* That's all. Cover is done by different functions every 3 months or so. */

  health_cost += HEALTH_RUNNING_COST;
}


void
do_health_cover (int x, int y)
{
  int xx, x1, x2, y1, y2;
  if (MP_INFO(x,y).int_1 < (HEALTH_CENTRE_JOBS * DAYS_BETWEEN_COVER) ||
      MP_INFO(x,y).int_2 < (HEALTH_CENTRE_GOODS * DAYS_BETWEEN_COVER))
    return;
  MP_INFO(x,y).int_1 -= (HEALTH_CENTRE_JOBS * DAYS_BETWEEN_COVER);
  MP_INFO(x,y).int_2 -= (HEALTH_CENTRE_GOODS * DAYS_BETWEEN_COVER);
  x1 = x - HEALTH_CENTRE_RANGE;
  if (x1 < 0)
    x1 = 0;
  x2 = x + HEALTH_CENTRE_RANGE;
  if (x2 > WORLD_SIDE_LEN)
    x2 = WORLD_SIDE_LEN;
  y1 = y - HEALTH_CENTRE_RANGE;
  if (y1 < 0)
    y1 = 0;
  y2 = y + HEALTH_CENTRE_RANGE;
  if (y2 > WORLD_SIDE_LEN)
    y2 = WORLD_SIDE_LEN;
  for (; y1 < y2; y1++)
    for (xx = x1; xx < x2; xx++)
      MP_INFO(xx,y1).flags |= FLAG_HEALTH_COVER;
}


void
mps_health_centre (int x, int y)
{
    int i = 0;

    mps_store_title(i++,_("Health Centre"));
    i++;
    mps_store_title(i++,_("Inventory"));
    mps_store_sfp(i++,_("Jobs"),
		  MP_INFO(x,y).int_1 * 100.0 / MAX_JOBS_AT_HEALTH_CENTRE);
    mps_store_sfp(i++,_("Goods"),
		  MP_INFO(x,y).int_2 * 100.0 / MAX_GOODS_AT_HEALTH_CENTRE);

}
