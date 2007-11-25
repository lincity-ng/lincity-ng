/* ---------------------------------------------------------------------- *
 * parkland.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "parkland.h"

void do_parkland(int x, int y)
{
    if (MP_POL(x, y) > 10 && (total_time & 1) == 0)
        MP_POL(x, y) -= 1;
}
