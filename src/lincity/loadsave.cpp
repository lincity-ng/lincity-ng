/* ---------------------------------------------------------------------- *
 * ldsvguts.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

/* this is for saving */

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <iostream>
#include "tinygettext/gettext.hpp"
#include "gui_interface/screen_interface.h"
#include "gui_interface/shared_globals.h"
#include "stats.h"

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
#include "power.h"
#include "gui_interface/pbar_interface.h"
#include "lincity-ng/ErrorInterface.hpp"
#include "stats.h"
#include "ldsvguts.h"

#if defined (WIN32) && !defined (NDEBUG)
#define START_FAST_SPEED 1
#define SKIP_OPENING_SCENE 1
#endif

#define SI_BLACK 252
#define SI_RED 253
#define SI_GREEN 254
#define SI_YELLOW 255

#define MP_SANITY_CHECK 1

/* Extern resources */
extern int yn_dial_box(const char *, const char *, const char *, const char *);
extern void ok_dial_box(const char *, int, const char *);
extern void prog_box(const char *, int);

extern void print_total_money(void);
extern int count_groups(int);
extern void upgrade_to_v2(void);

/* ---------------------------------------------------------------------- *
 * Public functions
 * ---------------------------------------------------------------------- */
void save_city_2(char *cname)
{
    int x, y, p;
    int dumbint = 0;
    gzFile ofile = gzopen(cname, "wb");
    if (ofile == NULL) {
        printf(_("Save file <%s> - "), cname);
        do_error(_("Can't open save file!"));
    }
    /* Now we have upgraded game */
    ldsv_version = WATERWELL_V2;
    gzprintf(ofile, "%d\n", ldsv_version);

    for (x = 0; x < WORLD_SIDE_LEN; x++)
        for (y = 0; y < WORLD_SIDE_LEN; y++) {
            /*               TY po fl cr or i1 i2 i3 i4 i5 i6 i7 PL al ec ws gp wa wp ww wn g1 g2 g3 g4 DA TK AN d4 d5 d6 d7 d8 d9 */
            gzprintf(ofile, "%u %d %d %u %u %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n" 
                       , MP_TYPE(x, y)
                       , MP_INFO(x, y).population
                       , MP_INFO(x, y).flags
                       , MP_INFO(x, y).coal_reserve
                       , MP_INFO(x, y).ore_reserve
                       , MP_INFO(x, y).int_1
                       , MP_INFO(x, y).int_2
                       , MP_INFO(x, y).int_3
                       , MP_INFO(x, y).int_4
                       , MP_INFO(x, y).int_5
                       , MP_INFO(x, y).int_6
                       , MP_INFO(x, y).int_7
                       , MP_POL(x, y)
                       , ground[x][y].altitude
                       , ground[x][y].ecotable
                       , ground[x][y].wastes
                       , ground[x][y].pollution
                       , ground[x][y].water_alt
                       , ground[x][y].water_pol
                       , ground[x][y].water_wast
                       , ground[x][y].water_next
                       , ground[x][y].int1
                       , ground[x][y].int2
                       , ground[x][y].int3
                       , ground[x][y].int4
                       , MP_DATE(x,y)   // d1 = date of built
                       , MP_TECH(x,y)   // d2 = tech at build time
                       , MP_ANIM(x,y)   // d3 = animation_time (see reset_animation_time mess :)
                       , dumbint        // d4  could be         image index for smooth animation, cf windmill anim_tile
                       , dumbint        // d5                   percentage of activity to choose family of pic
                       , dumbint        // d6
                       , dumbint        // d7
                       , dumbint        // d8
                       , dumbint        // d9
                       );
        }

    gzprintf(ofile, "%d\n", main_screen_originx);
    gzprintf(ofile, "%d\n", main_screen_originy);

    gzprintf(ofile, "%d\n", total_time);
    for (x = 0; x < MAX_NUMOF_SUBSTATIONS; x++) {
        gzprintf(ofile, "%d\n", substationx[x]);
        gzprintf(ofile, "%d\n", substationy[x]);
    }
    gzprintf(ofile, "%d\n", numof_substations);
    for (x = 0; x < MAX_NUMOF_MARKETS; x++) {
        gzprintf(ofile, "%d\n", marketx[x]);
        gzprintf(ofile, "%d\n", markety[x]);
    }
    gzprintf(ofile, "%d\n", numof_markets);
    gzprintf(ofile, "%d\n", people_pool);
    gzprintf(ofile, "%o\n", total_money);
    gzprintf(ofile, "%d\n", income_tax_rate);
    gzprintf(ofile, "%d\n", coal_tax_rate);
    gzprintf(ofile, "%d\n", dole_rate);
    gzprintf(ofile, "%d\n", transport_cost_rate);
    gzprintf(ofile, "%d\n", goods_tax_rate);
    gzprintf(ofile, "%d\n", export_tax);
    gzprintf(ofile, "%d\n", export_tax_rate);
    gzprintf(ofile, "%d\n", import_cost);
    gzprintf(ofile, "%d\n", import_cost_rate);
    gzprintf(ofile, "%d\n", tech_level);
    gzprintf(ofile, "%d\n", tpopulation);
    gzprintf(ofile, "%d\n", tstarving_population);
    gzprintf(ofile, "%d\n", tunemployed_population);
    gzprintf(ofile, "%d\n", 0); /* waste_goods is obsolete */
    gzprintf(ofile, "%d\n", power_made);
    gzprintf(ofile, "%d\n", power_used);
    gzprintf(ofile, "%d\n", coal_made);
    gzprintf(ofile, "%d\n", coal_used);
    gzprintf(ofile, "%d\n", goods_made);
    gzprintf(ofile, "%d\n", goods_used);
    gzprintf(ofile, "%d\n", ore_made);
    gzprintf(ofile, "%d\n", ore_used);
    gzprintf(ofile, "%d\n", 0); /* Removed diff_old_population, version 1.12 */

    /* Changed, version 1.12 */
    gzprintf(ofile, "%d\n", monthgraph_size);
    for (x = 0; x < monthgraph_size; x++) {
        gzprintf(ofile, "%d\n", monthgraph_pop[x]);
        gzprintf(ofile, "%d\n", monthgraph_starve[x]);
        gzprintf(ofile, "%d\n", monthgraph_nojobs[x]);
        gzprintf(ofile, "%d\n", monthgraph_ppool[x]);
    }
    gzprintf(ofile, "%d\n", rockets_launched);
    gzprintf(ofile, "%d\n", rockets_launched_success);
    gzprintf(ofile, "%d\n", coal_survey_done);

    for (x = 0; x < PBAR_DATA_SIZE; x++)
        for (p = 0; p < NUM_PBARS; p++)
            gzprintf(ofile, "%d\n", pbars[p].data[x]);

    for (p = 0; p < NUM_PBARS; p++) {
        gzprintf(ofile, "%d\n", pbars[p].oldtot);
        gzprintf(ofile, "%d\n", pbars[p].diff);
    }

    gzprintf(ofile, "%d\n", cheat_flag);
    gzprintf(ofile, "%d\n", total_pollution_deaths);
    gzprintf(ofile, "%f\n", pollution_deaths_history);
    gzprintf(ofile, "%d\n", total_starve_deaths);
    gzprintf(ofile, "%f\n", starve_deaths_history);
    gzprintf(ofile, "%d\n", total_unemployed_years);
    gzprintf(ofile, "%f\n", unemployed_history);
    gzprintf(ofile, "%d\n", max_pop_ever);
    gzprintf(ofile, "%d\n", total_evacuated);
    gzprintf(ofile, "%d\n", total_births);

    for (x = 0; x < NUMOF_MODULES; x++)
        gzprintf(ofile, "%d\n", module_help_flag[x]);
    gzprintf(ofile, "%d\n", 0); /* dummy values */

    gzprintf(ofile, "%d\n", 0); /* backward compatibility */

    if (strlen(given_scene) > 1)
        gzprintf(ofile, "%s\n", given_scene);
    else
        gzprintf(ofile, "dummy\n");     /* 1 */

    gzprintf(ofile, "%d\n", highest_tech_level);        /* 2 */

    gzprintf(ofile, "sust %d %d %d %d %d %d %d %d %d %d\n", sust_dig_ore_coal_count, sust_port_count, sust_old_money_count, sust_old_population_count, sust_old_tech_count, sust_fire_count, sust_old_money, sust_old_population, sust_old_tech, sustain_flag); /* 3 */

    gzprintf(ofile, "arid %d %d\n", global_aridity, global_mountainity); /* 4 */

    gzprintf(ofile, "dummy\n"); /* 5 */

    gzprintf(ofile, "dummy\n"); /* 6 */

    gzprintf(ofile, "dummy\n"); /* 7 */

    gzprintf(ofile, "dummy\n"); /* 8 */

    gzprintf(ofile, "dummy\n"); /* 9 */

    gzprintf(ofile, "dummy\n"); /* 10 */

    gzclose(ofile);
}

