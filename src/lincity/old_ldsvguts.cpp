/* ---------------------------------------------------------------------- *
 * old_ldsvguts.c
 * This file is part of lincity-ng.
 * ---------------------------------------------------------------------- */

/* This file is for loading old games (before NG 1.91)
 * and convert them to new format + data structure
 */

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <iostream>
#include "tinygettext/gettext.hpp"
#include "gui_interface/screen_interface.h"
#include "gui_interface/shared_globals.h"
#include "stats.h"
#include "init_game.h"
#include "all_buildings.h"
#include "modules/all_modules.h"

#include <fcntl.h>
#include <sys/types.h>

#if defined (TIME_WITH_SYS_TIME)
#include <time.h>
#include <sys/time.h>
#else
#if defined (HAVE_SYS_TIME_H)
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif

#include <cstdlib>
#include <string.h>
#include <math.h>
/*
#if defined (WIN32)
#include <winsock.h>
#include <io.h>
#include <direct.h>
#include <process.h>
#endif
*/
#ifdef __EMX__
#define chown(x,y,z)
#endif

#if defined (HAVE_DIRENT_H)
#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#define dirent direct
#define NAMLEN(dirent) (dirent)->d_namlen
#if defined (HAVE_SYS_NDIR_H)
#include <sys/ndir.h>
#endif
#if defined (HAVE_SYS_DIR_H)
#include <sys/dir.h>
#endif
#if defined (HAVE_NDIR_H)
#include <ndir.h>
#endif
#endif

#include <ctype.h>
//#include "common.h"
/*
#ifdef LC_X11
#include <X11/cursorfont.h>
#endif
*/
#include "lctypes.h"
#include "lin-city.h"
#include "engglobs.h"
#include "fileutil.h"
//#include "power.h"
#include "gui_interface/pbar_interface.h"
#include "lincity-ng/ErrorInterface.hpp"
#include "stats.h"
#include "old_ldsvguts.h"
#include "loadsave.h"
#include "simulate.h"
#include "engine.h"
#include "../lincity-ng/Config.hpp"
//#include "modules/market.h"

#if defined (WIN32) && !defined (NDEBUG)
#define START_FAST_SPEED 1
#define SKIP_OPENING_SCENE 1
#endif

#define SI_BLACK 252
#define SI_RED 253
#define SI_GREEN 254
#define SI_YELLOW 255

/* Extern resources */
extern void ok_dial_box(const char *, int, const char *);
extern void prog_box(const char *, int);

extern void print_total_money(void);
//extern int count_groups(int);
extern void reset_animation_times(void);

/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */
void upgrade_to_v2 (void);

/* ---------------------------------------------------------------------- *
 * Public functions
 * ---------------------------------------------------------------------- */

