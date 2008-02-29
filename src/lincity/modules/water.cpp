/* ---------------------------------------------------------------------- *
 * water.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "water.h"

void mps_water(int x, int y)
{
    int i;
    const char *p;

    for(i = 0; i < MPS_PARAGRAPH_COUNT; ++i)
                        mps_store_title( i, "" );

    i = 0;
    mps_store_title(i++, _("Water"));
    i++;

    p = (MP_INFO(x, y).flags & FLAG_IS_RIVER) ? _("Yes") : _("No");
    mps_store_ss(i++, _("Navigable"), p);

#ifdef DEBUG
    mps_store_sd(10, "x = ", x);
    mps_store_sd(11, "y = ", y);
    mps_store_sd(12, "altitude = ", ALT(x, y));

    fprintf(stderr, "water x %i, y %i, Alt %i\n", x, y, ALT(x,y));
#endif

}
