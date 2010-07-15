/* ---------------------------------------------------------------------- *
 * simulate.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "lcstring.h"
#include <sys/types.h>
#include <fcntl.h>

/*
#if defined (WIN32)
#include <winsock.h>
#include <io.h>
#include <direct.h>
#include <process.h>
#endif
*/
#include <ctype.h>
//#include "common.h"

/*
#ifdef LC_X11
#include <X11/cursorfont.h>
#endif
*/

#include "tinygettext/gettext.hpp"

#include "init_game.h"
#include "simulate.h"
#include "gui_interface/shared_globals.h"
#include "lctypes.h"
#include "lin-city.h"
#include "engglobs.h"
#include "gui_interface/screen_interface.h"
#include "power.h"
#include "stats.h"
#include "gui_interface/pbar_interface.h"
#include "modules/modules_interfaces.h"
#include "modules/all_modules.h"
#include "transport.h"
#include "all_buildings.h"
#include "sustainable.h"
#include "engine.h"

/* extern resources */
extern void print_total_money(void);
void setLincitySpeed( int speed );
extern void ok_dial_box(const char *, int, const char *);

/* AL1: they are all in engine.cpp */
extern void do_daily_ecology(void);
extern void do_pollution(void);
extern void do_fire_health_cricket_power_cover(void);

/* Flag to warn users that they have 10 years to put waterwell everywhere */
int flag_warning = false;

/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */
static void shuffle_mappoint_array(void);
static void do_periodic_events(void);
static void end_of_month_update(void);
static void start_of_year_update(void);
static void end_of_year_update(void);
static void simulate_mappoints(void);
static void set_mappoint_used(int fromx, int fromy, int x, int y);

static void sustainability_test(void);
static int sust_fire_cover(void);

/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
void do_time_step(void)
{
    if (flag_warning) {
        flag_warning = false;
        /* FIXME use blocking_dialog_open instead */
        setLincitySpeed(0);
        ok_dial_box("warning.mes", GOOD, \
                _("Upgrading from old game. You have 10 years to build water wells where needed. After, starvation will occur!\
  You should check starvation minimap, and read waterwell help page :-)") );

    }
    /* Increment game time */
    total_time++;
#ifdef DEBUG_ENGINE
    printf("In do_time_step (%d)\n", total_time);
#endif

    /* Initialize daily accumulators */
    init_daily();

    /* Initialize monthly accumulators */
    if (total_time % NUMOF_DAYS_IN_MONTH == 0) {
        init_monthly();
    }

    /* Initialize yearly accumulators */
    if ((total_time % NUMOF_DAYS_IN_YEAR) == 0) {
        init_yearly();
    }

    /* Clear the power grid */
    power_time_step();

    /* Run through simulation equations for each farm, residence, etc. */
    simulate_mappoints();

    /* Now do the stuff that happens once a year, once a month, etc. */
    do_periodic_events();
}

int count_groups(int g)
{
    int x, y, i;
    i = 0;
    for (y = 0; y < WORLD_SIDE_LEN; y++)
        for (x = 0; x < WORLD_SIDE_LEN; x++)
            if (MP_GROUP(x, y) == g)
                i++;
    return (i);
}

void count_all_groups(int *group_count)
{
    int x, y;
    unsigned short t, g;
    for (x = 0; x < NUM_OF_GROUPS; x++)
        group_count[x] = 0;
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
        for (x = 0; x < WORLD_SIDE_LEN; x++) {
            t = MP_TYPE(x, y);
            if (t != CST_USED && !GROUP_IS_BARE(MP_GROUP(x, y))) {
                g = get_group_of_type(t);
                group_count[g]++;
            }
        }
    }
}

void set_mappoint(int x, int y, short selected_type)
{
    int grp;
    int size;

    if ((grp = get_group_of_type(selected_type)) < 0)
        return;

    MP_TYPE(x, y) = selected_type;
    MP_GROUP(x, y) = grp;
    size = main_groups[grp].size;

    for (int i = 0; i<size; i++)
        for (int j = 0; j<size; j++) {
            if ( i == 0 && j == 0 )
                continue;
            set_mappoint_used(x, y, x+i, y+j);
        }

}

/* ---------------------------------------------------------------------- *
 * Private Functions
 * ---------------------------------------------------------------------- */

