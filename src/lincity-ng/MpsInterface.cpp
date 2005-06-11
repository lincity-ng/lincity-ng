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
#include "lincity/lcintl.h"
#include "lincity/modules/all_modules.h"
#include "tinygettext/gettext.hpp"
#include "MiniMap.hpp"

// implement everything here

// this is the MPS-status display, which describes the cells

int mps_x = 0;
int mps_y = 0;
int mps_style = 0;

Mps* currentMPS = 0;

void mps_full_refresh()
{
}

void mappoint_stats(int, int, int)
{
}

void mps_init()
{
}

int mps_set_silent(int x, int y, int style)
{
    int same_square = 0;
    mps_style = style;
    switch(style) {
        case MPS_MAP:
        case MPS_ENV: 
            if (mps_x == x && mps_y == y) {
                same_square = 1;
            }
            mps_x = x;
            mps_y = y;
            break;
        default: 
            break;
    }
    return same_square;
}

int mps_set( int x, int y, int style ) /* Attaches an area or global display */
{
    int same_square = mps_set_silent(x, y, style);

    switch(style) {
        case MPS_MAP:
            if(mapMPS)
                mapMPS->clear();
            getMiniMap()->switchView("MapMPS");
            break;
        case MPS_ENV:
            if(envMPS)
                envMPS->clear();
            getMiniMap()->switchView("EnvMPS");
            break;
        case MPS_GLOBAL:
            if(globalMPS)
                globalMPS->clear();
            getMiniMap()->switchView("GlobalMPS");
            break;
        default:
            assert(false);
            break;
    }                                                 
    mps_refresh();
    return same_square;
}

void mps_redraw()  /* Re-draw the mps area, bezel and all */
{
    mps_refresh();
}