void load_city_2(char *cname)
{
    int i, x, y, p;
    int dumbint = 0;
    int num_pbars, pbar_data_size;
    int pbar_tmp;
    int dummy;
    gzFile gzfile;
    char s[512];

    gzfile = gzopen(cname, "rb");
    if (gzfile == NULL) {
        printf(_("Can't open <%s> (gzipped)"), cname);
        do_error("Can't open it!");
    }

    sscanf(gzgets(gzfile, s, 256), "%d", &ldsv_version);
    if (ldsv_version < WATERWELL_V2) {
        ok_dial_box("too-old.mes", BAD, 0L); //FIXME: AL1 i guess this will crash as screen is not set */
        gzclose(gzfile);
        return;
    }

    fprintf(stderr, " ldsv_version = %i \n", ldsv_version);
    use_waterwell = true;

    init_pbars();
    num_pbars = NUM_PBARS;
    pbar_data_size = PBAR_DATA_SIZE;

    init_inventory();
    print_time_for_year();

    // Easier debugging from saved game: #Line = 100 x + y + 1  (first line = ldsv_version)
    for (x = 0; x < WORLD_SIDE_LEN; x++) {
        for (y = 0; y < WORLD_SIDE_LEN; y++) {
            gzgets(gzfile, s, 512);
            //         TY po fl cr or i1 i2 i3 i4 i5 i6 i7 PL al ec ws gp wa wp ww wn g1 g2 g3 g4 DA TK AN d4 d5 d6 d7 d8 d9
            sscanf(s, "%u %d %d %u %u %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"
                    , &MP_TYPE(x, y)
                    , &MP_INFO(x, y).population
                    , &MP_INFO(x, y).flags
                    , &MP_INFO(x, y).coal_reserve
                    , &MP_INFO(x, y).ore_reserve
                    , &MP_INFO(x, y).int_1
                    , &MP_INFO(x, y).int_2
                    , &MP_INFO(x, y).int_3
                    , &MP_INFO(x, y).int_4
                    , &MP_INFO(x, y).int_5
                    , &MP_INFO(x, y).int_6
                    , &MP_INFO(x, y).int_7
                    , &MP_POL(x, y)
                    , &ground[x][y].altitude
                    , &ground[x][y].ecotable
                    , &ground[x][y].wastes
                    , &ground[x][y].pollution
                    , &ground[x][y].water_alt
                    , &ground[x][y].water_pol
                    , &ground[x][y].water_wast
                    , &ground[x][y].water_next
                    , &ground[x][y].int1
                    , &ground[x][y].int2
                    , &ground[x][y].int3
                    , &ground[x][y].int4
                    , &MP_DATE(x,y)   // d1 = date of built
                    , &MP_TECH(x,y)   // d2 = tech at build time
                    , &MP_ANIM(x,y)   // d3 = animation_time (see reset_animation_time mess :)
                    , &dumbint        // d4  could be         image index for smooth animation, cf windmill anim_tile
                    , &dumbint        // d5                   percentage of activity to choose family of pic
                    , &dumbint        // d6
                    , &dumbint        // d7
                    , &dumbint        // d8
                    , &dumbint        // d9
                    );
            if (get_group_of_type(MP_TYPE(x, y)) == GROUP_MARKET)
                inventory(x, y);
        }
    }
    set_map_groups();

    sscanf(gzgets(gzfile, s, 256), "%d", &main_screen_originx);
    sscanf(gzgets(gzfile, s, 256), "%d", &main_screen_originy);

    sscanf(gzgets(gzfile, s, 256), "%d", &total_time);
    if (ldsv_version <= MM_MS_C_VER)
        i = OLD_MAX_NUMOF_SUBSTATIONS;
    else
        i = MAX_NUMOF_SUBSTATIONS;
    for (x = 0; x < i; x++) {
        sscanf(gzgets(gzfile, s, 256), "%d", &substationx[x]);
        sscanf(gzgets(gzfile, s, 256), "%d", &substationy[x]);
    }
    sscanf(gzgets(gzfile, s, 256), "%d", &numof_substations);
    if (ldsv_version <= MM_MS_C_VER)
        i = OLD_MAX_NUMOF_MARKETS;
    else
        i = MAX_NUMOF_MARKETS;
    for (x = 0; x < i; x++) {
        sscanf(gzgets(gzfile, s, 256), "%d", &marketx[x]);
        sscanf(gzgets(gzfile, s, 256), "%d", &markety[x]);
    }
    sscanf(gzgets(gzfile, s, 256), "%d", &numof_markets);
    sscanf(gzgets(gzfile, s, 256), "%d", &people_pool);
    sscanf(gzgets(gzfile, s, 256), "%o", &total_money);
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
    sscanf(gzgets(gzfile, s, 256), "%d", &rockets_launched);
    sscanf(gzgets(gzfile, s, 256), "%d", &rockets_launched_success);
    sscanf(gzgets(gzfile, s, 256), "%d", &coal_survey_done);

    for (x = 0; x < pbar_data_size; x++) {
        for (p = 0; p < num_pbars; p++) {
            sscanf(gzgets(gzfile, s, 256), "%d", &(pbar_tmp));
            update_pbar(p, pbar_tmp, 1);
/*	    sscanf( gzgets( gzfile, s, 256 ), "%d", &(pbars[p].data[x])); */
        }
    }

    for (p = 0; p < num_pbars; p++)
        pbars[p].data_size = pbar_data_size;

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
        sscanf(gzgets(gzfile, s, 256), "%d", &(module_help_flag[x]));
    sscanf(gzgets(gzfile, s, 256), "%d", &x);   /* just dummy reads */
    sscanf(gzgets(gzfile, s, 256), "%d", &x);   /* for backwards compatibility. */

    sscanf(gzgets(gzfile, s, 256), "%128s", given_scene);
    if (strncmp(given_scene, "dummy", 5) == 0 || strlen(given_scene) < 3)
        given_scene[0] = 0;
    sscanf(gzgets(gzfile, s, 256), "%128s", s);
    if (strncmp(given_scene, "dummy", 5) != 0)
        sscanf(s, "%d", &highest_tech_level);
    else
        highest_tech_level = 0;

    gzgets(gzfile, s, 80);   
    if (sscanf
        (s, "sust %d %d %d %d %d %d %d %d %d %d", &sust_dig_ore_coal_count, &sust_port_count, &sust_old_money_count,
         &sust_old_population_count, &sust_old_tech_count, &sust_fire_count, &sust_old_money, &sust_old_population,
         &sust_old_tech, &sustain_flag) == 10) {
        sust_dig_ore_coal_tip_flag = sust_port_flag = 1;
    } else
        sustain_flag = sust_dig_ore_coal_count = sust_port_count
            = sust_old_money_count = sust_old_population_count
            = sust_old_tech_count = sust_fire_count = sust_old_money = sust_old_population = sust_old_tech = 0;

    gzgets(gzfile, s, 80);
    sscanf(s, "arid %d %d", &global_aridity, &global_mountainity);

    gzclose(gzfile);

    /* FIXME: AL1 this is initialisation stuff, should go elsewhere */

    // Engine stuff
    if (tech_level > MODERN_WINDMILL_TECH)
        modern_windmill_flag = 1;

    numof_shanties = count_groups(GROUP_SHANTY);
    numof_communes = count_groups(GROUP_COMMUNE);

    /* set up the university intake. */
    x = count_groups(GROUP_UNIVERSITY);
    if (x > 0) {
        university_intake_rate = (count_groups(GROUP_SCHOOL) * 20) / x;
        if (university_intake_rate > 100)
            university_intake_rate = 100;
    } else
        university_intake_rate = 50;

    print_total_money();

    //reset_animation_times
    for ( y = 0; y < WORLD_SIDE_LEN; y++)
        for ( x = 0; x < WORLD_SIDE_LEN; x++) {
            MP_ANIM(x,y) = 0;
            if (MP_GROUP(x, y) == GROUP_FIRE)
                MP_INFO(x, y).int_3 = 0;
        }

    map_power_grid(true);       /* WCK:  Is this safe to do here?
                                 * AL1: No, in NG_1.1
                                 * In case of error message with ok_dial_box
                                 *    the dialog cannot appear because the screen
                                 *    is not set up => crash.
                                 * FIXME: move all initialisation elsewhere, in 
                                 *    engine.cpp or simulate.cpp.
                                 */
    // UI stuff
    if (main_screen_originx > WORLD_SIDE_LEN - getMainWindowWidth() / 16 - 1)
        main_screen_originx = WORLD_SIDE_LEN - getMainWindowWidth() / 16 - 1;

    if (main_screen_originy > WORLD_SIDE_LEN - getMainWindowHeight() / 16 - 1)
        main_screen_originy = WORLD_SIDE_LEN - getMainWindowHeight() / 16 - 1;

    unhighlight_module_button(selected_module);
    selected_module = sbut[7];  /* 7 is track.  Watch out though! */
    highlight_module_button(selected_module);
    set_selected_module(CST_TRACK_LR);
}