/** this is called at the beginning of every frame */
static void shuffle_mappoint_array(void)
{
    /** Mappoint array shuffles mappoint in order to stop linear simulation effects */
    int i, x, a;
    for (i = 0; i < SHUFFLE_MAPPOINT_COUNT; i++) {
        x = rand() % WORLD_SIDE_LEN;
        a = mappoint_array_x[i];
        mappoint_array_x[i] = mappoint_array_x[x];
        mappoint_array_x[x] = a;
        x = rand() % WORLD_SIDE_LEN;
        a = mappoint_array_y[i];
        mappoint_array_y[i] = mappoint_array_y[x];
        mappoint_array_y[x] = a;
    }
}

static void do_periodic_events(void)
{
    add_daily_to_monthly();
    do_daily_ecology();

    if ((total_time % NUMOF_DAYS_IN_YEAR) == 0) {
        start_of_year_update();
    }
    if ((total_time % DAYS_PER_POLLUTION) == 3) {
        do_pollution();
    }
    if ((total_time % DAYS_BETWEEN_FIRES) == 9 && tech_level > (GROUP_FIRESTATION_TECH * MAX_TECH_LEVEL / 1000)) {
        do_random_fire(-1, -1, 1);
    }
    if ((total_time % DAYS_BETWEEN_COVER) == 75) {
        do_fire_health_cricket_power_cover();
    }
    if ((total_time % DAYS_BETWEEN_SHANTY) == 85 && tech_level > (GROUP_HEALTH_TECH * MAX_TECH_LEVEL / 1000)) {
        update_shanty();
    }
    if (total_time % NUMOF_DAYS_IN_MONTH == (NUMOF_DAYS_IN_MONTH - 1)) {
        end_of_month_update();
    }
    if (total_time % NUMOF_DAYS_IN_YEAR == (NUMOF_DAYS_IN_YEAR - 1)) {
        end_of_year_update();
    }
}

static void end_of_month_update(void)
{
    housed_population = (tpopulation / NUMOF_DAYS_IN_MONTH);
    if ((housed_population + people_pool) > max_pop_ever)
        max_pop_ever = housed_population + people_pool;

    if (people_pool > 100) {
        if (rand() % 1000 < people_pool)
            people_pool -= 10;
    }
    if (people_pool < 0)
        people_pool = 0;

    if (tech_level > TECH_LEVEL_LOSS_START) {
        tech_level -= (int)(tech_level * (1. / TECH_LEVEL_LOSS)
                            * (1 + (tpopulation * (1. / NUMOF_DAYS_IN_MONTH / 120 / (TECH_LEVEL_LOSS - 200)))));

    } else
        tech_level += TECH_LEVEL_UNAIDED;
    /* we can go over 100, but it's even more difficult */
    if (tech_level > MAX_TECH_LEVEL)
        tech_level -= (int)((tech_level - MAX_TECH_LEVEL)
                            * (1. / TECH_LEVEL_LOSS)
                            * (1 + (tpopulation * (1. / NUMOF_DAYS_IN_MONTH / 120 / (TECH_LEVEL_LOSS - 100)))));

    if (highest_tech_level < tech_level)
        highest_tech_level = tech_level;

    deaths_cost += unnat_deaths * UNNAT_DEATHS_COST;

}

static void start_of_year_update(void)
{
    int u;

    sustainability_test();

    pollution_deaths_history -= pollution_deaths_history / 100.0;
    starve_deaths_history -= starve_deaths_history / 100.0;
    unemployed_history -= unemployed_history / 100.0;
    u = count_groups(GROUP_UNIVERSITY);
    if (u > 0) {
        university_intake_rate = (count_groups(GROUP_SCHOOL) * 20) / u;
        if (university_intake_rate > 100)
            university_intake_rate = 100;
    } else {
        university_intake_rate = 50;
    }

    map_power_grid();
}