void load_city_old(char *cname)
{
    int i, x, y, n, p;
    unsigned int z;
    int num_pbars, pbar_data_size;
    int pbar_tmp;
    int dummy;
    gzFile gzfile;
    char s[256];

#ifdef DEBUG
    fprintf(stderr, "old file format, so load with old function, and then convert to new format\n");
#endif

    gzfile = gzopen(cname, "rb");
    if (gzfile == NULL) {
        printf("%s <%s> (%s)",_("Can't open"),cname, _("gzipped"));
        do_error("Can't open it!");
    }
    /* Initialise additional structure FIXME random village does not go here*/
/*
    for (x = 0; x < world.len(); x++)
        for (y = 0; y < world.len(); y++) {
            MP_TECH(x,y) = 0;
            MP_DATE(x,y) = 0;
            MP_ANIM(x,y) = 0;
        }
*/
    /* Add version to shared global variables for playing/saving games without waterwell */
    sscanf(gzgets(gzfile, s, 256), "%d", &ldsv_version);
    if (ldsv_version < MIN_LOAD_VERSION) {
        //ok_dial_box("too-old.mes", BAD, 0L);  // FIXME: AL1 screen is not set, so ok_dial_box fails
                                                //        => we have an error message, but not the good one ;-)
        fprintf(stderr, "Too old file format, cannot load it, sorry\n");
        gzclose(gzfile);
        return;
    }

    fprintf(stderr, " ldsv_version = %i \n", ldsv_version);
    use_waterwell = true;

    init_pbars();
    num_pbars = OLD_NUM_PBARS;
    pbar_data_size = PBAR_DATA_SIZE;

    init_inventory();

    prog_box(_("Loading scene"), 0);

    for (x = 0; x < world.len(); x++) {
        for (y = 0; y < world.len(); y++) {
            int dummy; //FIXME hack to pretend reading obsolete data
            for (z = 0; z < sizeof(int); z++) {
                sscanf(gzgets(gzfile, s, 256), "%d", &n);
                *(((unsigned char *)&dummy) + z) = n;
            }
            for (z = 0; z < sizeof(int); z++) {
                sscanf(gzgets(gzfile, s, 256), "%d", &n);
                *(((unsigned char *)&world(x, y)->flags) + z) = n;
            }
            for (z = 0; z < sizeof(unsigned short); z++) {
                sscanf(gzgets(gzfile, s, 256), "%d", &n);
                *(((unsigned char *)&world(x, y)->coal_reserve) + z) = n;
            }
            for (z = 0; z < sizeof(unsigned short); z++) {
                sscanf(gzgets(gzfile, s, 256), "%d", &n);
                *(((unsigned char *)&world(x, y)->ore_reserve) + z) = n;
            }
            for (z = 0; z < sizeof(int); z++) { //int1
                sscanf(gzgets(gzfile, s, 256), "%d", &n);
                *(((unsigned char *)&dummy) + z) = n;
            }
            for (z = 0; z < sizeof(int); z++) { //int2
                sscanf(gzgets(gzfile, s, 256), "%d", &n);
                *(((unsigned char *)&dummy) + z) = n;
            }
            for (z = 0; z < sizeof(int); z++) { //int3
                sscanf(gzgets(gzfile, s, 256), "%d", &n);
                *(((unsigned char *)&dummy) + z) = n;
            }
            for (z = 0; z < sizeof(int); z++) { //int4
                sscanf(gzgets(gzfile, s, 256), "%d", &n);
                *(((unsigned char *)&dummy) + z) = n;
            }
            for (z = 0; z < sizeof(int); z++) { //int5
                sscanf(gzgets(gzfile, s, 256), "%d", &n);
                *(((unsigned char *)&dummy) + z) = n;
            }
            for (z = 0; z < sizeof(int); z++) { //int6
                sscanf(gzgets(gzfile, s, 256), "%d", &n);
                *(((unsigned char *)&dummy) + z) = n;
            }
            for (z = 0; z < sizeof(int); z++) { //int7
                sscanf(gzgets(gzfile, s, 256), "%d", &n);
                *(((unsigned char *)&dummy) + z) = n;
            }
            sscanf(gzgets(gzfile, s, 256), "%d", &n);
            world(x, y)->pollution = (unsigned short)n;
            sscanf(gzgets(gzfile, s, 256), "%d", &n);
            world(x, y)->type = (short)n;

        }
        if (((93 * x) / world.len()) % 3 == 0)
            prog_box("", (93 * x) / world.len());
    }
    check_endian();
    set_map_groups();

    sscanf(gzgets(gzfile, s, 256), "%d", &main_screen_originx);
    sscanf(gzgets(gzfile, s, 256), "%d", &main_screen_originy);
    if (main_screen_originx > world.len() - getConfig()->videoX / 16 - 1)
        main_screen_originx = world.len() - getConfig()->videoX / 16 - 1;

    if (main_screen_originy > world.len() - getConfig()->videoY / 16 - 1)
        main_screen_originy = world.len() - getConfig()->videoY / 16 - 1;

    sscanf(gzgets(gzfile, s, 256), "%d", &total_time);
    if (ldsv_version <= MM_MS_C_VER)
        i = OLD_MAX_NUMOF_SUBSTATIONS;
    else
        i = MAX_NUMOF_SUBSTATIONS;
    for (x = 0; x < i; x++) {
        sscanf(gzgets(gzfile, s, 256), "%d", &dummy);//&substationx[x]);
        sscanf(gzgets(gzfile, s, 256), "%d", &dummy);//&substationy[x]);
    }
    prog_box("", 92);
    sscanf(gzgets(gzfile, s, 256), "%d", &dummy);//&numof_substations);
    if (ldsv_version <= MM_MS_C_VER)
        i = OLD_MAX_NUMOF_MARKETS;
    else
        i = MAX_NUMOF_MARKETS;
    for (x = 0; x < i; x++) {
        sscanf(gzgets(gzfile, s, 256), "%d", &dummy);//&marketx[x]);
        sscanf(gzgets(gzfile, s, 256), "%d", &dummy);//&markety[x]);
    }
    prog_box("", 94);
    sscanf(gzgets(gzfile, s, 256), "%d", &dummy);//&numof_markets);
    sscanf(gzgets(gzfile, s, 256), "%d", &people_pool);
    sscanf(gzgets(gzfile, s, 256), "%d", &total_money);
    sscanf(gzgets(gzfile, s, 256), "%d", &income_tax_rate);
    sscanf(gzgets(gzfile, s, 256), "%d", &coal_tax_rate);
    sscanf(gzgets(gzfile, s, 256), "%d", &dole_rate);
    sscanf(gzgets(gzfile, s, 256), "%d", &transport_cost_rate);
    sscanf(gzgets(gzfile, s, 256), "%d", &goods_tax_rate);
    sscanf(gzgets(gzfile, s, 256), "%d", &export_tax);
    sscanf(gzgets(gzfile, s, 256), "%d", &export_tax_rate);
    sscanf(gzgets(gzfile, s, 256), "%d", &import_cost);
    sscanf(gzgets(gzfile, s, 256), "%d", &import_cost_rate);
    sscanf(gzgets(gzfile, s, 256), "%d", &tech_level);
    if (tech_level > MODERN_WINDMILL_TECH)
        modern_windmill_flag = 1;
    sscanf(gzgets(gzfile, s, 256), "%d", &tpopulation);
    sscanf(gzgets(gzfile, s, 256), "%d", &tstarving_population);
    sscanf(gzgets(gzfile, s, 256), "%d", &tunemployed_population);
    sscanf(gzgets(gzfile, s, 256), "%d", &x);   /* waste_goods obsolete */
    sscanf(gzgets(gzfile, s, 256), "%d", &power_made);
    sscanf(gzgets(gzfile, s, 256), "%d", &power_used);
    sscanf(gzgets(gzfile, s, 256), "%d", &coal_made);
    sscanf(gzgets(gzfile, s, 256), "%d", &coal_used);
    sscanf(gzgets(gzfile, s, 256), "%d", &goods_made);
    sscanf(gzgets(gzfile, s, 256), "%d", &goods_used);
    sscanf(gzgets(gzfile, s, 256), "%d", &ore_made);
    sscanf(gzgets(gzfile, s, 256), "%d", &ore_used);
    sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &diff_old_population */

    /* Update variables calculated from those above */
    housed_population = tpopulation / NUMOF_DAYS_IN_MONTH;

    prog_box("", 96);
    /* Get size of monthgraph array */
    if (ldsv_version <= MG_C_VER) {
        i = 120;
    } else {
        sscanf(gzgets(gzfile, s, 256), "%d", &i);
    }
    for (x = 0; x < i; x++) {
        /* If more entries in file than will fit on screen,
           then we need to skip past them. */
        if (x >= monthgraph_size) {
            sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &monthgraph_pop[x] */
            sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &monthgraph_starve[x] */
            sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &monthgraph_nojobs[x] */
            sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &monthgraph_ppool[x] */
        } else {
            sscanf(gzgets(gzfile, s, 256), "%d", &monthgraph_pop[x]);
            sscanf(gzgets(gzfile, s, 256), "%d", &monthgraph_starve[x]);
            sscanf(gzgets(gzfile, s, 256), "%d", &monthgraph_nojobs[x]);
            sscanf(gzgets(gzfile, s, 256), "%d", &monthgraph_ppool[x]);
        }
        /* If our save file is old, skip past obsolete diffgraph entries */
        if (ldsv_version <= MG_C_VER) {
            sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &diffgraph_power[x] */
            sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &diffgraph_coal[x] */
            sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &diffgraph_goods[x] */
            sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &diffgraph_ore[x] */
            sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &diffgraph_population[x] */
        }
    }
    /* If screen bigger than number of entries in file, pad with zeroes */
    while (x < monthgraph_size) {
        monthgraph_pop[x] = 0;
        monthgraph_starve[x] = 0;
        monthgraph_nojobs[x] = 0;
        monthgraph_ppool[x] = 0;
        x++;
    }
    prog_box("", 98);
    sscanf(gzgets(gzfile, s, 256), "%d", &rockets_launched);
    sscanf(gzgets(gzfile, s, 256), "%d", &rockets_launched_success);
    sscanf(gzgets(gzfile, s, 256), "%d", &coal_survey_done);

    for (x = 0; x < pbar_data_size; x++) {
        for (p = 0; p < num_pbars; p++) {
            sscanf(gzgets(gzfile, s, 256), "%d", &(pbar_tmp));
            update_pbar(p, pbar_tmp, 1);
/*      sscanf( gzgets( gzfile, s, 256 ), "%d", &(pbars[p].data[x])); */
        }
    }

    for (p = 0; p < num_pbars; p++)
        pbars[p].data_size = pbar_data_size;

    prog_box("", 99);

    for (p = 0; p < num_pbars; p++) {
        sscanf(gzgets(gzfile, s, 256), "%d", &(pbars[p].oldtot));
        sscanf(gzgets(gzfile, s, 256), "%d", &(pbars[p].diff));
    }

    sscanf(gzgets(gzfile, s, 256), "%d", &cheat_flag);
    sscanf(gzgets(gzfile, s, 256), "%d", &total_pollution_deaths);
    sscanf(gzgets(gzfile, s, 256), "%f", &pollution_deaths_history);
    sscanf(gzgets(gzfile, s, 256), "%d", &total_starve_deaths);
    sscanf(gzgets(gzfile, s, 256), "%f", &starve_deaths_history);
    sscanf(gzgets(gzfile, s, 256), "%d", &total_unemployed_years);
    sscanf(gzgets(gzfile, s, 256), "%f", &unemployed_history);
    sscanf(gzgets(gzfile, s, 256), "%d", &max_pop_ever);
    sscanf(gzgets(gzfile, s, 256), "%d", &total_evacuated);
    sscanf(gzgets(gzfile, s, 256), "%d", &total_births);
    for (x = 0; x < NUMOF_MODULES; x++)
        sscanf(gzgets(gzfile, s, 256), "%d", &dummy);
    sscanf(gzgets(gzfile, s, 256), "%d", &x);   /* just dummy reads */
    sscanf(gzgets(gzfile, s, 256), "%d", &x);   /* for backwards compatibility. */

    /* 10 dummy strings, for missed out things, have been put in save. */
    /* Input from this point uses them. */
    /* XXX: WCK: Huh? Missed out things? */

    sscanf(gzgets(gzfile, s, 256), "%128s", given_scene);
    if (strncmp(given_scene, "dummy", 5) == 0 || strlen(given_scene) < 3)
        given_scene[0] = 0;
    sscanf(gzgets(gzfile, s, 256), "%128s", s);
    if (strncmp(given_scene, "dummy", 5) != 0)
        sscanf(s, "%d", &highest_tech_level);
    else
        highest_tech_level = 0;

    gzgets(gzfile, s, 200);
    if (sscanf
        (s, "sust %d %d %d %d %d %d %d %d %d %d", &sust_dig_ore_coal_count, &sust_port_count, &sust_old_money_count,
         &sust_old_population_count, &sust_old_tech_count, &sust_fire_count, &sust_old_money, &sust_old_population,
         &sust_old_tech, &sustain_flag) == 10) {
        sust_dig_ore_coal_tip_flag = sust_port_flag = 1;
        /* GCS FIX: Check after loading file if screen is drawn OK */
        /* draw_sustainable_window (); */
    } else
        sustain_flag = sust_dig_ore_coal_count = sust_port_count
            = sust_old_money_count = sust_old_population_count
            = sust_old_tech_count = sust_fire_count = sust_old_money = sust_old_population = sust_old_tech = 0;

    if (ldsv_version == WATERWELL_V2) {
        gzgets(gzfile, s, 80);
        sscanf(s, "arid %d %d", &global_aridity, &global_mountainity);
#ifdef DEBUG
        fprintf(stderr," arid %d, mountain %d \n", global_aridity, global_mountainity);
#endif
        for (x = 0; x < world.len(); x++) {
            for (y = 0; y < world.len(); y++) {
                gzgets(gzfile, s, 200);
                sscanf(s,"%d %d %d %d %d %d %d %d %d %d %d %d",&(world(x, y)->ground.altitude)
                        , &world(x, y)->ground.ecotable
                        , &world(x, y)->ground.wastes
                        , &world(x, y)->ground.pollution
                        , &world(x, y)->ground.water_alt
                        , &world(x, y)->ground.water_pol
                        , &world(x, y)->ground.water_wast
                        , &world(x, y)->ground.water_next
                        , &world(x, y)->ground.int1
                        , &world(x, y)->ground.int2
                        , &world(x, y)->ground.int3
                        , &world(x, y)->ground.int4
                        );
#ifdef DEBUG
                if (x == 10 && y == 10)
                    fprintf(stderr," alt %d, int4 %d \n", world(x, y)->ground.altitude, world(x, y)->ground.int4);
#endif
            }
        }
    }
    gzclose(gzfile);

    //numof_shanties = count_groups(GROUP_SHANTY);
    //numof_communes = count_groups(GROUP_COMMUNE);
    prog_box("", 100);

    /* set up the university intake. */
    /*
    x = Counted<University>::getInstanceCount();
    if (x > 0) {
        university_intake_rate = (Counted<School>::getInstanceCount() * 20) / x;
        if (university_intake_rate > 100)
            university_intake_rate = 100;
    } else
        university_intake_rate = 50;
    */
    //unhighlight_module_button(selected_module);
    //selected_module = sbut[7];  /* 7 is track.  Watch out though! */
    //highlight_module_button(selected_module);
    //set_selected_module(CST_TRACK_LR);

    print_total_money();
    //reset_animation_times();
    /* kind upgrade of MP_TECH for old buildings, when we don't know
     * eg light industries pollution depends on tech */
    int tk = (3 * highest_tech_level) / 4;
    if (tech_level >= tk)
        tk = tech_level;

    /* update tech dep */
    for (x = 0; x < world.len(); x++)
        for (y = 0; y < world.len(); y++) {

            switch (world(x, y)->getGroup()) {
            case (GROUP_WINDMILL):
/*
                MP_TECH(x,y) = MP_INFO(x, y).int_2;
                MP_INFO(x, y).int_1 = (int)(WINDMILL_POWER + (((double)MP_TECH(x, y) * WINDMILL_POWER)
                                                              / MAX_TECH_LEVEL));
*/
                break;

            case (GROUP_COAL_POWER):
/*
                MP_TECH(x,y) = MP_INFO(x,y).int_4;
                MP_INFO(x, y).int_1 = (int)(POWERS_COAL_OUTPUT + (((double)MP_TECH(x, y) * POWERS_COAL_OUTPUT)
                                                                  / MAX_TECH_LEVEL));
*/
                break;

            case (GROUP_SOLAR_POWER):
                {
/*
                    float PSO = POWERS_SOLAR_OUTPUT;
                    float MT = MAX_TECH_LEVEL;
                    int t1, t2 , t3;
                        t1 = (int) (((MP_INFO(x,y).int_1 - PSO) * MT)/PSO);
                        t2 = MP_INFO(x,y).int_2;
                        t3 = (int) (((MP_INFO(x,y).int_3 - PSO) * MT)/PSO);
                    //because of bug introduced then fixed near 1207 1218 or in waterwell branch
                    if (MP_INFO(x,y).int_2 != 0) {
                        MP_TECH(x,y) = t2;
                    } else if ( MP_INFO(x,y).int_3 >=0 ) {
                        MP_TECH(x,y) = t3;
                    } else if (MP_INFO(x,y).int_1 != 0) {
                        MP_TECH(x,y) = t1;
                    } else {
                        fprintf(stderr,"    Error, unknown tech level for solar plant at x=%d, y=%d\n", x, y);
                        if (tk > GROUP_SOLAR_POWER_TECH)
                            MP_TECH(x,y) = tk;
                        else
                            MP_TECH(x,y) = GROUP_SOLAR_POWER_TECH;
                    }
                    //fprintf(stderr,"TECH %d, t1 %d, t2 %d, t3 %d, plant at x=%d, y=%d\n", MP_TECH(x,y), t1, t2, t3, x, y);
                    MP_INFO(x, y).int_1 = (int)(POWERS_SOLAR_OUTPUT + (((double)MP_TECH(x, y) * POWERS_SOLAR_OUTPUT)
                                / MAX_TECH_LEVEL));
*/
                    break;
                }
            case GROUP_ORGANIC_FARM:
//              MP_TECH(x,y) = MP_INFO(x,y).int_1;
                break;

            case GROUP_RECYCLE:
//              MP_TECH(x,y) = MP_INFO(x, y).int_4;
                break;

            case GROUP_INDUSTRY_L:
/*
                if ( MP_TECH(x,y) == 0 ){
                    if ( tk > GROUP_INDUSTRY_L_TECH ){
                        MP_TECH(x,y) = tk;
                    } else {
                        MP_TECH(x,y) = GROUP_INDUSTRY_L_TECH;
                    }
                }
*/
                break;
            }
        }

    //map_power_grid(true);
                                /* WCK:  Is this safe to do here?
                                 * AL1: No, in NG_1.1
                                 * In case of error message with ok_dial_box
                                 *    the dialog cannot appear because the screen
                                 *    is not set up => crash.
                                 * FIXME: move all initialisation elsewhere, in
                                 *    engine.cpp or simulate.cpp.
                                 */
    upgrade_to_v2();

}
/*
void reset_animation_times(void)
{
    int x, y;

    for (y = 0; y < world.len(); y++)
        for (x = 0; x < world.len(); x++) {
            MP_ANIM(x,y) = 0;
            if (map == GROUP_FIRE)
                MP_INFO(x, y).int_3 = 0;
        }
}
*/
void check_endian(void)
{
    static int flag = 0;
    char *cs;
    int t, x, y;
    t = 0;
    cs = (char *)&t;
    *cs = 1;
    if (t == 1)                 /* little endian */
        return;
    if (flag == 0) {
        flag = 1;
    }
    for (y = 0; y < world.len(); y++) {
        for (x = 0; x < world.len(); x++) {
            //eswap32(&(map.info[x][y].population));
            eswap32(&(world(x, y)->flags));
            if (sizeof(short) == 2) {
                eswap16(&(world(x, y)->coal_reserve));
                eswap16(&(world(x, y)->ore_reserve));
            } else if (sizeof(short) == 4) {
                eswap32((int *)&(world(x, y)->coal_reserve));
                eswap32((int *)&(world(x, y)->ore_reserve));
            } else {
                /* prevent gcc warning on amd64: argument 2 has type 'long unsigned int' !!! */
                printf("Strange size (%d) for short, please mail me.\n", (int) sizeof(short));
            }
/*
            eswap32(&(MP_INFO(x, y).int_1));
            eswap32(&(MP_INFO(x, y).int_2));
            eswap32(&(MP_INFO(x, y).int_3));
            eswap32(&(MP_INFO(x, y).int_4));
            eswap32(&(MP_INFO(x, y).int_5));
            eswap32(&(MP_INFO(x, y).int_6));
            eswap32(&(MP_INFO(x, y).int_7));
*/
        }
    }
}

