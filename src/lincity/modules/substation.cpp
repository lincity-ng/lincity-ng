/* ---------------------------------------------------------------------- *
 * substation.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "../power.h"
#include "substation.h"

#include <stdlib.h>

/* Substations 
 *
 * int_1 unused
 * int_2 unused except for windmills (true = hightech)
 * int_3 unused
 * int_4 is the power demand at this substation 
 * int_5 forbidden for substations: it is power production from power sources
 *                and windmill is both a source power and a substation
 * int_6 is the grid its connected to
 * int_7 is a grid timestamp
 */

void do_power_substation(int x, int y)
{
    switch (grid[MP_INFO(x, y).int_6]->powered) {
    case -1:{
            MP_TYPE(x, y) = CST_SUBSTATION_R;
        }
        break;
    case 0:{
            MP_TYPE(x, y) = CST_SUBSTATION_RG;
        }
        break;
    case 1:{
            MP_TYPE(x, y) = CST_SUBSTATION_G;
        }
        break;
    default:{
            printf("Default case in do_power_substation\n");
        }
        break;
    }
}

int add_a_substation(int x, int y)
{                               /* add to substationx substationy to list */
    if (numof_substations >= MAX_NUMOF_SUBSTATIONS)
        return (0);
    substationx[numof_substations] = x;
    substationy[numof_substations] = y;
    numof_substations++;
    return (1);
}

void remove_a_substation(int x, int y)
{
    int q;
    for (q = 0; q < numof_substations; q++)
        if (substationx[q] == x && substationy[q] == y)
            break;
    for (; q < numof_substations; q++) {
        substationx[q] = substationx[q + 1];
        substationy[q] = substationy[q + 1];
    }
    numof_substations--;
}

void shuffle_substations(void)
{
    int q, x, r, m;
    m = (numof_substations / 2) + 1;
    for (x = 0; x < m; x++) {
        r = rand() % numof_substations;
        if (r == x)
            continue;
        q = substationx[x];
        substationx[x] = substationx[r];
        substationx[r] = q;
        q = substationy[x];
        substationy[x] = substationy[r];
        substationy[r] = q;
    }
}

void mps_substation(int x, int y)
{
    int i = 0;
    char s[12];

    mps_store_title(i++, _("Substation"));
    i++;

    format_power(s, sizeof(s), MP_INFO(x, y).int_4);
    mps_store_title(i++, _("Local Demand"));
    mps_store_title(i++, s);
    i++;

    mps_store_title(i++, _("Grid Status"));

    format_power(s, sizeof(s), grid[MP_INFO(x, y).int_6]->max_power);
    mps_store_ss(i++, _("T. Cap."), s);

    format_power(s, sizeof(s), grid[MP_INFO(x, y).int_6]->avail_power);
    mps_store_ss(i++, _("A. Cap."), s);

    format_power(s, sizeof(s), grid[MP_INFO(x, y).int_6]->demand);
    mps_store_ss(i++, _("Demand"), s);
    i++;

    mps_store_sd(i++, _("Grid ID"), MP_INFO(x, y).int_6);

}