static void end_of_year_update(void)
{
    income_tax = (income_tax * income_tax_rate) / 100;
    ly_income_tax = income_tax;
    total_money += income_tax;

    coal_tax = (coal_tax * coal_tax_rate) / 100;
    ly_coal_tax = coal_tax;
    total_money += coal_tax;

    goods_tax = (goods_tax * goods_tax_rate) / 100;
    goods_tax += (int)((float)(goods_tax * goods_tax_rate)
                       * (float)tech_level / 2000000.0);
    ly_goods_tax = goods_tax;
    total_money += goods_tax;

    /* The price of exports on the world market drops as you export more.
       The exporters have to discount there wares, therefore the
       tax take is less.
     */
    if (export_tax > ex_tax_dis[0]) {
        int discount, disi;
        discount = 0;
        for (disi = 0; disi < NUMOF_DISCOUNT_TRIGGERS && export_tax > ex_tax_dis[disi]; disi++)
            discount += (export_tax - ex_tax_dis[disi]) / 10;
        export_tax -= discount;
    }
    ly_export_tax = export_tax;
    total_money += export_tax;

    ly_university_cost = university_cost;
    ly_recycle_cost = recycle_cost;
    ly_deaths_cost = deaths_cost;
    ly_health_cost = (health_cost * (tech_level / 10000)
                      * HEALTH_RUNNING_COST_MUL) / (MAX_TECH_LEVEL / 10000);
    ly_rocket_pad_cost = rocket_pad_cost;
    ly_school_cost = school_cost;
    ly_windmill_cost = windmill_cost;
    ly_fire_cost = (fire_cost * (tech_level / 10000)
                    * FIRESTATION_RUNNING_COST_MUL) / (MAX_TECH_LEVEL / 10000);
    ly_cricket_cost = cricket_cost;
    if (total_money < 0) {
        ly_interest = ((-total_money / 1000) * INTEREST_RATE);
        if (ly_interest > 1000000)
            ly_interest = 1000000;
    } else
        ly_interest = 0;

    other_cost = university_cost + recycle_cost + deaths_cost
        + ly_health_cost + rocket_pad_cost + school_cost + ly_interest + windmill_cost + ly_fire_cost + ly_cricket_cost;
    ly_other_cost = other_cost;
    total_money -= other_cost;

    unemployment_cost = (unemployment_cost * dole_rate) / 100;
    ly_unemployment_cost = unemployment_cost;
    total_money -= unemployment_cost;

    transport_cost = (transport_cost * transport_cost_rate) / 100;
    ly_transport_cost = transport_cost;
    total_money -= transport_cost;

    import_cost = (import_cost * import_cost_rate) / 100;
    ly_import_cost = import_cost;
    total_money -= import_cost;

    if (total_money > 2000000000)
        total_money = 2000000000;
    else if (total_money < -2000000000)
        total_money = -2000000000;

    print_total_money();
}

static void simulate_mappoints(void)
{
    int xx, yy;
    shuffle_mappoint_array();
    for (yy = 0; yy < WORLD_SIDE_LEN; yy++) {
        /* indirection to rand array to stop lots of linear effects */
        int y = mappoint_array_y[yy];
        for (xx = 0; xx < WORLD_SIDE_LEN; xx++) {
            int x = mappoint_array_x[xx];
            short grp = MP_GROUP(x, y);
            if (grp == GROUP_USED || GROUP_IS_BARE(grp))
                continue;
            switch (grp) {
            case GROUP_TRACK:
            case GROUP_TRACK_BRIDGE:
                do_track(x, y);
                break;
            case GROUP_RAIL:
            case GROUP_RAIL_BRIDGE:
                do_rail(x, y);
                break;
            case GROUP_ROAD:
            case GROUP_ROAD_BRIDGE:
                do_road(x, y);
                break;
            case GROUP_ORGANIC_FARM:
                do_organic_farm(x, y);
                break;
            case GROUP_MARKET:
                do_market(x, y);
                break;
            case GROUP_RESIDENCE_LL:
            case GROUP_RESIDENCE_ML:
            case GROUP_RESIDENCE_HL:
            case GROUP_RESIDENCE_LH:
            case GROUP_RESIDENCE_MH:
            case GROUP_RESIDENCE_HH:
                do_residence(x, y);
                break;
            case GROUP_POWER_LINE:
                do_power_line(x, y);
                break;
            case GROUP_SOLAR_POWER:
                do_power_source_solar(x, y);
                break;
            case GROUP_SUBSTATION:
                do_power_substation(x, y);
                break;
            case GROUP_COALMINE:
                do_coalmine(x, y);
                break;
            case GROUP_COAL_POWER:
                do_power_source_coal(x, y);
                break;
            case GROUP_INDUSTRY_L:
                do_industry_l(x, y);
                break;
            case GROUP_INDUSTRY_H:
                do_industry_h(x, y);
                break;
            case GROUP_COMMUNE:
                do_commune(x, y);
                break;
            case GROUP_OREMINE:
                do_oremine(x, y);
                break;
            case GROUP_PORT:
                do_port(x, y);
                break;
            case GROUP_TIP:
                do_tip(x, y);
                break;
            case GROUP_PARKLAND:
                do_parkland(x, y);
                break;
            case GROUP_UNIVERSITY:
                do_university(x, y);
                break;
            case GROUP_RECYCLE:
                do_recycle(x, y);
                break;
            case GROUP_HEALTH:
                do_health_centre(x, y);
                break;
            case GROUP_ROCKET:
                do_rocket_pad(x, y);
                break;
            case GROUP_WINDMILL:
                do_windmill(x, y);
                break;
            case GROUP_MONUMENT:
                do_monument(x, y);
                break;
            case GROUP_SCHOOL:
                do_school(x, y);
                break;
            case GROUP_BLACKSMITH:
                do_blacksmith(x, y);
                break;
            case GROUP_MILL:
                do_mill(x, y);
                break;
            case GROUP_POTTERY:
                do_pottery(x, y);
                break;
            case GROUP_FIRESTATION:
                do_firestation(x, y);
                break;
            case GROUP_CRICKET:
                do_cricket(x, y);
                break;
            case GROUP_FIRE:
                do_fire(x, y);
                break;
            case GROUP_SHANTY:
                do_shanty(x, y);
                break;
            }
        }
    }
}

