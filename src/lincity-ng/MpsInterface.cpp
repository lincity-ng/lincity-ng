#include <config.h>

#include "gui_interface/mps.h"

#include "Mps.hpp"

#include <sstream>
#include <iomanip>

#include "lincity/stats.h"
#include "lincity/lclib.h"
#include "lincity/engglobs.h"
#include "lincity/lcintl.h"

// implement everything here

// this is the MPS-status display, which describes the cells


void mps_full_refresh (void)
{
}
void mappoint_stats (int, int, int)
{
}

void mps_init()
{
}

int mps_set(int style, int x, int y) /* Attaches an area or global display */
{
    (void) style;
    (void) x;
    (void) y;
    return 0;
}
void mps_redraw(void)  /* Re-draw the mps area, bezel and all */
{
}
void mps_refresh(void) /* refresh the information display's contents */
{
}
void mps_update(void)  /* Update text contents for later display (refresh) */
{
}
void mps_global_advance(void) /* Changes global var to next display */
{
}


/* mps_info storage functions; place values of corresponding type into
   mps_info[], performing certain pretification. The single argument 
   forms center their argument.  The dual arguments left-justify the
   first and right-justify the second.  the ..p forms put a % after
   the second argument 
 */
void mps_store_title(int i, const char * t)
{
    getMPS()->setText(i,t);
}
void mps_store_fp(int i, double f)
{
    (void) i;
    (void) f;
}
void mps_store_f(int i, double f)
{
    (void) i;
    (void) f;
}
void mps_store_d(int i, int d)
{
    (void) i;
    (void) d;
}

void mps_store_ss(int i, const char * s1, const char * s2)
{
    std::ostringstream os;
    os<<s1<<": "<<s2;
    getMPS()->setText(i,os.str());
}
void mps_store_sd(int i, const char * s, int d)
{
    std::ostringstream os;
    os<<s<<": "<<d;
    getMPS()->setText(i,os.str());
}
void mps_store_sfp(int i, const char * s, double fl)
{
    std::ostringstream os;
    os<<std::setprecision(1)<<std::fixed;
    os<<s<<": "<<fl<<"%";
    getMPS()->setText(i,os.str());
}

void mps_store_sss(int i, const char * s1, const char * s2, const char * s3)
{
    std::ostringstream os;
    os<<s1<<": "<<s2<<" "<<s3;
    getMPS()->setText(i,os.str());
}

/* Data for new mps routines */
//extern char mps_info[MAPPOINT_STATS_LINES][MPS_INFO_CHARS];
int mps_global_style;

/* MPS Global displays */
void mps_right (int x, int y)
{
    (void) x;
    (void) y;
}

void mps_global_finance(void) {
    int i = 0;
    char s[12];

    int cashflow = 0;

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

    i++;

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

    i++;

    num_to_ansi(s, 12, cashflow);
    mps_store_ss(i++,_("Net"), s);
}    

void 
mps_global_other_costs (void)
{
    int i = 0;
    int year;
    char s[12];

    mps_store_title(i++,_("Other Costs"));

    /* Don't write year if it's negative. */
    year = (total_time / NUMOF_DAYS_IN_YEAR) - 1;
    if (year >= 0) {
	mps_store_sd(i++, _("For year"), year);
    }
    i++;
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
    mps_store_ss(i++,_("Cricket"),s);
    num_to_ansi(s,sizeof(s),ly_recycle_cost);
    mps_store_ss(i++,_("Recycle"),s);
}


void 
mps_global_housing (void)
{
    int i = 0;
    int tp = housed_population + people_pool;

    mps_store_title(i++,_("Population"));
    i++;
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
}
