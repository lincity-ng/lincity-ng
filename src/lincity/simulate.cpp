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

#include "simulate.h"
#include "gui_interface/shared_globals.h"
#include "lctypes.h"
#include "lin-city.h"
#include "ldsvguts.h"
#include "engglobs.h"
#include "gui_interface/screen_interface.h"
#include "power.h"
#include "stats.h"
#include "gui_interface/pbar_interface.h"
#include "modules/all_modules.h"
#include "transport.h"

/* extern resources */
extern void print_total_money(void);
void setLincitySpeed( int speed );
extern void ok_dial_box(const char *, int, const char *);

/* AL1: they are all in engine.cpp */
extern void connect_rivers(void);
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

static void sustainability_test(void);
static int sust_fire_cover(void);

static void clear_game(void);
static void nullify_mappoint(int x, int y);
static void random_start(int *originx, int *originy);
static void setup_land(void);
static void coal_reserve_setup(void);
static void ore_reserve_setup(void);
static void setup_river(void);
static void setup_river2(int x, int y, int d, int alt, int mountain);
static void set_mappoint_used(int fromx, int fromy, int x, int y);
static void setup_ground(void);

#ifdef DEBUG
        static void debug_mappoints(void); /* AL1: NG 1.1.2 compiler warns that this is unused */
#endif

#define IS_RIVER(x,y) (MP_INFO(x,y).flags & FLAG_IS_RIVER)

