/* ---------------------------------------------------------------------- *
 * waterwell.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "waterwell.h"


void
do_waterwell_cover (int x, int y)
{
  int xx, x1, x2, y1, y2;
#ifdef DEBUG_WATERWELL
  if (use_waterwell)
  	fprintf(stderr," doing waterwell cover\n");
  else 
  	return;
#endif

  x1 = x - WATERWELL_RANGE;
  if (x1 < 0)
    x1 = 0;
  x2 = x + WATERWELL_RANGE;
  if (x2 > WORLD_SIDE_LEN)
    x2 = WORLD_SIDE_LEN;
  y1 = y - WATERWELL_RANGE;
  if (y1 < 0)
    y1 = 0;
  y2 = y + WATERWELL_RANGE;
  if (y2 > WORLD_SIDE_LEN)
    y2 = WORLD_SIDE_LEN;
  for (; y1 < y2; y1++)
    for (xx = x1; xx < x2; xx++)
      MP_INFO(xx,y1).flags |= FLAG_WATERWELL_COVER;
}


void
mps_waterwell (int x, int y)
{
    int i = 0;

    const char * p;

    mps_store_title(i++,_("Water_well"));
    i++; /* blank line */

    mps_store_sd(i++, _("Pollution"), MP_POL(x,y));

    /* p = (MP_INFO(x,y).flags & FLAG_IS_RIVER) ? _("Yes") : _("No"); */
    p = _("Yes");
    mps_store_ss(i++,_("Drinkable"),p);

}