void mps_refresh() /* refresh the information display's contents */
{
    switch (mps_style) {
        case MPS_MAP:
            currentMPS = mapMPS;
            switch(MP_GROUP(mps_x, mps_y)) 
            {
                case GROUP_BLACKSMITH:
                    mps_blacksmith (mps_x, mps_y);
                    break;
                case GROUP_COALMINE:
                    mps_coalmine (mps_x, mps_y);
                    break;
                case GROUP_COAL_POWER:
                    mps_coal_power (mps_x, mps_y);
                    break;
                case GROUP_COMMUNE:
                    mps_commune (mps_x, mps_y);
                    break;
                case GROUP_CRICKET:
                    mps_cricket (mps_x, mps_y);
                    break;
                case GROUP_FIRESTATION:
                    mps_firestation (mps_x, mps_y);
                    break;
                case GROUP_HEALTH:
                    mps_health_centre (mps_x, mps_y);
                    break;
                case GROUP_INDUSTRY_H:
                    mps_heavy_industry (mps_x, mps_y);
                    break;
                case GROUP_INDUSTRY_L:
                    mps_light_industry (mps_x, mps_y);
                    break;
                case GROUP_MILL:
                    mps_mill (mps_x, mps_y);
                    break;
                case (GROUP_MONUMENT):
                    mps_monument (mps_x, mps_y);
                    break;
                case (GROUP_OREMINE):
                    mps_oremine (mps_x, mps_y);
                    break;
                case GROUP_ORGANIC_FARM: 
                    mps_organic_farm(mps_x, mps_y);
                    break;
                case GROUP_PORT:
                    mps_port (mps_x, mps_y);
                    break;
                case GROUP_POTTERY:
                    mps_pottery (mps_x, mps_y);
                    break;
                case GROUP_POWER_LINE:
                    mps_power_line (mps_x, mps_y);
                    break;
                case GROUP_RAIL:
                    mps_rail (mps_x, mps_y);
                    break;
                case GROUP_RECYCLE:
                    mps_recycle (mps_x, mps_y);
                    break;
                case GROUP_RESIDENCE_LL:
                    mps_residence(mps_x, mps_y);
                    break;
                case GROUP_RESIDENCE_LH:
                    mps_residence(mps_x, mps_y);
                    break;
                case GROUP_RESIDENCE_ML:
                case GROUP_RESIDENCE_MH:
                case GROUP_RESIDENCE_HL:
                case GROUP_RESIDENCE_HH:
                    mps_residence(mps_x, mps_y);
                    break;
                case GROUP_ROAD:
                    mps_road (mps_x, mps_y);
                    break;
                case GROUP_ROCKET:
                    mps_rocket (mps_x, mps_y);
                    break;
                case GROUP_SCHOOL:
                    mps_school (mps_x, mps_y);
                    break;
                case GROUP_SOLAR_POWER:
                    mps_solar_power (mps_x, mps_y);
                    break;
                case GROUP_SUBSTATION:
                    mps_substation (mps_x, mps_y);
                    break;
                case GROUP_TIP:
                    mps_tip (mps_x, mps_y);
                    break;
                case GROUP_TRACK:
                    mps_track(mps_x, mps_y);
                    break;
                case GROUP_MARKET:
                    mps_market (mps_x, mps_y);
                    break;
                case GROUP_UNIVERSITY:
                    mps_university (mps_x, mps_y);
                    break;
                case GROUP_WATER:
                    mps_water (mps_x, mps_y);
                    break;
                case GROUP_WINDMILL:
                    mps_windmill (mps_x, mps_y);
                    break;
                default: 
                    //no special information on this group, just show the Name.
                    mps_store_title(0, 
                            _(main_groups[MP_GROUP(mps_x, mps_y)].name));
                    if( MP_TYPE( mps_x, mps_y ) == CST_GREEN ){
                        mps_store_title(4,_("build something here") );
                    }
                    mps_store_title(2, _("no further information available") );
                    
                    //printf("MPS unimplemented for that module\n");
            }
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

/** Changes global var to next display */
void mps_global_advance()
{
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
    os<<s1<<": "<<s2;
    currentMPS->setText(i,os.str());
}

void mps_store_sd(int i, const char * s, int d)
{
    if(!currentMPS)
        return;
    
    std::ostringstream os;
    os<<s<<": "<<d;
    currentMPS->setText(i,os.str());
}

void mps_store_sfp(int i, const char * s, double fl)
{
    if(!currentMPS)
        return;
    
    std::ostringstream os;
    os<<std::setprecision(1)<<std::fixed;
    os<<s<<": "<<fl<<"%";
    currentMPS->setText(i,os.str());
}

void mps_store_sss(int i, const char * s1, const char * s2, const char * s3)
{
    if(!currentMPS)
        return;
    
    std::ostringstream os;
    os<<s1<<": "<<s2<<" "<<s3;
    currentMPS->setText(i,os.str());
}

/* Data for new mps routines */
//extern char mps_info[MAPPOINT_STATS_LINES][MPS_INFO_CHARS];
int mps_global_style = MPS_GLOBAL_FINANCE;

/* MPS Global displays */
void mps_right (int x, int y)
{
    int i = 0;
    char s[12];
    const char* p;
    int g;

    currentMPS = envMPS;

    snprintf(s,sizeof(s),"%d,%d",x,y);
    mps_store_title(i++,s);
    i++;
    mps_store_title(i++,_("Coverage"));
    p = (MP_INFO(x,y).flags & FLAG_FIRE_COVER) ? _("Yes") : _("No");
    mps_store_ss(i++,_("Fire"),p);

    p = (MP_INFO(x,y).flags & FLAG_HEALTH_COVER) ? _("Yes") : _("No");
    mps_store_ss(i++,_("Health"),p);

    p = (MP_INFO(x,y).flags & FLAG_CRICKET_COVER) ? _("Yes") : _("No");
    mps_store_ss(i++,_("Sport"),p);
    i++;
    mps_store_title(i++,_("Pollution"));

    if (MP_POL(x,y) < 10)
	p = _("clear");
    else if (MP_POL(x,y) < 25)
	p = _("good");
    else if (MP_POL(x,y) < 70)
	p = _("fair");
    else if (MP_POL(x,y) < 190)
	p = _("smelly");
    else if (MP_POL(x,y) < 450)
	p = _("smokey");
    else if (MP_POL(x,y) < 1000)
	p = _("smoggy");
    else if (MP_POL(x,y) < 1700)
	p = _("bad");
    else if (MP_POL(x,y) < 3000)
	p = _("very bad");
    else
	p = _("death!");

    mps_store_sd(i++,p,MP_POL(x,y));
    i++;

    mps_store_title(i++,_("Bulldoze Cost"));
    if (MP_TYPE(x,y) == CST_USED)
        g = MP_GROUP( MP_INFO(x,y).int_1, MP_INFO(x,y).int_2 );
    else
        g = MP_GROUP(x,y);
    if (g == 0) {	/* Can't bulldoze grass. */
	mps_store_title(i++,_("N/A"));
    } else {
	if (g < 7)
	    g--;			/* translate into button type */
	mps_store_d(i++,main_groups[g].bul_cost);
    }

    currentMPS = 0;
}

void mps_global_finance()
{
    int i = 0;
    char s[12];

    int cashflow = 0;
    currentMPS = globalMPS;

    mps_store_title(i++,_("Tax Income"));

    cashflow += ly_income_tax;
    num_to_ansi (s, 12, ly_income_tax);
    mps_store_ss(i++,_("Income"), s);

    cashflow += ly_coal_tax;
    num_to_ansi(s, 12, ly_coal_tax);
    mps_store_ss(i++,_("Coal"), s);

    cashflow += ly_goods_tax;
    num_to_ansi(s, 12, ly_goods_tax);
    mps_store_ss(i++,_("Goods"), s);

    cashflow += ly_export_tax;
    num_to_ansi(s, 12, ly_export_tax);
    mps_store_ss(i++,_("Export"), s);

    mps_store_title(i++, "");

    mps_store_title(i++,_("Expenses"));

    cashflow -= ly_unemployment_cost;
    num_to_ansi(s, 12, ly_unemployment_cost);
    mps_store_ss(i++,_("Unemp."), s);

    cashflow -= ly_transport_cost;
    num_to_ansi(s, 12, ly_transport_cost);
    mps_store_ss(i++,_("Transport"), s);

    cashflow -= ly_import_cost;
    num_to_ansi(s, 12, ly_import_cost);
    mps_store_ss(i++,_("Imports"), s);

    cashflow -= ly_other_cost;
    num_to_ansi(s, 12, ly_other_cost);
    mps_store_ss(i++,_("Others"), s);

    mps_store_title(i++, "" );

    num_to_ansi(s, 12, cashflow);
    mps_store_ss(i++,_("Net"), s);

    currentMPS = 0;
}    

void mps_global_other_costs()
{
    int i = 0;
    int year;
    char s[12];

    currentMPS = globalMPS;
    mps_store_title(i++,_("Other Costs"));

    /* Don't write year if it's negative. */
    year = (total_time / NUMOF_DAYS_IN_YEAR) - 1;
    if (year >= 0) {
	mps_store_sd(i++, _("For year"), year);
    }
    mps_store_title(i++,"");
    num_to_ansi(s,sizeof(s),ly_interest);
    mps_store_ss(i++,_("Interest"),s);
    num_to_ansi(s,sizeof(s),ly_school_cost);
    mps_store_ss(i++,_("Schools"),s);
    num_to_ansi(s,sizeof(s),ly_university_cost);
    mps_store_ss(i++,_("Univers."),s);
    num_to_ansi(s,sizeof(s),ly_deaths_cost);
    mps_store_ss(i++,_("Deaths"),s);
    num_to_ansi(s,sizeof(s),ly_windmill_cost);
    mps_store_ss(i++,_("Windmill"),s);
    num_to_ansi(s,sizeof(s),ly_health_cost);
    mps_store_ss(i++,_("Hospital"),s);
    num_to_ansi(s,sizeof(s),ly_rocket_pad_cost);
    mps_store_ss(i++,_("Rockets"),s);
    num_to_ansi(s,sizeof(s),ly_fire_cost);
    mps_store_ss(i++,_("Fire Stn"),s);
    num_to_ansi(s,sizeof(s),ly_cricket_cost);
    mps_store_ss(i++,_("Sport"),s);
    num_to_ansi(s,sizeof(s),ly_recycle_cost);
    mps_store_ss(i++,_("Recycle"),s);

    currentMPS = 0;
}

void mps_global_housing()
{
    int i = 0;
    int tp = housed_population + people_pool;

    currentMPS = globalMPS;

    mps_store_title(i++,_("Population"));
    mps_store_title(i++,"");
    mps_store_sd(i++,_("Total"),tp);
    mps_store_sd(i++,_("Housed"),housed_population);
    mps_store_sd(i++,_("Homeless"),people_pool);
    mps_store_sd(i++,_("Shanties"),numof_shanties);
    mps_store_sd(i++,_("Unn Dths"),unnat_deaths);
    mps_store_title(i++,_("Unemployment"));
    mps_store_sd(i++,_("Claims"),tunemployed_population);
    mps_store_sfp(i++,_("Rate"),
		  ((tunemployed_population * 100.0) / tp));
    mps_store_title(i++,_("Starvation"));
    mps_store_sd(i++,_("Cases"),tstarving_population);

    mps_store_sfp(i++,_("Rate"),
		  ((tstarving_population * 100.0) / tp));

    currentMPS = 0;
}

