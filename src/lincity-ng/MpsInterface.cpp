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

#include "gui_interface/mps.h"

#include "Mps.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>

#include "lincity/stats.h"
#include "lincity/lclib.h"
#include "lincity/engglobs.h"
#include "tinygettext/gettext.hpp"
#include "lincity/modules/modules_interfaces.h"
#include "lincity/lctypes.h"
#include "lincity/lin-city.h"
#include "lincity/modules/shanty.h" //for counting Shanties in housing display
#include "MiniMap.hpp"

// implement everything here

// this is the MPS-status display, which describes the cells

int mps_x = 0;
int mps_y = 0;
int mps_style = 0;
int mps_map_page = 0;

Mps* currentMPS = 0;

void mps_init()
{
}

int mps_set_silent(int x, int y, int style)
{
    static int same_square = 0;
    mps_style = style;
    switch(style)
    {
        case MPS_MAP:
        case MPS_ENV:
            if (mps_x == x && mps_y == y)
            {   ++same_square;}
            else
            {
                same_square = 0;
                mps_x = x;
                mps_y = y;
            }
            break;
        default:
            break;
    }
    return same_square;
}

int mps_set( int x, int y, int style ) /* Attaches an area or global display */
{
    int same_square = mps_set_silent(x, y, style);
    if(same_square)
    {   mps_map_page = (mps_map_page + 1)%MPS_MAP_PAGES;}

    switch(style) {
        case MPS_MAP:
            if(mapMPS)
            {   mapMPS->clear();}
            getMiniMap()->switchView("MapMPS");
            break;
        case MPS_ENV:
            if(envMPS)
            {   envMPS->clear();}
            getMiniMap()->switchView("EnvMPS");
            break;
        case MPS_GLOBAL:
            if(globalMPS)
            {   globalMPS->clear();}
            getMiniMap()->switchView("GlobalMPS");
            break;
        default:
            assert(false);
            break;
    }
    mps_refresh();
    return same_square;
}

void mps_refresh() /* refresh the information display's contents */
{

    switch (mps_style)
    {
        case MPS_MAP:
            currentMPS = mapMPS;
            if (world(mps_x, mps_y)->reportingConstruction)
            {
                world(mps_x, mps_y)->reportingConstruction->report();
            }
            else
               {
                switch(world(mps_x, mps_y)->getGroup())
                {
                    case GROUP_WATER:
                        mps_water (mps_x, mps_y);
                        break;
                    default:
                        //no special information on this group, just show the ground info.

                    mps_store_sdd(0,world(mps_x, mps_y)->getTileConstructionGroup()->name, mps_x, mps_y);
                    //mps_store_sdd(0,main_groups[world(mps_x, mps_y)->getGroup()].name, mps_x, mps_y);

                        mps_store_title(2, "no further information available" );

                    if( world(mps_x, mps_y)->is_bare() )
                    {
                        mps_store_title(8,"build something here" );
                    }
 /*#ifdef DEBUG
                    mps_store_sd(10, "x", mps_x);
                    mps_store_sd(11, "y", mps_y);
                    mps_store_sd(12, "altitude", world(mps_x, mps_y)->ground.altitude);

                    fprintf(stderr, "x %i, y %i, Alt %i\n", mps_x, mps_y, world(mps_x, mps_y)->ground.altitude);
 #endif
 */
             } //endswitch groups
            } //elseif use of modern report
            currentMPS = 0;
            break;

        case MPS_ENV:
            currentMPS = envMPS;
            mps_right (mps_x, mps_y);
            currentMPS = 0;
            break;

        case MPS_GLOBAL:
            currentMPS = globalMPS;
            switch (mps_global_style) {
                case MPS_GLOBAL_FINANCE:
                    mps_global_finance();
                    break;
                case MPS_GLOBAL_OTHER_COSTS:
                    mps_global_other_costs();
                    break;
                case MPS_GLOBAL_HOUSING:
                    mps_global_housing();
                    break;
                default:
                    printf("MPS unimplemented for global display\n");
                    break;
            }
            currentMPS = 0;
            break;

        default:
            break;
    }
}