void eswap32(int *i)
{
    char *cs, c1, c2, c3, c4;
    cs = (char *)i;
    c1 = *cs;
    c2 = *(cs + 1);
    c3 = *(cs + 2);
    c4 = *(cs + 3);
    *(cs++) = c4;
    *(cs++) = c3;
    *(cs++) = c2;
    *cs = c1;
}

void eswap16(unsigned short *i)
{
    char *cs, c1, c2;
    cs = (char *)i;
    c1 = *cs;
    c2 = *(cs + 1);
    *(cs++) = c2;
    *cs = c1;
}

void upgrade_to_v2 (void)
{
    // Follow order and logic of new_city
    int x,y;

    global_mountainity= 10 + rand () % 300;

    // Grey border (not visible on the map, x = 0 , x = 99, y = 0, y = 99)
    for (x = 0; x < world.len(); x++)
        for (y = 0; y < world.len(); y++) {
            world(x, y)->ground.altitude = 0;
            if ( !world(x, y)->is_bare()) {
                /* be nice, put water under all existing builings / farms / parks ... */
                /* This may change according to global_aridity and distance_to_river */
                world(x, y)->flags |= FLAG_HAS_UNDERGROUND_WATER;
            }
        }

    /* Let 10 years in game time to put waterwells where needed, then starvation will occur */
    deadline=total_time + 1200 * 10;
    flag_warning = true; // warn player.

    setup_land();
}

/** @file lincity/old_ldsvguts.cpp */

