/* ---------------------------------------------------------------------- *
 * loadsave.cpp
 * This file is part of lincity-NG
 * See COPYING for license, and CREDITS for authors
 * ---------------------------------------------------------------------- */

/* this is for saving */

#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
//#include <zlib.h> //moved to lintypes.h
#include <iostream>
#include "tinygettext/gettext.hpp"
#include "gui_interface/screen_interface.h"
#include "gui_interface/shared_globals.h"
#include "stats.h"
#include "init_game.h"
#include "transport.h"
#include "modules/all_modules.h"
#include "../lincity-ng/Config.hpp"

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
#include "modules/all_modules.h"
#include "loadsave.h"
#include "xmlloadsave.h"


#if defined (WIN32) && !defined (NDEBUG)
#define START_FAST_SPEED 1
#define SKIP_OPENING_SCENE 1
#endif

#define SI_BLACK 252
#define SI_RED 253
#define SI_GREEN 254
#define SI_YELLOW 255

#define MP_SANITY_CHECK 1
#define COMPATIBLE_WORLD_SIDE_LEN 100

/* Extern resources */
extern int yn_dial_box(const char *, const char *, const char *, const char *);
extern void ok_dial_box(const char *, int, const char *);
extern void prog_box(const char *, int);

extern void print_total_money(void);
//extern int count_groups(int);
extern void desert_frontier(int originx, int originy, int w, int h);
extern void set_river_tile( int x, int y);


/* -----------------------------------------------*
 * Private Functions
 * -----------------------------------------------*/


/* ---------------------------------------------------------------------- *
 * Public functions
 * ---------------------------------------------------------------------- */

void save_city(char *cname)
{
    char *s;
    int l;

    if ((l = strlen(cname)) < 2)
        return;
    if ((s = (char *)malloc(lc_save_dir_len + l + 16)) == 0)
        malloc_failure();

    sprintf(s, "%s%c%s", lc_save_dir, PATH_SLASH, cname);

    save_city_2(s);
    free(s);
}