/** Update text contents for later display (refresh) */
void mps_update()
{
    mps_update( mps_x, mps_y , mps_style );
}

/**
 * mps_info storage functions; place values of corresponding type into
 * mps_info[], performing certain pretification. The single argument
 * forms center their argument.  The dual arguments left-justify the
 * first and right-justify the second.  the ..p forms put a % after
 * the second argument
 */
void mps_store_title(int i, const char * t)
{
    if(!currentMPS)
        return;

    currentMPS->setText(i,t);
}

void mps_store_fp(int i, double f)
{
    if(!currentMPS)
        return;

    std::ostringstream os;
    os << std::setprecision(1) << std::fixed << f << "%";
    currentMPS->setText(i,os.str());
}

void mps_store_f(int i, double f)
{
    if(!currentMPS)
        return;

    std::ostringstream os;
    os<<std::setprecision(1)<<std::fixed;
    os<<f;
    currentMPS->setText(i,os.str());
}

void mps_store_sf(int i, const char * s, double fl)
{
    if(!currentMPS)
        return;

    std::ostringstream os;
    os<<std::setprecision(1)<<std::fixed;
    os<<s<<"\t"<<fl;
    currentMPS->setText(i,os.str());
}


void mps_store_d(int i, int d)
{
    if(!currentMPS)
        return;

    std::ostringstream os;
    os<<d;
    currentMPS->setText(i,os.str());
}

void mps_store_ss(int i, const char * s1, const char * s2)
{
    if(!currentMPS)
        return;

    std::ostringstream os;
    os<<s1<<"\t"<<s2;
    currentMPS->setText(i,os.str());
}

void mps_store_sd(int i, const char * s, int d)
{
    if(!currentMPS)
        return;

    std::ostringstream os;
    os<<s<<"\t"<<d;
    currentMPS->setText(i,os.str());
}

void mps_store_ssd(int i, const char * s1, const char * s2, int d)
{
    if(!currentMPS)
        return;

    std::ostringstream os;
    os<<s1<<"\t"<<s2<<"\t"<<d;
    currentMPS->setText(i,os.str());
}

void mps_store_sfp(int i, const char * s, double fl)
{
    if(!currentMPS)
        return;

    std::ostringstream os;
    os<<std::setprecision(1)<<std::fixed;
    os<<s<<"\t"<<fl<<"%";
    currentMPS->setText(i,os.str());
}

void mps_store_sdd(int i, const char * s, int d1, int d2)
{
    if(!currentMPS)
        return;

    std::ostringstream os;
    os<<s<<"\t"<<d1<<"\t"<<d2;
    currentMPS->setText(i,os.str());
}

void mps_store_sddp(int i, const char * s, int d, int max)
{
    if(!currentMPS)
        return;

    std::ostringstream os;
    os<<std::setprecision(1)<<std::fixed;
    os<<s<<"\t"<<d<<"\t"<<(d*100.0/max)<<"%";
    currentMPS->setText(i,os.str());
}

void mps_store_ssddp(int i, const char * s1, const char * s2,int d, int max)
{
    if(!currentMPS)
        return;

    std::ostringstream os;
    os<<std::setprecision(1)<<std::fixed;
    os<<s1<<s2<<"\t"<<d<<"\t"<<(d*100.0/max)<<"%";
    currentMPS->setText(i,os.str());
}


void mps_store_sss(int i, const char * s1, const char * s2, const char * s3)
{
    if(!currentMPS)
        return;

    std::ostringstream os;
    os<<s1<<"\t"<<s2<<"\t"<<s3;
    currentMPS->setText(i,os.str());
}

/* Data for new mps routines */
int mps_global_style = MPS_GLOBAL_FINANCE;