/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
void do_time_step(void)
{
    if (flag_warning) {
        flag_warning = false;
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

void initialize_tax_rates(void)
{
    income_tax_rate = INCOME_TAX_RATE;
    coal_tax_rate = COAL_TAX_RATE;
    goods_tax_rate = GOODS_TAX_RATE;
    dole_rate = DOLE_RATE;
    transport_cost_rate = TRANSPORT_COST_RATE;
    import_cost_rate = IM_PORT_COST_RATE;
}


void init_mappoint_array(void)
{
    int x;
    for (x = 0; x < WORLD_SIDE_LEN; x++) {
        mappoint_array_x[x] = x;
        mappoint_array_y[x] = x;
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

void upgrade_to_v2 (void)
{
    // Follow order and logic of new_city
    int x,y;
    int alt0 = 0;
    int mount;
    int c;

    global_mountainity= 10 + rand () % 300;
    mount = global_mountainity;

    // Grey border (not visible on the map, x = 0 , x = 99, y = 0, y = 99) 
    for (x = 0; x < WORLD_SIDE_LEN; x++)
        for (y = 0; y < WORLD_SIDE_LEN; y++) {
            ALT(x,y) = 0;
            if ( !GROUP_IS_BARE(MP_GROUP(x, y)) ) {
                /* be nice, put water under all existing builings / farms / parks ... */
                /* This may change according to global_aridity and distance_to_river */
                MP_INFO(x,y).flags |= FLAG_HAS_UNDERGROUND_WATER;
            }
        }

    /* Let 10 years in game time to put waterwells where needed, then starvation will occur */
    deadline=total_time + 1200 * 10;
    flag_warning = true; // warn player.

    /* Upgrade ground[x][y].water_alt and .altitude */
    /* choose this order for rivers and beach scenario 
     *  = say the lowest part of the map is South-east
     *    and the sea may have a slope :-) 
     *     (strictly speaking this is true, but at much smaller order of magnitude */

    /* River mouth is on south of the map */
    y = WORLD_SIDE_LEN -2;
    for (x = WORLD_SIDE_LEN - 1; x >=0; x--)
        if ( IS_RIVER(x,y) )
            ALT(x,y) = 1;

    alt0 = 1;
    #define max(X, Y) (X>Y?X:Y)
    #define min(X, Y) (X>Y?Y:X)
    for (y = WORLD_SIDE_LEN - 2; y >= 0; y--) {
        for (x = WORLD_SIDE_LEN - 1; x >= 0; x--) {
            if ( IS_RIVER(x,y) ) {
                // attempt to find large area of water = lake or sea => slope = 0
                c = 0;
                int alt=0;
                if (x < WORLD_SIDE_LEN - 2)  {
                    if IS_RIVER(x + 1, y + 1) {
                        c++;
                        alt = max (alt, ALT(x+1, y+1));
                    }
                    if IS_RIVER(x + 1, y ) {
                        c++;
                        alt = max (alt, ALT(x+1, y));
                    }
                }
                if IS_RIVER(x , y +1 ) {
                    c++;
                    alt = max (alt, ALT(x, y+1));
                }
                if (x > 1) {
                    if IS_RIVER(x - 1, y+1 ) {
                        c++;
                        alt = max (alt, ALT(x-1, y+1));
                    }
                    if IS_RIVER(x - 1, y ) {
                        c++;
                        alt = max (alt, ALT(x-1, y));
                    }
                }
                if (c == 0) {
                    fprintf(stderr," error in upgrade_to_v2, c = 0, impossible, x = %d, y =%d\n", x, y);
                    return;
                }
                if (c <= 2) {
                    alt0 = alt + rand() % ( 2 + mount / 100 );
                    alt += rand() % ( 2 + mount / 100 );
                    ALT(x,y) = max(alt, alt0);
                    fprintf(stderr, " x %d, y %d, alt %d, alt0 %d , c=%d \n", x, y, alt, alt0, c);
                } else {
                    fprintf(stderr, " x %d, y %d, alt %d, alt0 %d\n", x, y, alt, alt0);
                    ALT(x,y) = alt;
                }
            }
        }
        alt0 ++; // minimum slope toward south
    }

    setup_land();
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
    /* GCS FIX -- seems to be a bit of engine code embedded in 
       do_monthgraph(), such as coal_made, coal_used, etc.
       Check it out soon... */
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
                do_track(x, y);
                break;
            case GROUP_RAIL:
                do_rail(x, y);
                break;
            case GROUP_ROAD:
                do_road(x, y);
                break;
            case GROUP_ORGANIC_FARM:
                do_organic_farm(x, y);
                break;
            case GROUP_MARKET:
                do_market(x, y);
                break;
            case GROUP_RESIDENCE_LL:
                do_residence(x, y);
                break;
            case GROUP_RESIDENCE_ML:
                do_residence(x, y);
                break;
            case GROUP_RESIDENCE_HL:
                do_residence(x, y);
                break;
            case GROUP_RESIDENCE_LH:
                do_residence(x, y);
                break;
            case GROUP_RESIDENCE_MH:
                do_residence(x, y);
                break;
            case GROUP_RESIDENCE_HH:
                do_residence(x, y);
                break;
            case GROUP_POWER_LINE:
                do_power_line(x, y);
                break;
            case GROUP_SOLAR_POWER:
                do_power_source(x, y);
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

static void clear_game(void)
{
    int x, y;
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
        for (x = 0; x < WORLD_SIDE_LEN; x++) {
            nullify_mappoint(x, y);
        }
    }
    total_time = 0;
    coal_survey_done = 0;
    numof_shanties = 0;
    numof_communes = 0;
    numof_substations = 0;
    numof_health_centres = 0;
    numof_markets = 0;
    max_pop_ever = 0;
    total_evacuated = 0;
    total_births = 0;
    total_money = 0;
    tech_level = 0;
    highest_tech_level = 0;
    rockets_launched = 0;
    rockets_launched_success = 0;
    init_inventory();
    update_avail_modules(0);
    /* Al1. NG 1.1 is this enough ? Are all global variables reseted ? */
    /* TODO reset screen, sustain info, max_tech when load scenario... */
    use_waterwell = true;
    ldsv_version = WATERWELL_V2;
    global_aridity = 0;
    global_mountainity =0;
}

void new_city(int *originx, int *originy, int random_village)
{
    clear_game();
    coal_reserve_setup();

    global_mountainity= 10 + rand () % 300; // roughly water slope = 30m / 1km (=from N to S)
    setup_river();
    setup_ground();
    setup_land();
    ore_reserve_setup();
    init_pbars();

    /* Initial population is 100 for empty board or 200 
       for random village (100 are housed). */
    people_pool = 100;

    if (random_village != 0) {
        random_start(originx, originy);
        update_pbar(PPOP, 200, 1);      /* So pbars don't flash */
    } else {
        *originx = *originy = WORLD_SIDE_LEN / 2;
        update_pbar(PPOP, 100, 1);
    }
    connect_transport(1, 1, WORLD_SIDE_LEN - 2, WORLD_SIDE_LEN - 2);
    refresh_pbars();
}

static void coal_reserve_setup(void)
{
    int i, j, x, y, xx, yy;
    for (i = 0; i < NUMOF_COAL_RESERVES / 5; i++) {
        x = (rand() % (WORLD_SIDE_LEN - 12)) + 6;
        y = (rand() % (WORLD_SIDE_LEN - 10)) + 6;
        do {
            xx = (rand() % 3) - 1;
            yy = (rand() % 3) - 1;
        }
        while (xx == 0 && yy == 0);
        for (j = 0; j < 5; j++) {
            MP_INFO(x, y).coal_reserve += rand() % COAL_RESERVE_SIZE;
            x += xx;
            y += yy;
        }
    }
}

static void ore_reserve_setup(void)
{
    int x, y;
    for (y = 0; y < WORLD_SIDE_LEN; y++)
        for (x = 0; x < WORLD_SIDE_LEN; x++)
            MP_INFO(x, y).ore_reserve = ORE_RESERVE;
}

void setup_river(void)
{
    int x, y, i, j;
    int alt = 1; //lowest altitude in the map = surface of the river at mouth.
    x = (1 * WORLD_SIDE_LEN + rand() % WORLD_SIDE_LEN) / 3;
    y = WORLD_SIDE_LEN - 1;
    ground[x][y].water_alt = alt; // 1 unit = 1 cm , 
                        //for rivers .water_alt = .altitude = surface of the water
                        //for "earth tile" .water_alt = alt of underground water
                        //                 .altitude = alt of the ground
                        //            so .water_alt <= .altitude

    /* Mouth of the river, 3 tiles wide, 6 + %12 long */
    i = (rand() % 12) + 6;
    for (j = 0; j < i; j++) {
        x += (rand() % 3) - 1;
        MP_TYPE(x, y) = CST_WATER;
        MP_GROUP(x, y) = GROUP_WATER;
        MP_INFO(x, y).flags |= FLAG_IS_RIVER;
        ground[x][y].altitude=alt;

        MP_TYPE(x + 1, y) = CST_WATER;
        MP_GROUP(x + 1, y) = GROUP_WATER;
        MP_INFO(x + 1, y).flags |= FLAG_IS_RIVER;
        ground[x + 1][y].altitude=alt;

        MP_TYPE(x - 1, y) = CST_WATER;
        MP_GROUP(x - 1, y) = GROUP_WATER;
        MP_INFO(x - 1, y).flags |= FLAG_IS_RIVER;
        ground[x -1][y].altitude=alt;

        y--;
        alt += 1; // wide river, so very small slope
    }

    MP_TYPE(x, y) = CST_WATER;
    MP_GROUP(x, y) = GROUP_WATER;
    MP_INFO(x, y).flags |= FLAG_IS_RIVER;
    ground[x][y].altitude=alt;

    MP_TYPE(x + 1, y) = CST_WATER;
    MP_GROUP(x + 1, y) = GROUP_WATER;
    MP_INFO(x + 1, y).flags |= FLAG_IS_RIVER;
    ground[x + 1][y].altitude=alt;

    MP_TYPE(x - 1, y) = CST_WATER;
    MP_GROUP(x - 1, y) = GROUP_WATER;
    MP_INFO(x - 1, y).flags |= FLAG_IS_RIVER;
    ground[x -1][y].altitude=alt;

    alt += 2;

#ifdef DEBUG
    fprintf(stderr," x= %d, y=%d, altitude = %d, mountainity = %d\n", x, y, alt, global_mountainity);
#endif
    setup_river2(x - 1, y, -1, alt, global_mountainity); /* left tributary */
    setup_river2(x + 1, y, 1, alt, global_mountainity);  /* right tributary */
}

void setup_river2(int x, int y, int d, int alt, int mountain)
{
    int i, j, r;
    i = (rand() % 55) + 15;
    for (j = 0; j < i; j++) {
        r = (rand() % 3) - 1 + (d * (rand() % 3));
        if (r < -1) {
            alt += rand() % (mountain / 10);
            r = -1;
        } else if (r > 1) {
            alt += rand() % (mountain / 10);
            r = 1;
        }
        x += r;
        if (!GROUP_IS_BARE(MP_GROUP(x + (d + d), y))
            || !GROUP_IS_BARE(MP_GROUP(x + (d + d + d), y)))
            return;
        if (x > 5 && x < WORLD_SIDE_LEN - 5) {
            MP_TYPE(x, y) = CST_WATER;
            MP_GROUP(x, y) = GROUP_WATER;
            MP_INFO(x, y).flags |= FLAG_IS_RIVER;
            ground[x][y].altitude = alt;
            alt += rand() % (mountain / 10);

            MP_TYPE(x + d, y) = CST_WATER;
            MP_GROUP(x + d, y) = GROUP_WATER;
            MP_INFO(x + d, y).flags |= FLAG_IS_RIVER;
            ground[x + d][y].altitude = alt;
            alt += rand () % (mountain / 10);
        }
        if (--y < 10 || x < 5 || x > WORLD_SIDE_LEN - 5)
            break;
    }
#ifdef DEBUG
    fprintf(stderr," x= %d, y=%d, altitude = %d\n", x, y, alt);
#endif

    if (y > 20) {
        if (x > 5 && x < WORLD_SIDE_LEN - 5) {
#ifdef DEBUG
            fprintf(stderr," x= %d, y=%d, altitude = %d\n", x, y, alt);
#endif
            setup_river2(x, y, -1, alt, (mountain * 3)/2 );
        }
        if (x > 5 && x < WORLD_SIDE_LEN - 5) {
#ifdef DEBUG
            fprintf(stderr," x= %d, y=%d, altitude = %d\n", x, y, alt);
#endif
            setup_river2(x, y, 1, alt, (mountain *3)/2 );
        }
    }
}

void setup_ground(void)
{
    int x,y;
    int hmax =0;
    /* fill the corrects fields: ground[x][y).stuff, global_aridity, global_mountainity */
    /* currently only dummy things in order to compile */

    /* FIXME: AL1 i did it ugly: should not use ground struct for tmp */
#define TMP(x,y) ground[x][y].int4

    for (x = 1; x < WORLD_SIDE_LEN - 1; x++) {
         for (y = 1; y < WORLD_SIDE_LEN - 1; y++) {
                if ( !IS_RIVER(x,y) ) {
                    ALT(x,y) = 0;
                    TMP(x,y) = 0;
                } else {
                    ground[x][y].water_alt = ALT(x,y);
                    //shore is higher than water
                    ALT(x,y) += 10 + rand() % (global_mountainity/7);
                    TMP(x,y) = ALT(x,y);
                    if (ALT(x,y) >= hmax)
                        hmax = ALT(x,y);
                }
         }
    }
#ifdef DEBUG
    fprintf(stderr,"\n river max = %d\n\n", hmax);
    hmax=0;
#endif

    for (int i =0; i < 90; i++ ) {
        int tot_cnt = 0;
        for (x = 1; x < WORLD_SIDE_LEN - 1; x++) {
            for (y = 1; y < WORLD_SIDE_LEN - 1; y++) {
                if ( ALT(x,y) != 0 )
                    continue;
                int count = 0;
                int lmax = 0;
                tot_cnt ++;
                for ( int k = -1; k <= 1; k++ )
                    for ( int l = -1; l <= 1; l++) 
                        if ( ALT(x+k, y+l) != 0 ) {
                            count ++;
                            if ( ALT(x+k, y+l) >= lmax )
                                lmax = ALT(x+k, y+l);
                        }

                if (count != 0)
                    TMP(x,y) = lmax + rand () % (global_mountainity/3);

                if (TMP(x,y) >= hmax)
                    hmax = TMP(x,y);
            }
        }
        for (x = 1; x < WORLD_SIDE_LEN - 1; x++)
            for (y = 1; y < WORLD_SIDE_LEN - 1; y++)
                ALT(x,y)=TMP(x,y);

#ifdef DEBUG
        if ( (i%5) == 1 )
        fprintf(stderr," i= %2d, alt max = %d, tot_cnt = %d\n", i, hmax, tot_cnt);
#endif
    }
}

void setup_land(void)
{
    int x, y, xw, yw;
    int aridity = rand() % 400 - 150;

    global_aridity = aridity;
    

    for (y = 0; y < WORLD_SIDE_LEN; y++) {
        for (x = 0; x < WORLD_SIDE_LEN; x++) {
            int d2w_min = 2 * WORLD_SIDE_LEN * WORLD_SIDE_LEN;
            int r;
            int arid = aridity;

            /* test against IS_RIVER to prevent terrible recursion */
            if (IS_RIVER(x, y) || !GROUP_IS_BARE(MP_GROUP(x, y)))
                continue;

            for (yw = 0; yw < WORLD_SIDE_LEN; yw++) {
                for (xw = 0; xw < WORLD_SIDE_LEN; xw++) {
                    int d2w;
                    if (!IS_RIVER(xw, yw))
                        continue;
                    d2w = (xw - x) * (xw - x) + (yw - y) * (yw - y);
                    if (d2w < d2w_min)
                        d2w_min = d2w;
                    /* TODO ? Store square of distance to river for each tile */
                }
            }

            /* near river lower aridity */
            if (aridity > 0) {
                if (d2w_min < 5)
                    arid = aridity / 3;
                else if (d2w_min < 17)
                    arid = (aridity * 2) / 3;
            }
            r = rand() % (d2w_min / 3 + 1) + arid;
            ground[x][y].ecotable=r;
            /* needed to setup quasi randome land. The flag is set below */
            MP_INFO(x, y).flags |= FLAG_HAS_UNDERGROUND_WATER;
            do_rand_ecology(x,y);
            MP_POL(x, y) = 0;

            /* preserve rivers, so that we can connect port later */
            if (MP_TYPE(x, y) == CST_WATER) {
                int navigable = MP_INFO(x, y).flags & FLAG_IS_RIVER;
                set_mappoint(x, y, CST_WATER);
                MP_INFO(x, y).flags |= navigable;
                MP_INFO(x, y).flags |= FLAG_HAS_UNDERGROUND_WATER;
            }
            /* set undergroung water according to first random land setup */
            if (MP_TYPE(x, y) == CST_DESERT) {
                MP_INFO(x, y).flags &= (0xffffffff - FLAG_HAS_UNDERGROUND_WATER);
            }
        }
    }
    for (y = 0; y < WORLD_SIDE_LEN; y++)
        for (x = 0; x < WORLD_SIDE_LEN; x++)
            if (MP_TYPE(x, y) == CST_WATER)
                MP_INFO(x, y).flags |= FLAG_HAS_UNDERGROUND_WATER;

    connect_rivers();
}

void do_rand_ecology(int x, int y)
{
    int r = ground[x][y].ecotable;
    if ( (MP_INFO(x, y).flags | FLAG_HAS_UNDERGROUND_WATER) == 0 ) {
        /*true desert*/
        return;
    }

    if (r >= 300) {
        /* very dry land */
        int r2 = rand() % 10;
        if (r2 <= 6)
            set_mappoint(x, y, CST_DESERT);
        else if (r2 <= 8)
            set_mappoint(x, y, CST_GREEN);
        else
            set_mappoint(x, y, CST_TREE);
    } else if (r >= 160) {
        int r2 = rand() % 10;
        if (r2 <= 2)
            set_mappoint(x, y, CST_DESERT);
        else if (r2 <= 6)
            set_mappoint(x, y, CST_GREEN);
        else
            set_mappoint(x, y, CST_TREE);
    } else if (r >= 80) {
        int r2 = rand() % 10;
        if (r2 <= 1)
            set_mappoint(x, y, CST_DESERT);
        else if (r2 <= 4)
            set_mappoint(x, y, CST_GREEN);
        else if (r2 <= 6)
            set_mappoint(x, y, CST_TREE);
        else
            set_mappoint(x, y, CST_TREE2);
    } else if (r >= 40) {
        int r2 = rand() % 40;
        if (r2 == 0)
            set_mappoint(x, y, CST_DESERT);
        else if (r2 <= 12)
            set_mappoint(x, y, CST_GREEN);
        else if (r2 <= 24)
            set_mappoint(x, y, CST_TREE);
        else if (r2 <= 36)
            set_mappoint(x, y, CST_TREE2);
        else
            set_mappoint(x, y, CST_TREE3);
    } else if (r >= 0) {
        /* normal land */
        int r2 = rand() % 40;
        if (r2 <= 10)
            set_mappoint(x, y, CST_GREEN);
        else if (r2 <= 20)
            set_mappoint(x, y, CST_TREE);
        else if (r2 <= 30)
            set_mappoint(x, y, CST_TREE2);
        else
            set_mappoint(x, y, CST_TREE3);
    } else if (r >= -40) {
        /* forest */
        int r2 = rand() % 40;
        if (r2 <= 5)
            set_mappoint(x, y, CST_GREEN);
        else if (r2 <= 10)
            set_mappoint(x, y, CST_TREE);
        else if (r2 <= 25)
            set_mappoint(x, y, CST_TREE2);
        else
            set_mappoint(x, y, CST_TREE3);
    } else if (r >= -80) {
        int r2 = rand() % 40;
        if (r2 <= 0)
            MP_TYPE(x, y) = CST_WATER;
        else if (r2 <= 6)
            set_mappoint(x, y, CST_GREEN);
        else if (r2 <= 15)
            set_mappoint(x, y, CST_TREE);
        else if (r2 <= 28)
            set_mappoint(x, y, CST_TREE2);
        else
            set_mappoint(x, y, CST_TREE3);
    } else if (r >= -120) {
        int r2 = rand() % 40;
        if (r2 <= 1)
            MP_TYPE(x, y) = CST_WATER;
        else if (r2 <= 6)
            set_mappoint(x, y, CST_GREEN);
        else if (r2 <= 16)
            set_mappoint(x, y, CST_TREE);
        else if (r2 <= 30)
            set_mappoint(x, y, CST_TREE2);
        else
            set_mappoint(x, y, CST_TREE3);
    } else {
        /* wetland */
        int r2 = rand() % 40;
        if (r2 <= 3)
            MP_TYPE(x, y) = CST_WATER;
        else if (r2 <= 8)
            set_mappoint(x, y, CST_GREEN);
        else if (r2 <= 20)
            set_mappoint(x, y, CST_TREE);
        else if (r2 <= 35)
            set_mappoint(x, y, CST_TREE2);
        else
            set_mappoint(x, y, CST_TREE3);
    }

}

static void nullify_mappoint(int x, int y)
{
    MP_TYPE(x, y) = CST_GREEN;
    MP_GROUP(x, y) = GROUP_BARE;
    MP_SIZE(x, y) = 1;
    MP_POL(x, y) = 0;
    MP_INFO(x, y).population = 0;
    MP_INFO(x, y).flags = 0;
    MP_INFO(x, y).coal_reserve = 0;
    MP_INFO(x, y).ore_reserve = 0;
    MP_INFO(x, y).int_1 = 0;
    MP_INFO(x, y).int_2 = 0;
    MP_INFO(x, y).int_3 = 0;
    MP_INFO(x, y).int_4 = 0;
    MP_INFO(x, y).int_5 = 0;
    MP_INFO(x, y).int_6 = 0;
    MP_INFO(x, y).int_7 = 0;

    ground[x][y].altitude = 0;
    ground[x][y].ecotable = 0;
    ground[x][y].wastes = 0;
    ground[x][y].pollution = 0;
    ground[x][y].water_alt = 0;
    ground[x][y].water_pol = 0;
    ground[x][y].water_wast = 0;
    ground[x][y].water_next = 0;
    ground[x][y].int1 = 0;
    ground[x][y].int2 = 0;
    ground[x][y].int3 = 0;
    ground[x][y].int4 = 0;

}

static void random_start(int *originx, int *originy)
{
    int x, y, xx, yy, flag, watchdog;

    /* first find a place that has some water. */
    watchdog = 90;              /* if too many tries, random placement. */
    do {
        do {
            xx = rand() % (WORLD_SIDE_LEN - 25);
            yy = rand() % (WORLD_SIDE_LEN - 25);
            flag = 0;
            for (y = yy + 2; y < yy + 23; y++)
                for (x = xx + 2; x < xx + 23; x++)
                    if (IS_RIVER(x, y)) {
                        flag = 1;
                        x = xx + 23;    /* break out of loop */
                        y = yy + 23;    /* break out of loop */
                    }
        } while (flag == 0 && (--watchdog) > 1);
        for (y = yy + 4; y < yy + 22; y++)
            for (x = xx + 4; x < xx + 22; x++)
                /* Don't put the village on a river, but don't care of
                 * isolated random water tiles putted by setup_land
                 */
                if (IS_RIVER(x, y)) {
                    flag = 0;
                    x = xx + 22;        /* break out of loop */
                    y = yy + 22;        /* break out of loop */
                }
    } while (flag == 0 && (--watchdog) > 1);
#ifdef DEBUG
    fprintf(stderr, "random village watchdog = %i\n", watchdog);
#endif

    /* These are going to be the main_screen_origin? vars */
    *originx = xx;
    *originy = yy;

    /*  Draw the start scene. */
    set_mappoint(xx + 5, yy + 5, CST_FARM_O0);
    /* The first two farms have more underground water */
    for (int i = 0; i < MP_SIZE(xx + 5, yy + 5); i++)
        for (int j = 0; j < MP_SIZE(xx + 5, yy + 5); j++)
            if (!HAS_UGWATER(xx + 5 + i, yy + 5 + j) && (rand() % 2))
                MP_INFO(xx + 5 + i, yy + 5 + j).flags |= FLAG_HAS_UNDERGROUND_WATER;

    set_mappoint(xx + 9, yy + 6, CST_RESIDENCE_ML);
    MP_INFO(xx + 9, yy + 6).population = 50;
    MP_INFO(xx + 9, yy + 6).flags |= (FLAG_FED + FLAG_EMPLOYED + FLAG_WATERWELL_COVER);
    set_mappoint(xx + 9, yy + 9, CST_POTTERY_0);
    set_mappoint(xx + 16, yy + 9, CST_WATERWELL);

    set_mappoint(xx + 14, yy + 6, CST_RESIDENCE_ML);
    MP_INFO(xx + 14, yy + 6).population = 50;
    MP_INFO(xx + 14, yy + 6).flags |= (FLAG_FED + FLAG_EMPLOYED + FLAG_WATERWELL_COVER);
    set_mappoint(xx + 17, yy + 5, CST_FARM_O0);
    for (int i = 0; i < MP_SIZE(xx + 17, yy + 5); i++)
        for (int j = 0; j < MP_SIZE(xx + 17, yy + 5); j++)
            if (!HAS_UGWATER(xx + 17 + i, yy + 5 + j) && (rand() % 2))
                MP_INFO(xx + 17 + i, yy + 5 + j).flags |= FLAG_HAS_UNDERGROUND_WATER;

    set_mappoint(xx + 14, yy + 9, CST_MARKET_EMPTY);
    marketx[numof_markets] = xx + 14;
    markety[numof_markets] = yy + 9;
    numof_markets++;
    /* Bootstrap markets with some stuff. */
    MP_INFO(xx + 14, yy + 9).int_1 = 2000;
    MP_INFO(xx + 14, yy + 9).int_2 = 10000;
    MP_INFO(xx + 14, yy + 9).int_3 = 100;
    MP_INFO(xx + 14, yy + 9).int_5 = 10000;
    MP_INFO(xx + 14, yy + 9).flags
        |= (FLAG_MB_FOOD + FLAG_MS_FOOD + FLAG_MB_JOBS
            + FLAG_MS_JOBS + FLAG_MB_COAL + FLAG_MS_COAL + FLAG_MB_ORE
            + FLAG_MS_ORE + FLAG_MB_GOODS + FLAG_MS_GOODS + FLAG_MB_STEEL + FLAG_MS_STEEL);

    for (x = 5; x < 19; x++) {
        set_mappoint(xx + x, yy + 11, CST_TRACK_LR);
        MP_INFO(xx + x, yy + 11).flags |= FLAG_IS_TRANSPORT;
    }
    set_mappoint(xx + 6, yy + 12, CST_COMMUNE_1);
    set_mappoint(xx + 6, yy + 17, CST_COMMUNE_1);
    set_mappoint(xx + 11, yy + 12, CST_COMMUNE_1);
    set_mappoint(xx + 11, yy + 17, CST_COMMUNE_1);
    set_mappoint(xx + 16, yy + 12, CST_COMMUNE_1);
    set_mappoint(xx + 16, yy + 17, CST_COMMUNE_1);
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

