/* ---------------------------------------------------------------------- *
 * shanty.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
//#include "mouse.h" /* XXX: fire_area! */
#include "shanty.h"

#include <stdlib.h>

void
add_a_shanty (void)
{
  int r, x, y;
  x = rand () % WORLD_SIDE_LEN;
  y = rand () % WORLD_SIDE_LEN;
  if (numof_shanties > 0 && rand () % 8 != 0)
    {
      r = spiral_find_group (x, y, GROUP_SHANTY);
      if (r == -1)
	{
	  printf ("Looked for a shanty, without any! x=%d y=%d\n", x, y);
	  return;
	}
      y = r / WORLD_SIDE_LEN;
      x = r % WORLD_SIDE_LEN;
      r = spiral_find_2x2 (x, y);
      if (r == -1)
	{
#ifdef commentout /* wck: These are annoying when the map is full */
	 	  printf ("Adding a shanty (s), no space for it?!\n");
#endif
	  return;
	}
      y = r / WORLD_SIDE_LEN;
      x = r % WORLD_SIDE_LEN;
    }
  else
    {
      r = spiral_find_group (x, y, GROUP_MARKET);
      if (r == -1)
	return;			/* silently return, we havn't started yet. */

      y = r / WORLD_SIDE_LEN;
      x = r % WORLD_SIDE_LEN;
      r = spiral_find_2x2 (x, y);
      if (r == -1)
	{
#ifdef commentout /* see above */
	  printf ("Adding a shanty (r), no space for it?!\n");
#endif
	  return;
	}
      y = r / WORLD_SIDE_LEN;
      x = r % WORLD_SIDE_LEN;
    }
  set_mappoint (x, y, CST_SHANTY);
  numof_shanties++;
}

void
remove_a_shanty (void)
{
  int x, y, r;
  if (numof_shanties <= 0)
    return;
  x = rand () % WORLD_SIDE_LEN;
  y = rand () % WORLD_SIDE_LEN;
  r = spiral_find_group (x, y, GROUP_SHANTY);
  if (r == -1) {
      printf ("Can't find a shanty to remove! ?\n");
      return;
  }
  y = r / WORLD_SIDE_LEN;
  x = r % WORLD_SIDE_LEN;
  /* decrement of numof_shanties is done in fire area() */
  fire_area (x, y);
  /* now put the fire out */
  MP_INFO(x,y).int_2 = FIRE_LENGTH + 1;
  MP_INFO(x + 1,y).int_2 = FIRE_LENGTH + 1;
  MP_INFO(x,y + 1).int_2 = FIRE_LENGTH + 1;
  MP_INFO(x + 1,y + 1).int_2 = FIRE_LENGTH + 1;
}

void
update_shanty (void)
{
  int i, pp;
  pp = people_pool - (COMMUNE_POP * numof_communes);
  i = (pp - SHANTY_MIN_PP) / SHANTY_POP;
  if (i > numof_shanties)
    add_a_shanty ();		/*                   vv-- schmitt trigger */

  else if (numof_shanties > 0 && i < (numof_shanties - 1))
    remove_a_shanty ();
  else if (numof_shanties > 0 && rand () % 100 == 10)
    remove_a_shanty ();		/* randomly close some down. */

}

void
do_shanty (int x, int y)
{				/* just steal some stuff and make pollution. */

  get_food (x, y, SHANTY_GET_FOOD);
  if (get_goods (x, y, SHANTY_GET_GOODS) != 0)
    if ((goods_tax -= SHANTY_GET_GOODS * 2) < 0)
      goods_tax = 0;
  get_ore (x, y, SHANTY_GET_ORE);
  get_steel (x, y, SHANTY_GET_STEEL);
  if (get_jobs (x, y, SHANTY_GET_JOBS) != 0)
    if ((income_tax -= SHANTY_GET_JOBS * 2) < 0)
      income_tax = 0;
  if (get_coal (x, y, SHANTY_GET_COAL) != 0)
    if ((coal_tax -= SHANTY_GET_COAL * 2) < 0)
      coal_tax = 0;
  if ((total_time & 1) == 0)
    MP_POL(x,y)++;
  else
    MP_POL(x+1,y+1)++;
}
