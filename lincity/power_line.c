/* ---------------------------------------------------------------------- *
 * power_line.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "power.h"
#include "power_line.h"


/*** Power Lines ***/
/*
  int_5 is animation schedule
  int_6 is the grid it is on
  int_7 is a grid timestamp
*/
void
do_power_line (int x, int y)
{
    if (grid[MP_INFO(x,y).int_6]->powered == -1)
	return;

    switch(MP_INFO(x,y).int_5) 
    {
    case 0: 
	MP_INFO(x,y).int_5 = POWER_MODULUS;
	break;
    case 1:
	if (!(MP_TYPE(x,y) <= 11 && MP_TYPE(x,y) >= 1))
	    break;
	MP_TYPE(x,y) += 11;
	break;
    case 2:
	if (!(MP_TYPE(x,y) >= 11 && MP_TYPE(x,y) <= 22))
	    break;
	MP_TYPE(x,y) -= 11;
	break;
    }

    MP_INFO(x,y).int_5--;
}

void
mps_power_line (int x, int y)
{
  int i = 0;

  mps_store_title(i++,_("Power Line"));
  i++;

  mps_store_sd(i++,_("Grid ID"),MP_INFO(x,y).int_6);
}
