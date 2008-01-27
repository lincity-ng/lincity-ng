/* ---------------------------------------------------------------------- *
 * coal_power.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "../power.h"
#include "coal_power.h"
#include "../transport.h" 

/*** Coal Power ***/
/*
  // int_1 is the maximum possible power (depends on MP_TECH)
  // int_2 is the coal at the power station
  // int_3 is the stored jobs... Must be an interesting warehouse
  // int_4 unused  
  // int_5 is the projected output.
  // int_6 is the grid ID
  // int_7 is grid_timestamp
  //
  // MP_TECH is the tech level when built
 
*/

void do_power_source_coal(int x, int y)
{

    /* Need coal?  Try transport. */
    if (MP_INFO(x, y).int_2 < MAX_COAL_AT_POWER_STATION) {

        /* left side */
        if (XY_IS_TRANSPORT(x - 1, y + 1) && MP_INFO(x - 1, y + 1).int_3 > 0) {
            if (get_jobs(x, y, JOBS_LOAD_COAL) != 0) {
                MP_INFO(x, y).int_2 += (MP_INFO(x - 1, y + 1).int_3 / 2 + ((MP_INFO(x - 1, y + 1).int_3) % 2));
                MP_INFO(x - 1, y + 1).int_3 /= 2;
                MP_POL(x, y)++;
            }
        }
        /* top side */
        else if (XY_IS_TRANSPORT(x + 1, y - 1) && MP_INFO(x + 1, y - 1).int_3 > 0) {
            if (get_jobs(x, y, JOBS_LOAD_COAL) != 0)
                MP_INFO(x, y).int_2 += (MP_INFO(x + 1, y - 1).int_3 / 2 + ((MP_INFO(x + 1, y - 1).int_3) % 2));
            MP_INFO(x + 1, y - 1).int_3 /= 2;
            MP_POL(x, y)++;
        }
    }

    /* Need jobs?  get_jobs. */
    if ((MP_INFO(x, y).int_3 + JOBS_COALPS_GENERATE + 10)
        < MAX_JOBS_AT_COALPS)
        if (get_jobs(x, y, JOBS_COALPS_GENERATE + 10) != 0)
            MP_INFO(x, y).int_3 += JOBS_COALPS_GENERATE + 10;

    /* Generate Power */
    if (MP_INFO(x, y).int_2 > POWERS_COAL_OUTPUT / 500 && MP_INFO(x, y).int_3 > JOBS_COALPS_GENERATE) {
        MP_INFO(x, y).int_5 = MP_INFO(x, y).int_1;
        MP_INFO(x, y).int_3 -= JOBS_COALPS_GENERATE;
        MP_INFO(x, y).int_2 -= POWERS_COAL_OUTPUT / 500;
        coal_used += POWERS_COAL_OUTPUT / 500;
        MP_POL(x, y) += POWERS_COAL_POLLUTION;
        grid[MP_INFO(x, y).int_6]->avail_power += MP_INFO(x, y).int_1;
    }

    /* Animation */
    /* choose a graphic */
    if (MP_INFO(x, y).int_2 > (MAX_COAL_AT_POWER_STATION - (MAX_COAL_AT_POWER_STATION / 5)))
        MP_TYPE(x, y) = CST_POWERS_COAL_FULL;
    else if (MP_INFO(x, y).int_2 > (MAX_COAL_AT_POWER_STATION / 2))
        MP_TYPE(x, y) = CST_POWERS_COAL_MED;
    else if (MP_INFO(x, y).int_2 > (MAX_COAL_AT_POWER_STATION / 10))
        MP_TYPE(x, y) = CST_POWERS_COAL_LOW;
    else
        MP_TYPE(x, y) = CST_POWERS_COAL_EMPTY;
}

void mps_coal_power(int x, int y)
{
    int i = 0;

    char s[12];

    mps_store_title(i++, _("Coal"));
    mps_store_title(i++, _("Power Station"));
    i++;

    format_power(s, sizeof(s), MP_INFO(x, y).int_1);
    mps_store_title(i++, _("Max Output"));
    mps_store_title(i++, s);
    i++;

    format_power(s, sizeof(s), MP_INFO(x, y).int_5);
    mps_store_title(i++, _("Current Output"));
    mps_store_title(i++, s);
    i++;

    mps_store_sfp(i++, _("Coal"), MP_INFO(x, y).int_2 * 100.0 / MAX_COAL_AT_POWER_STATION);
    mps_store_sfp(i++, _("Jobs"), MP_INFO(x, y).int_3 * 100.0 / MAX_JOBS_AT_COALPS);
    mps_store_sfp(i++, _("Tech"), MP_TECH(x, y) * 100.0 / MAX_TECH_LEVEL);
    mps_store_sd(i++, _("Grid ID"), MP_INFO(x, y).int_6);
}
