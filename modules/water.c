/* ---------------------------------------------------------------------- *
 * water.c
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
#include <water.h>


void
mps_water (int x, int y)
{
    int i = 0;

    char * p;

    mps_store_title(i++,_("Water"));
    i++;

    p = (MP_INFO(x,y).flags & FLAG_IS_RIVER) ? _("Yes") : _("No");
    mps_store_ss(i++,_("Navigable"),p);
      
}