/* MPS Global displays */
void mps_right (int x, int y)
{
    int i = 0;
    //char s[12];
    const char* p;
    unsigned short group = world(x,y)->group;
    int pol = world(x,y)->pollution;
    currentMPS = envMPS;

    Uint8 *keystate = SDL_GetKeyState(NULL);
    if (!binary_mode && keystate[SDLK_d])
    {
        world(x,y)->saveMembers(&std::cout);
    }
    mps_store_sdd(i++, world(x, y)->getTileConstructionGroup()->name, x, y);

    p = ((world(x,y)->flags & FLAG_HAS_UNDERGROUND_WATER) != 0) ? "Yes" : "No";
    mps_store_ss(i++, "Fertile", p);
    if( group == GROUP_WATER)
    {
        p = (world(x,y)->flags & FLAG_IS_RIVER) ? "River" : "Lake";
        mps_store_title(i++, p);
    }
    else
    {
        i++;
    }
    p = (world(x,y)->flags & FLAG_FIRE_COVER) ? "Yes" : "No";
    mps_store_ss(i++,"Fire Protection",p);

    p = (world(x,y)->flags & FLAG_HEALTH_COVER) ? "Yes" : "No";
    mps_store_ss(i++,"Health Care",p);

    p = (world(x,y)->flags & FLAG_CRICKET_COVER) ? "Yes" : "No";
    mps_store_ss(i++,"Public Sports",p);

    p = (world(x,y)->flags & FLAG_MARKET_COVER) ? "Yes" : "No";
    mps_store_ss(i++,"Market Range",p);

    if (pol < 10)
    p = "clear";
    else if (pol < 25)
    p = "good";
    else if (pol < 70)
    p = "fair";
    else if (pol < 190)
    p = "smelly";
    else if (pol < 450)
    p = "smokey";
    else if (pol < 1000)
    p = "smoggy";
    else if (pol < 1700)
    p = "bad";
    else if (pol < 3000)
    p = "very bad";
    else
    p = "death!";

    mps_store_ssd(i++,"Pollution",p,pol);

    if (world(x,y)->reportingConstruction)
    {
        mps_store_sd(i++,"Bull. Cost", world(x,y)->reportingConstruction->constructionGroup->bul_cost);
    }
    else
    {
        if (group == GROUP_DESERT)
        {   mps_store_ss(i++,"Bull. Cost","N/A");}
        else
        {   mps_store_sd(i++, "Bull. Cost", world(x, y)->getTileConstructionGroup()->bul_cost);}
    }
    mps_store_sd(i++,"Ore Reserve",world(x,y)->ore_reserve);
    mps_store_sd(i++,"Coal Reserve",world(x,y)->coal_reserve);
    mps_store_sd(i++, "ground level", world(x,y)->ground.altitude);
/*  //Not needed if altitude == flooding level
    if(world(x,y)->is_water())
    {
        mps_store_sd(i++, "water level", world(x,y)->ground.water_alt);

    }
*/
    p = "-";
    if (world.saddlepoint(x,y))
    {   p = "saddle point";}
    else if (!world(x,y)->is_water() && world.minimum(x,y))
    {   p = "minimum";}
    else if (!world(x,y)->is_water() && world.maximum(x,y))
    {   p = "maximum";}
    else if (world.checkEdgeMin(x,y))
    {   p = "lowest edge";}

    mps_store_title(i++, p);
    currentMPS = 0;
}

