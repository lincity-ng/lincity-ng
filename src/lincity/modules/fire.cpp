/* ---------------------------------------------------------------------- *
 * fire.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "fire.h"
#include <stdlib.h>

void
do_fire (int x, int y)
{
  /*
    // int_1 is the next animation frame time
    // int_2 is the fire length
    // int_3 is the real_time before the fire can spread or -1 if triggered 
    // int_4 is the idle land length
  */
  int i;
  /* this so we don't get whole blocks changing in one go. */
  if (MP_INFO(x,y).int_2 == 0)

    MP_INFO(x,y).int_2 = rand () % (FIRE_LENGTH / 5);
  if (MP_INFO(x,y).int_2 > FIRE_LENGTH)
    {
      if (MP_INFO(x,y).int_4 == 0)	/* rand length here also */
	MP_INFO(x,y).int_4 = rand () % (AFTER_FIRE_LENGTH / 6);
      MP_INFO(x,y).int_4++;
      if (MP_INFO(x,y).int_4 > AFTER_FIRE_LENGTH)
          if (use_waterwell) {
              do_bulldoze_area (CST_DESERT, x, y);
          } else {
              do_bulldoze_area (CST_GREEN, x, y);
          }
      else if (MP_INFO(x,y).int_4 > (3 * AFTER_FIRE_LENGTH) / 4)
	MP_TYPE(x,y) = CST_FIRE_DONE4;
      else if (MP_INFO(x,y).int_4 > (2 * AFTER_FIRE_LENGTH) / 4)
	MP_TYPE(x,y) = CST_FIRE_DONE3;
      else if (MP_INFO(x,y).int_4 > (AFTER_FIRE_LENGTH) / 4)
	MP_TYPE(x,y) = CST_FIRE_DONE2;
      else
	MP_TYPE(x,y) = CST_FIRE_DONE1;
      return;
    }
  MP_INFO(x,y).int_2++;
  if (real_time > MP_INFO(x,y).int_1)
    {
      MP_INFO(x,y).int_1 = real_time + FIRE_ANIMATION_SPEED;
      if (MP_TYPE(x,y) == CST_FIRE_1)
	MP_TYPE(x,y) = CST_FIRE_2;
      else if (MP_TYPE(x,y) == CST_FIRE_2)
	MP_TYPE(x,y) = CST_FIRE_3;
      else if (MP_TYPE(x,y) == CST_FIRE_3)
	MP_TYPE(x,y) = CST_FIRE_4;
      else if (MP_TYPE(x,y) == CST_FIRE_4)
	MP_TYPE(x,y) = CST_FIRE_5;
      else if (MP_TYPE(x,y) == CST_FIRE_5)
	MP_TYPE(x,y) = CST_FIRE_1;
    }
  if (MP_INFO(x,y).int_3 == -1)
    {
      if ((rand () % FIRE_DAYS_PER_SPREAD) == 1)
	{
	  i = rand () % 4;
	  switch (i)
	    {
	    case (0):
	      do_random_fire (x - 1, y, 0);
	      break;
	    case (1):
	      do_random_fire (x, y - 1, 0);
	      break;
	    case (2):
	      do_random_fire (x + 1, y, 0);
	      break;
	    case (3):
	      do_random_fire (x, y + 1, 0);
	      break;
	    }
	}
    }
  /* check here 'cos we can wait in the ok box for ever. */
  else if (MP_INFO(x,y).int_3 == 0)
    MP_INFO(x,y).int_3 = real_time + 15000;	/* 15 secs seem fair */
  else if (real_time >= MP_INFO(x,y).int_3)
    MP_INFO(x,y).int_3 = -1;
}
