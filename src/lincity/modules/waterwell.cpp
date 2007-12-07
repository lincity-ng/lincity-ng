/* ---------------------------------------------------------------------- *
 * waterwell.c
 * Copyright (C) 2007 Alain Baeckeroot (alain.baeckeroot@laposte.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "modules.h"
#include "waterwell.h"

void do_waterwell_cover(int x, int y)
{
    int xx, x1, x2, y1, y2;
#ifdef DEBUG_WATERWELL
    if (use_waterwell)
        fprintf(stderr, " doing waterwell cover\n");
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
            MP_INFO(xx, y1).flags |= FLAG_WATERWELL_COVER;
}

void mps_waterwell(int x, int y)
{
    int i = 0;

    const char *p;

    mps_store_title(i++, _("Water_well"));
    i++;                        /* blank line */

    mps_store_sd(i++, _("Pollution"), MP_POL(x, y));

    /* p = (MP_INFO(x,y).flags & FLAG_IS_RIVER) ? _("Yes") : _("No"); */
    p = _("Yes");
    mps_store_ss(i++, _("Drinkable"), p);

}
