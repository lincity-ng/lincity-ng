/* ---------------------------------------------------------------------- *
 * water.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "water.h"
#include "all_buildings.h"

void mps_water(int x, int y)
{
    int i;
    const char *p;

    i = 0;
    mps_store_sdd(i++, waterConstructionGroup.name, x, y);
    i++;

    p = (world(x,y)->flags & FLAG_IS_RIVER) ? _("River") : _("Lake");
    mps_store_title(i++, p);
/*
#ifdef DEBUG
    mps_store_sd(10, "x", x);
    mps_store_sd(11, "y", y);
    mps_store_sd(12, "altitude", world(x,y)->ground.altitude);

    fprintf(stderr, "water x %i, y %i, Alt %i\n", x, y, world(x,y)->ground.altitude);
#endif
*/
}

/** @file lincity/modules/water.cpp */