void save_city_2(char *cname)
{
    size_t found;

    std::string xml_file_name;
    xml_file_name = cname;
    found = xml_file_name.find(".gz");

    if (found > xml_file_name.length()-3)
    {   xml_file_name += ".gz";}
    else
    {   strcpy(cname,(xml_file_name.substr(0,xml_file_name.length()-3)).c_str());}
    xml_loadsave.saveXMLfile(xml_file_name);
#ifdef DEBUG
    //TODO abandon support for writing old style savegame
    if (world.len() == COMPATIBLE_WORLD_SIDE_LEN)
    {
        int dumbint = 0;
        gzFile ofile = gzopen(cname, "wb");
        if (ofile == NULL) {
            printf("%s <%s> - ", _("Save file"), cname);
            do_error(_("Can't open save file!"));
        }
        /* Now we have upgraded game */
        ldsv_version = WATERWELL_V2;
        gzprintf(ofile, "%d\n", ldsv_version);

        for (int x = 0; x < world.len(); x++)
        {
            for (int y = 0; y < world.len(); y++)
            {
                /*               TY po fl cr or i1 i2 i3 i4 i5 i6 i7 PL al ec ws gp wa wp ww wn g1 g2 g3 g4 DA TK AN d4 d5 d6 d7 d8 d9 */
                gzprintf(ofile, "%u %d %d %u %u %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n"
                           , world(x, y)->is_visible() ? world(x, y)->getTopType() : CST_USED
                           , 0 //MP_INFO(x, y).population
                           , world(x, y)->flags
                           , world(x, y)->coal_reserve
                           , world(x, y)->ore_reserve
                           , 0 //MP_INFO(x, y).int_1
                           , 0 //MP_INFO(x, y).int_2
                           , 0 //MP_INFO(x, y).int_3
                           , 0 //MP_INFO(x, y).int_4
                           , 0 //MP_INFO(x, y).int_5
                           , 0 //MP_INFO(x, y).int_6
                           , 0 //MP_INFO(x, y).int_7
                           , world(x, y)->pollution
                           , world(x, y)->ground.altitude
                           , world(x, y)->ground.ecotable
                           , world(x, y)->ground.wastes
                           , world(x, y)->ground.pollution
                           , world(x, y)->ground.water_alt
                           , world(x, y)->ground.water_pol
                           , world(x, y)->ground.water_wast
                           , world(x, y)->ground.water_next
                           , world(x, y)->ground.int1
                           , world(x, y)->ground.int2
                           , world(x, y)->ground.int3
                           , world(x, y)->ground.int4
                           , 0 //MP_DATE(x,y)   // d1 = date of built
                           , 0 //MP_TECH(x,y)   // d2 = tech at build time
                           , 0 //MP_ANIM(x,y)   // d3 = animation_time (see reset_animation_time mess :)
                           , dumbint        // d4  could be         image index for smooth animation, cf windmill anim_tile
                           , dumbint        // d5                   percentage of activity to choose family of pic
                           , dumbint        // d6
                           , dumbint        // d7
                           , dumbint        // d8
                           , dumbint        // d9
                           );
            }//end for y
        }//end for x
        gzprintf(ofile, "%d\n", main_screen_originx);
        gzprintf(ofile, "%d\n", main_screen_originy);

        gzprintf(ofile, "%d\n", total_time);
        for (int x = 0; x < MAX_NUMOF_SUBSTATIONS; x++) {
            gzprintf(ofile, "%d\n", 0);//substationx[x]
            gzprintf(ofile, "%d\n", 0);//substationy[x]
        }
        gzprintf(ofile, "%d\n", 0);//numof_substations);
        for (int x = 0; x < MAX_NUMOF_MARKETS; x++) {
            gzprintf(ofile, "%d\n", 0);//marketx[x]
            gzprintf(ofile, "%d\n", 0);//markety[x]
        }
        gzprintf(ofile, "%d\n", 0);//numof_markets);
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
        gzprintf(ofile, "%d\n", 0); // power_made);
        gzprintf(ofile, "%d\n", 0); // power_used);
        gzprintf(ofile, "%d\n", 0); // coal_made);
        gzprintf(ofile, "%d\n", 0); // coal_used);
        gzprintf(ofile, "%d\n", 0); // goods_made);
        gzprintf(ofile, "%d\n", 0); // goods_used);
        gzprintf(ofile, "%d\n", 0); // ore_made);
        gzprintf(ofile, "%d\n", 0); // ore_used);
        gzprintf(ofile, "%d\n", 0); /* Removed diff_old_population, version 1.12 */

        /* Changed, version 1.12 */
        gzprintf(ofile, "%d\n", monthgraph_size);
        for (int x = 0; x < monthgraph_size; x++) {
            gzprintf(ofile, "%d\n", monthgraph_pop[x]);
            gzprintf(ofile, "%d\n", monthgraph_starve[x]);
            gzprintf(ofile, "%d\n", monthgraph_nojobs[x]);
            gzprintf(ofile, "%d\n", monthgraph_ppool[x]);
        }
        gzprintf(ofile, "%d\n", rockets_launched);
        gzprintf(ofile, "%d\n", rockets_launched_success);
        gzprintf(ofile, "%d\n", coal_survey_done);

        for (int x = 0; x < PBAR_DATA_SIZE; x++)
            for (int p = 0; p < NUM_PBARS; p++)
                gzprintf(ofile, "%d\n", pbars[p].data[x]);

        for (int p = 0; p < NUM_PBARS; p++) {
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

        for (int x = 0; x < NUMOF_MODULES; x++)
            gzprintf(ofile, "%d\n", 0);

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
        std::cout << "old saved " << cname << std::endl;
    }
#endif
}

void load_city_2(char *cname)
{
    int i, x, y, p, r;
    int dumbint = 0;
    int num_pbars, pbar_data_size;
    int pbar_tmp;
    int dummy;
    gzFile gzfile;
    char s[512];
    unsigned found;
    clear_game();

    std::string xml_file_name;
    xml_file_name = cname;
    found = xml_file_name.find(".gz");

    if (found > xml_file_name.length() - 3)
    {
        xml_file_name += ".gz";
    }
    else
    {
        strcpy(cname,(xml_file_name.substr(0,xml_file_name.length()-3)).c_str());
    }

    init_pbars();
    num_pbars = OLD_NUM_PBARS;
    pbar_data_size = PBAR_DATA_SIZE;
    init_inventory();
    r = xml_loadsave.loadXMLfile(xml_file_name);
    if (r == -1)
    {
        //old savegames are always WORLD_SIDE_LEN == 100
        world.len(COMPATIBLE_WORLD_SIDE_LEN);
        clear_game();
        binary_mode = false; //set save default for old files
        seed_compression = false; //set save default for old files
        gzfile = gzopen(cname, "rb");
        if (gzfile == NULL)
        {
            printf("%s <%s> (%s)",_("Can't open"),cname, _("gzipped"));
            do_error("Can't open it!");
        }

        sscanf(gzgets(gzfile, s, 256), "%d", &ldsv_version);
        if (ldsv_version < WATERWELL_V2)
        {
            gzclose(gzfile);
            load_city_old( cname );
            /* Fix desert frontier for old saved games and scenarios */
            desert_frontier(0, 0, world.len(), world.len());
            return;
        }

        fprintf(stderr, " ldsv_version = %i \n", ldsv_version);
        use_waterwell = true;


        // Easier debugging from saved game: #Line = 100 x + y + 1  (first line = ldsv_version)
        for (x = 0; x < COMPATIBLE_WORLD_SIDE_LEN; x++)
        {
            for (y = 0; y < COMPATIBLE_WORLD_SIDE_LEN; y++)
            {
                gzgets(gzfile, s, 512);
                //         TY  po fl cr  or  i1 i2 i3 i4 i5 i6 i7 PL al ec ws gp wa wp ww wn g1 g2 g3 g4 DA TK AN d4 d5 d6 d7 d8 d9
                sscanf(s, "%hu %d %i %hu %hu %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"
                        , &(world(x, y)->type)
                        , &dummy //&MP_INFO(x, y).population
                        , &(world(x, y)->flags)
                        , &(world(x, y)->coal_reserve)
                        , &(world(x, y)->ore_reserve)
                        , &dummy //&MP_INFO(x, y).int_1
                        , &dummy //&MP_INFO(x, y).int_2
                        , &dummy //&MP_INFO(x, y).int_3
                        , &dummy //&MP_INFO(x, y).int_4
                        , &dummy //&MP_INFO(x, y).int_5
                        , &dummy //&MP_INFO(x, y).int_6
                        , &dummy //&MP_INFO(x, y).int_7
                        , &(world(x, y)->pollution)
                        , &world(x, y)->ground.altitude
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
                        , &dummy//&MP_DATE(x,y)   // d1 = date of built
                        , &dummy//&MP_TECH(x,y)   // d2 = tech at build time
                        , &dummy//&MP_ANIM(x,y)   // d3 = animation_time (see reset_animation_time mess :)
                        , &dumbint        // d4  could be         image index for smooth animation, cf windmill anim_tile
                        , &dumbint        // d5                   percentage of activity to choose family of pic
                        , &dumbint        // d6
                        , &dumbint        // d7
                        , &dumbint        // d8
                        , &dumbint        // d9
                        );
                world(x, y)->group = get_group_of_type(world(x, y)->type);

            }
        }
        //set_map_groups();

        sscanf(gzgets(gzfile, s, 256), "%d", &main_screen_originx);
        sscanf(gzgets(gzfile, s, 256), "%d", &main_screen_originy);

        sscanf(gzgets(gzfile, s, 256), "%d", &total_time);

        for (x = 0; x < MAX_NUMOF_SUBSTATIONS; x++)
        {
            sscanf(gzgets(gzfile, s, 256), "%d", &dummy);//&substationx[x]);
            sscanf(gzgets(gzfile, s, 256), "%d", &dummy);//&substationy[x]);
        }
        sscanf(gzgets(gzfile, s, 256), "%d", &dummy);//&numof_substations);

        for (x = 0; x < MAX_NUMOF_MARKETS; x++)
        {
            sscanf(gzgets(gzfile, s, 256), "%d", &dummy);//&marketx[x]);
            sscanf(gzgets(gzfile, s, 256), "%d", &dummy);//&markety[x]);
        }
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
        sscanf(gzgets(gzfile, s, 256), "%d", &tpopulation);
        sscanf(gzgets(gzfile, s, 256), "%d", &tstarving_population);
        sscanf(gzgets(gzfile, s, 256), "%d", &tunemployed_population);
        sscanf(gzgets(gzfile, s, 256), "%d", &dummy);   /* waste_goods obsolete */
        sscanf(gzgets(gzfile, s, 256), "%d", &power_made);
        sscanf(gzgets(gzfile, s, 256), "%d", &power_used);
        sscanf(gzgets(gzfile, s, 256), "%d", &coal_made);
        sscanf(gzgets(gzfile, s, 256), "%d", &coal_used);
        sscanf(gzgets(gzfile, s, 256), "%d", &goods_made);
        sscanf(gzgets(gzfile, s, 256), "%d", &goods_used);
        sscanf(gzgets(gzfile, s, 256), "%d", &ore_made);
        sscanf(gzgets(gzfile, s, 256), "%d", &ore_used);
        sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &diff_old_population */

        //build new constructions according to type info
        int constuctionCounter = 0;
        for (x = 0; x < COMPATIBLE_WORLD_SIDE_LEN; x++)
        {
            for (y = 0; y < COMPATIBLE_WORLD_SIDE_LEN; y++)
            {
                unsigned short type = world(x, y)->type;
                unsigned short group = world(x,y)->group;
                world(x,y)->flags &= ~VOLATILE_FLAGS;
                if (world(x, y)->flags & FLAG_IS_RIVER)
                    {   set_river_tile( x, y);}

                if((group == GROUP_TRACK_BRIDGE) || (group == GROUP_ROAD_BRIDGE)
                || (group == GROUP_RAIL_BRIDGE))
                {   world(x, y)->setTerrain(CST_WATER);}

                if (ConstructionGroup::countConstructionGroup(group))
                {
                    ConstructionGroup::getConstructionGroup(group)->placeItem(x, y);
                    if(world(x,y)->is_residence())
                    {
                        Residence *residence = dynamic_cast<Residence*>(world(x,y)->construction);
                        residence->local_population = 95 * residence->max_population / 100;
                    }
                    world(x,y)->construction->bootstrap_commodities(50);
                    constuctionCounter++;
                }
                else if(type == CST_USED)
                {
                    world(x, y)->setTerrain(CST_DESERT);
                }//endif countConstruction
            }// end for y
        }// end for x
        std::cout<<"Generated and initialzed "<<constuctionCounter<<" modern Constructions"<<std::endl;
        //}



        /* Get size of monthgraph array */
        sscanf(gzgets(gzfile, s, 256), "%d", &i);
        for (x = 0; x < i; x++)
        {
            /* If more entries in file than will fit on screen,
               then we need to skip past them. */
            if (x >= monthgraph_size)
            {
                sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &monthgraph_pop[x] */
                sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &monthgraph_starve[x] */
                sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &monthgraph_nojobs[x] */
                sscanf(gzgets(gzfile, s, 256), "%d", &dummy);       /* &monthgraph_ppool[x] */
            }
            else
            {
                sscanf(gzgets(gzfile, s, 256), "%d", &monthgraph_pop[x]);
                sscanf(gzgets(gzfile, s, 256), "%d", &monthgraph_starve[x]);
                sscanf(gzgets(gzfile, s, 256), "%d", &monthgraph_nojobs[x]);
                sscanf(gzgets(gzfile, s, 256), "%d", &monthgraph_ppool[x]);
            }
        }
        /* If screen bigger than number of entries in file, pad with zeroes */
        while (x < monthgraph_size)
        {
            monthgraph_pop[x] = 0;
            monthgraph_starve[x] = 0;
            monthgraph_nojobs[x] = 0;
            monthgraph_ppool[x] = 0;
            x++;
        }
        sscanf(gzgets(gzfile, s, 256), "%d", &rockets_launched);
        sscanf(gzgets(gzfile, s, 256), "%d", &rockets_launched_success);
        sscanf(gzgets(gzfile, s, 256), "%d", &coal_survey_done);

        for (x = 0; x < pbar_data_size; x++)
        {
            for (p = 0; p < num_pbars; p++)
            {
                sscanf(gzgets(gzfile, s, 256), "%d", &(pbar_tmp));
                update_pbar(p, pbar_tmp, 1);
            }
        }

        for (p = 0; p < num_pbars; p++)
        {   pbars[p].data_size = pbar_data_size;}

        for (p = 0; p < num_pbars; p++)
        {
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
        {   sscanf(gzgets(gzfile, s, 256), "%d", &dummy);}

        sscanf(gzgets(gzfile, s, 256), "%128s", given_scene);
        if (strncmp(given_scene, "dummy", 5) == 0 || strlen(given_scene) < 3)
        {   given_scene[0] = 0;}
        sscanf(gzgets(gzfile, s, 256), "%128s", s);
        if (strncmp(given_scene, "dummy", 5) != 0)
        {   sscanf(s, "%d", &highest_tech_level);}
        else
        {   highest_tech_level = 0;}

        gzgets(gzfile, s, 200);
        if (sscanf
            (s, "sust %d %d %d %d %d %d %d %d %d %d", &sust_dig_ore_coal_count, &sust_port_count, &sust_old_money_count,
             &sust_old_population_count, &sust_old_tech_count, &sust_fire_count, &sust_old_money, &sust_old_population,
             &sust_old_tech, &sustain_flag) == 10)
        {
            sust_dig_ore_coal_tip_flag = sust_port_flag = 1;
        }
        else
        {    sustain_flag = sust_dig_ore_coal_count = sust_port_count
                = sust_old_money_count = sust_old_population_count
                = sust_old_tech_count = sust_fire_count = sust_old_money = sust_old_population = sust_old_tech = 0;
        }

        gzgets(gzfile, s, 80);
        sscanf(s, "arid %d %d", &global_aridity, &global_mountainity);

        gzclose(gzfile);
        std::cout << "old loaded " << cname << std::endl;
    }
    /* Update variables calculated from those above */
    housed_population = tpopulation / ((total_time % NUMOF_DAYS_IN_MONTH) + 1);

    /* FIXME: AL1 this is initialisation stuff, should go elsewhere */

    // Engine stuff
    if (tech_level > MODERN_WINDMILL_TECH)
    {   modern_windmill_flag = 1;}

    //numof_shanties = count_groups(GROUP_SHANTY);
    //numof_communes = count_groups(GROUP_COMMUNE);

    /* set up the university intake. */
/*
    x = Counted<University>::getInstanceCount();
    if (x > 0)
    {
        university_intake_rate = (Counted<School>::getInstanceCount() * 20) / x;
        if (university_intake_rate > 100)
            university_intake_rate = 100;
    } else
        university_intake_rate = 50;
*/
    print_total_money();

    //reset_animation_times
    //get alt_min, alt_max
    alt_min = 2000000000;
    alt_max = -alt_min;

    for ( y = 0; y < world.len(); y++){
        for ( x = 0; x < world.len(); x++) {
            if (alt_min > world(x, y)->ground.altitude)
            {
                 alt_min = world(x, y)->ground.altitude;
            }
            if (alt_max < world(x, y)->ground.altitude)
            {
                 alt_max = world(x, y)->ground.altitude;
            }
        }
    }

    alt_step = (alt_max - alt_min) /10;

    //map_power_grid(true);
                                /* WCK:  Is this safe to do here?
                                 * AL1: No, in NG_1.1
                                 * In case of error message with ok_dial_box
                                 *    the dialog cannot appear because the screen
                                 *    is not set up => crash.
                                 * FIXME: move all initialisation elsewhere, in
                                 *    engine.cpp or simulate.cpp.
                                 */
    // UI stuff
    if (main_screen_originx > COMPATIBLE_WORLD_SIDE_LEN - getConfig()->videoX / 16 - 1)
        main_screen_originx = COMPATIBLE_WORLD_SIDE_LEN - getConfig()->videoX / 16 - 1;

    if (main_screen_originy > COMPATIBLE_WORLD_SIDE_LEN - getConfig()->videoY / 16 - 1)
        main_screen_originy = COMPATIBLE_WORLD_SIDE_LEN - getConfig()->videoY / 16 - 1;

    //unhighlight_module_button(selected_module);
    //selected_module = sbut[7];  /* 7 is track.  Watch out though! */
    //highlight_module_button(selected_module);
    //set_selected_module(CST_TRACK_LR);
    connect_transport(1, 1, world.len() - 2, world.len() - 2);
    /* Fix desert frontier for old saved games and scenarios */
    desert_frontier(0, 0, world.len(), world.len());

}


/** @file lincity/loadsave.cpp */

