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
//#include "lincity/modules/market.h" //to access Market counter and capacities
struct pbar_st pbars[NUM_PBARS];

void update_pbar (int pbar_num, int value, int month_flag)
{
    // copy of update_pbar from src/oldgui/pbar.cpp
    // store values for savegame
    int i;
    struct pbar_st * pbar = &pbars[pbar_num];

    if (month_flag) {
         pbar->oldtot = pbar->tot;

        /* If the dataset isn't full, just add it and forget month_flag */
        if (pbar->data_size < PBAR_DATA_SIZE)
        {
            pbar->oldtot += pbar->data[0];// new total has one additional value
            pbar->data_size++;
            month_flag = 0;
        }
    }

    pbar->tot = 0;

    for (i = 0; i < (pbar->data_size - 1); i++)
    {
        if (month_flag)
            pbar->tot += (pbar->data[i] = pbar->data[i+1]);
        else
            pbar->tot += pbar->data[i];
    }
    pbar->tot += pbar->data[i] = value;
    pbar->diff = pbar->tot - pbar->oldtot;

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
            {   LCPBarPage1->setValue(p,pbar->data[pbar->data_size - 1],pbar->diff);}
            else if (pbarGlobalStyle == 1)
            {   LCPBarPage2->setValue(p,pbar->data[pbar->data_size - 1],pbar->diff);}
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
        pbars[p].diff = 1;
        for (i = 0; i < PBAR_DATA_SIZE; i++)
            pbars[p].data[i] = 0;
    }
}


void update_pbars_monthly()
{
    update_pbar (PPOP, housed_population + people_pool, 1);
    update_pbar (PTECH, tech_level, 1);
    update_pbar (PMONEY, total_money, 1);
    if (tstat_capacities[Construction::STUFF_FOOD] > 99)
    {    update_pbar (PFOOD, tstat_census[Construction::STUFF_FOOD] / (tstat_capacities[Construction::STUFF_FOOD]/100), 1);}
    else update_pbar (PFOOD,0,1);
    if (tstat_capacities[Construction::STUFF_JOBS] > 99)
    {    update_pbar (PJOBS, tstat_census[Construction::STUFF_JOBS] / ( tstat_capacities[Construction::STUFF_JOBS]/100), 1);}
    else update_pbar (PJOBS,0,1);
    if (tstat_capacities[Construction::STUFF_GOODS] > 99)
    {    update_pbar (PGOODS, tstat_census[Construction::STUFF_GOODS] / ( tstat_capacities[Construction::STUFF_GOODS]/100), 1);}
    else update_pbar (PGOODS,0,1);
    if (tstat_capacities[Construction::STUFF_COAL] > 99)
    {    update_pbar (PCOAL, tstat_census[Construction::STUFF_COAL] / ( tstat_capacities[Construction::STUFF_COAL]/100), 1);}
    else update_pbar (PCOAL,0,1);
    if (tstat_capacities[Construction::STUFF_ORE] > 99)
    {    update_pbar (PORE, tstat_census[Construction::STUFF_ORE] / ( tstat_capacities[Construction::STUFF_ORE]/100), 1);}
    else update_pbar (PORE,0,1);
    if (tstat_capacities[Construction::STUFF_STEEL] > 99)
    {    update_pbar (PSTEEL, tstat_census[Construction::STUFF_STEEL] / ( tstat_capacities[Construction::STUFF_STEEL]/100), 1);}
    else update_pbar (PSTEEL,0,1);

    update_pbar (PPOL, total_pollution, 1);
    if (tstat_capacities[Construction::STUFF_KWH] > 99)
    {    update_pbar (PKWH, tstat_census[Construction::STUFF_KWH] / ( tstat_capacities[Construction::STUFF_KWH]/100), 1);}
    else update_pbar (PKWH,0,1);
    if (tstat_capacities[Construction::STUFF_MWH] > 99)
    {    update_pbar (PMWH, tstat_census[Construction::STUFF_MWH] / ( tstat_capacities[Construction::STUFF_MWH]/100), 1);}
    else update_pbar (PMWH,0,1);
    if (tstat_capacities[Construction::STUFF_WATER] > 99)
    {    update_pbar (PWATER, tstat_census[Construction::STUFF_WATER] / ( tstat_capacities[Construction::STUFF_WATER]/100), 1);}
    else update_pbar (PWATER,0,1);
    if (tstat_capacities[Construction::STUFF_WASTE] > 99)
    {    update_pbar (PWASTE, tstat_census[Construction::STUFF_WASTE] / (tstat_capacities[Construction::STUFF_WASTE]/100), 1);}
    else update_pbar (PWASTE,0,1);
    if (total_housing)
    {    update_pbar (PHOUSE, (100 * housed_population)/total_housing , 1);}
    else update_pbar (PHOUSE,0,1);

}

/** @file lincity-ng/PbarInterface.cpp */