void mps_global_finance()
{
    int i = 0;
    char s[12];

    int cashflow = 0;
    currentMPS = globalMPS;

    mps_store_title(i++,"Tax Income");

    cashflow += ly_income_tax;
    num_to_ansi (s, sizeof(s), ly_income_tax);
    mps_store_ss(i++,"Income", s);

    cashflow += ly_coal_tax;
    num_to_ansi(s, sizeof(s), ly_coal_tax);
    mps_store_ss(i++,"Coal", s);

    cashflow += ly_goods_tax;
    num_to_ansi(s, sizeof(s), ly_goods_tax);
    mps_store_ss(i++,"Goods", s);

    cashflow += ly_export_tax;
    num_to_ansi(s, sizeof(s), ly_export_tax);
    mps_store_ss(i++,"Export", s);

    mps_store_title(i++, "");

    mps_store_title(i++,"Expenses");

    cashflow -= ly_unemployment_cost;
    num_to_ansi(s, sizeof(s), ly_unemployment_cost);
    mps_store_ss(i++,"Unemp.", s);

    cashflow -= ly_transport_cost;
    num_to_ansi(s, sizeof(s), ly_transport_cost);
    mps_store_ss(i++,"Transport", s);

    cashflow -= ly_import_cost;
    num_to_ansi(s, sizeof(s), ly_import_cost);
    mps_store_ss(i++,"Imports", s);

    cashflow -= ly_other_cost;
    num_to_ansi(s, sizeof(s), ly_other_cost);
    mps_store_ss(i++,"Others", s);

    mps_store_title(i++, "" );

    num_to_ansi(s, sizeof(s), cashflow);
    mps_store_ss(i++,"Net", s);

    currentMPS = 0;
}

void mps_global_other_costs()
{
    int i = 0;
    int year;
    char s[12];

    currentMPS = globalMPS;
    mps_store_title(i++,"Other Costs");

    /* Don't write year if it's negative. */
    year = (total_time / NUMOF_DAYS_IN_YEAR) - 1;
    if (year >= 0) {
    mps_store_sd(i++, "For year", year);
    }
    mps_store_title(i++,"");
    num_to_ansi(s,sizeof(s),ly_interest);
    mps_store_ss(i++,"Interest",s);
    num_to_ansi(s,sizeof(s),ly_school_cost);
    mps_store_ss(i++,"Schools",s);
    num_to_ansi(s,sizeof(s),ly_university_cost);
    mps_store_ss(i++,"Univers.",s);
    num_to_ansi(s,sizeof(s),ly_deaths_cost);
    mps_store_ss(i++,"Deaths",s);
    num_to_ansi(s,sizeof(s),ly_windmill_cost);
    mps_store_ss(i++,"Windmill",s);
    num_to_ansi(s,sizeof(s),ly_health_cost);
    mps_store_ss(i++,"Hospital",s);
    num_to_ansi(s,sizeof(s),ly_rocket_pad_cost);
    mps_store_ss(i++,"Rockets",s);
    num_to_ansi(s,sizeof(s),ly_fire_cost);
    mps_store_ss(i++,"Fire Stn",s);
    num_to_ansi(s,sizeof(s),ly_cricket_cost);
    mps_store_ss(i++,"Sport",s);
    num_to_ansi(s,sizeof(s),ly_recycle_cost);
    mps_store_ss(i++,"Recycle",s);

    currentMPS = 0;
}

void mps_global_housing()
{
    int days = total_time % NUMOF_DAYS_IN_MONTH +1; // 1..NUMOF_DAYS_IN_MONTH

    int i = 0;
    int tp = population + people_pool;

    currentMPS = globalMPS;

    mps_store_title(i++,"Population");
    mps_store_title(i++,"");
    mps_store_sd(i++,"Total",tp);
    mps_store_sd(i++,"Housed",population);
    mps_store_sd(i++,"Homeless",people_pool);
    mps_store_sd(i++,"Shanties",Counted<Shanty>::getInstanceCount());
    mps_store_sd(i++,"Unnat. Deaths",unnat_deaths);
    mps_store_title(i++,"Unemployment");
    mps_store_sd(i++,"Claims",tunemployed_population/days);
    mps_store_sfp(i++,"Rate",
          (((tunemployed_population/days) * 100.0) / tp));
    mps_store_title(i++,"Starvation");
    mps_store_sd(i++,"Cases",tstarving_population/days);

    mps_store_sfp(i++,"Rate",
          (((tstarving_population/days) * 100.0) / tp));

    currentMPS = 0;
}


/** @file lincity-ng/MpsInterface.cpp */

