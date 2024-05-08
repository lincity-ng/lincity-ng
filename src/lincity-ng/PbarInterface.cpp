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

#include <map>                             // for map

#include "PBar.hpp"                        // for LCPBar, LCPBarPage1, LCPBa...
#include "gui_interface/pbar_interface.h"  // for pbar_st, PBAR_DATA_SIZE
#include "lincity/commodities.hpp"         // for Commodity
#include "lincity/engglobs.h"              // for housed_population, total_h...
#include "lincity/lintypes.h"              // for tstat_capacities, tstat_ce...

struct pbar_st pbars[NUM_PBARS];

void update_pbar (int pbar_num, int value, int month_flag)
{
    // copy of update_pbar from src/oldgui/pbar.cpp
    // store values for savegame
    struct pbar_st * pbar = &pbars[pbar_num];
    int old = pbar->data[0];

    if(month_flag) {
        for(int i = 0; i < PBAR_DATA_SIZE - 1; i++) {
            pbar->data[i] = pbar->data[i+1];
        }
        pbar->data[PBAR_DATA_SIZE - 1] = value;
    }
    pbar->diff = value - old;

    // new: update bars
    if(LCPBarPage1 && LCPBarPage2)
    {
        LCPBarPage1->setValue(pbar_num,value,pbar->diff);
        LCPBarPage2->setValue(pbar_num,value,pbar->diff);
    }
}

void refresh_pbars (void)
{
    if(LCPBarPage1 && LCPBarPage2)
        for (int p = 0; p<NUM_PBARS; p++)
        {
            struct pbar_st * pbar = &pbars[p];
            if (pbarGlobalStyle == 0)
            {   LCPBarPage1->setValue(p,pbar->data[PBAR_DATA_SIZE - 1],pbar->diff);}
            else if (pbarGlobalStyle == 1)
            {   LCPBarPage2->setValue(p,pbar->data[PBAR_DATA_SIZE - 1],pbar->diff);}
        }
}

void init_pbars (void)
{
    int i, p;
    for (p = 0; p < NUM_PBARS; p++)
    {
        pbars[p].data_size = 0;
        pbars[p].oldtot = 0;
        pbars[p].tot = 0;
        pbars[p].diff = 0;
        for (i = 0; i < PBAR_DATA_SIZE; i++)
        {   pbars[p].data[i] = 0;}
    }
}


void update_pbars_monthly()
{
    update_pbar (PPOP, housed_population + people_pool, 1);
    update_pbar (PTECH, tech_level, 1);
    update_pbar (PMONEY, total_money, 1);
    update_pbar (PFOOD, tstat_census[STUFF_FOOD] * 1000L / tstat_capacities[STUFF_FOOD], 1);
    update_pbar (PJOBS, tstat_census[STUFF_JOBS] * 1000L / tstat_capacities[STUFF_JOBS], 1);
    update_pbar (PGOODS, tstat_census[STUFF_GOODS] * 1000L / tstat_capacities[STUFF_GOODS], 1);
    update_pbar (PCOAL, tstat_census[STUFF_COAL] * 1000L / tstat_capacities[STUFF_COAL], 1);
    update_pbar (PORE, tstat_census[STUFF_ORE] * 1000L / tstat_capacities[STUFF_ORE], 1);
    update_pbar (PSTEEL, tstat_census[STUFF_STEEL] * 1000L / tstat_capacities[STUFF_STEEL], 1);

    update_pbar (PPOL, total_pollution, 1);
    update_pbar (PKWH, tstat_census[STUFF_KWH] * 1000L / tstat_capacities[STUFF_KWH], 1);
    update_pbar (PMWH, tstat_census[STUFF_MWH] * 1000L / tstat_capacities[STUFF_MWH], 1);
    update_pbar (PWATER, tstat_census[STUFF_WATER] * 1000L / tstat_capacities[STUFF_WATER], 1);
    update_pbar (PWASTE, tstat_census[STUFF_WASTE] * 1000L / tstat_capacities[STUFF_WASTE], 1);
    if (total_housing)
    {    update_pbar (PHOUSE, (1000 * housed_population)/total_housing , 1);}
    else update_pbar (PHOUSE,0,1);

}

/** @file lincity-ng/PbarInterface.cpp */
