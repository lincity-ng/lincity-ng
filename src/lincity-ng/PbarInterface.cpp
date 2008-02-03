/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <config.h>

//#include "ag_debug.h"
#include "lincity/engglobs.h"
#include "gui_interface/pbar_interface.h"
#include "PBar.hpp"
#include "lincity/stats.h"

struct pbar_st pbars[NUM_PBARS];

void pbars_full_refresh (void)
{
  //  TRACE;
}

void update_pbar (int pbar_num, int value, int month_flag)
{
    // copy of update_pbar from src/oldgui/pbar.cpp 
    // store values for savegame 
    int i;
    struct pbar_st * pbar = &pbars[pbar_num];

    if (month_flag) {
         pbar->oldtot = pbar->tot;
    
        /* If the dataset isn't full, just add it and forget month_flag */
        if (pbar->data_size < PBAR_DATA_SIZE) {
            pbar->oldtot += pbar->data[0];// new total has one additional value
            pbar->data_size++;
            month_flag = 0;
        }
    }

    pbar->tot = 0;

    for (i = 0; i < (pbar->data_size - 1); i++) {
        if (month_flag) 
            pbar->tot += (pbar->data[i] = pbar->data[i+1]);
        else
            pbar->tot += pbar->data[i];
    }
    pbar->tot += pbar->data[i] = value;
    pbar->diff = pbar->tot - pbar->oldtot;

    // new: update bars
    // NG 1.91 AL1: FIXME is this test needed ? 
    if(LCPBarInstance)
        LCPBarInstance->setValue(pbar_num,value,pbar->diff);
}

void refresh_pbars (void)
{
  //  TRACE;
}

void init_pbars (void)
{
    int i, p;
    for (p = 0; p < NUM_PBARS; p++) {
        pbars[p].data_size = 0;
        pbars[p].oldtot = 0;
        pbars[p].tot = 0;
        pbars[p].diff = 0;
        for (i = 0; i < PBAR_DATA_SIZE; i++)
            pbars[p].data[i] = 0;
    }
}


void update_pbars_monthly()
{
    update_pbar (PPOP, housed_population + people_pool, 1);
    update_pbar (PTECH, tech_level, 1);
    update_pbar (PFOOD, tfood_in_markets / data_last_month, 1);
    update_pbar (PJOBS, tjobs_in_markets / data_last_month, 1);
    update_pbar (PCOAL, tcoal_in_markets / data_last_month, 1);
    update_pbar (PGOODS, tgoods_in_markets / data_last_month, 1);
    update_pbar (PORE, tore_in_markets / data_last_month, 1);
    update_pbar (PSTEEL, tsteel_in_markets / data_last_month, 1);
    update_pbar (PMONEY, total_money, 1);
}