static void sustainability_test(void)
{
    int i;
    if (sust_dig_ore_coal_tip_flag == 0) {
        sust_dig_ore_coal_tip_flag = 1;
        sust_dig_ore_coal_count = 0;
    } else
        sust_dig_ore_coal_count++;

    if (sust_port_flag == 0) {
        sust_port_flag = 1;
        sust_port_count = 0;
    } else
        sust_port_count++;

    /* Money must be going up or the same. (ie can't build.) */
    if (sust_old_money > total_money)
        sust_old_money_count = 0;
    else
        sust_old_money_count++;
    sust_old_money = total_money;

    /* population must be withing 2% of when it started. */
    i = (housed_population + people_pool) - sust_old_population;
    if (abs(i) > (sust_old_population / 40)     /* 2.5%  */
        ||(housed_population + people_pool) < SUST_MIN_POPULATION) {
        sust_old_population = (housed_population + people_pool);
        sust_old_population_count = 0;
    } else
        sust_old_population_count++;

    /* tech level must be going up or not fall more than 0.5% from it's
       highest during the sus count
     */
    i = tech_level - sust_old_tech;
    if (i < 0 || tech_level < SUST_MIN_TECH_LEVEL) {
        i = -i;
        if ((i > sust_old_tech / 100) || tech_level < SUST_MIN_TECH_LEVEL) {
            sust_old_tech_count = 0;
            sust_old_tech = tech_level;
        } else
            sust_old_tech_count++;
    } else {
        sust_old_tech_count++;
        sust_old_tech = tech_level;
    }

    /* check fire cover only every three years */
    if (total_time % (NUMOF_DAYS_IN_YEAR * 3) == 0) {
        if (sust_fire_cover() != 0)
            sust_fire_count += 3;
        else
            sust_fire_count = 0;

    }
}

static int sust_fire_cover(void)
{
    int x, y;
    for (x = 0; x < WORLD_SIDE_LEN; x++)
        for (y = 0; y < WORLD_SIDE_LEN; y++) {
            if (GROUP_IS_BARE(MP_GROUP(x, y))
                || MP_TYPE(x, y) == CST_USED || MP_GROUP(x, y) == GROUP_WATER || MP_GROUP(x, y) == GROUP_POWER_LINE \
                                || MP_GROUP(x, y) == GROUP_OREMINE || MP_GROUP(x, y) == GROUP_ROCKET \
                                || MP_GROUP(x, y) == GROUP_MONUMENT || MP_GROUP(x, y) == GROUP_BURNT) ;/* do nothing */

            else if ((MP_INFO(x, y).flags & FLAG_FIRE_COVER) == 0)
                return (0);
        }
    return (1);
}

static void set_mappoint_used(int fromx, int fromy, int x, int y)
{
    MP_TYPE(x, y) = CST_USED;
    MP_GROUP(x, y) = GROUP_USED;
    MP_INFO(x, y).int_1 = fromx;
    MP_INFO(x, y).int_2 = fromy;
}


/** @file lincity/simulate.cpp */

