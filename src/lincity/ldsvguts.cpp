/* ---------------------------------------------------------------------- *
 * ldsvguts.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

/* this is for saving */

//#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <iostream>
//#include "lcstring.h"
#include "lcintl.h"
#include "gui_interface/screen_interface.h"
#include "gui_interface/shared_globals.h"
//#include "cliglobs.h"
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
//#include "cliglobs.h"
#include "engglobs.h"
#include "fileutil.h"
#include "power.h"
#include "gui_interface/pbar_interface.h"
#include "stats.h"

#if defined (WIN32) && !defined (NDEBUG)
#define START_FAST_SPEED 1
#define SKIP_OPENING_SCENE 1
#endif

#define SI_BLACK 252
#define SI_RED 253
#define SI_GREEN 254
#define SI_YELLOW 255

#define MP_SANITY_CHECK 1

/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */
void dump_screen (void);
int verify_city (char *cname);

/* ---------------------------------------------------------------------- *
 * Private Global Variables
 * ---------------------------------------------------------------------- */

char save_names[10][42];

/* ---------------------------------------------------------------------- *
 * Public functions
 * ---------------------------------------------------------------------- */
void
remove_scene (char *cname)
{
    char *s;
    int l;
    if ((l = strlen (cname)) < 2)	/* Thanks to Chris J. Kiick */
	return;

    if ((s = (char *) malloc (lc_save_dir_len + l + 16)) == 0)
	malloc_failure ();
    sprintf (s, "%s%c%s", lc_save_dir, PATH_SLASH, cname);
    remove (s);
    free (s);
}

void
save_city_raw (char *cname)
{
    int x, y, z, q, n, p;
    gzFile ofile = gzopen(cname, "wb");
    if (ofile == NULL) {
	printf (_("Save file <%s> - "), cname);
	do_error (_("Can't open save file!"));
    }

    gzprintf (ofile, "%d\n", (int) VERSION_INT);
    q = sizeof (Map_Point_Info);
    prog_box (_("Saving scene"), 0);
    check_endian ();
    for (x = 0; x < WORLD_SIDE_LEN; x++) {
	for (y = 0; y < WORLD_SIDE_LEN; y++) {
	    for (z = 0; z < sizeof(int); z++) {
		n = *(((unsigned char *) &MP_INFO(x,y).population) + z);
		gzprintf (ofile, "%d\n", n);
	    }
	    for (z = 0; z < sizeof(int); z++) {
		n = *(((unsigned char *) &MP_INFO(x,y).flags) + z);
		gzprintf (ofile, "%d\n", n);
	    }
	    for (z = 0; z < sizeof(unsigned short); z++) {
		n = *(((unsigned char *) &MP_INFO(x,y).coal_reserve) + z);
		gzprintf (ofile, "%d\n", n);
	    }
	    for (z = 0; z < sizeof(unsigned short); z++) {
		n = *(((unsigned char *) &MP_INFO(x,y).ore_reserve) + z);
		gzprintf (ofile, "%d\n", n);
	    }
	    for (z = 0; z < sizeof(int); z++) {
		n = *(((unsigned char *) &MP_INFO(x,y).int_1) + z);
		gzprintf (ofile, "%d\n", n);
	    }
	    for (z = 0; z < sizeof(int); z++) {
		n = *(((unsigned char *) &MP_INFO(x,y).int_2) + z);
		gzprintf (ofile, "%d\n", n);
	    }
	    for (z = 0; z < sizeof(int); z++) {
		n = *(((unsigned char *) &MP_INFO(x,y).int_3) + z);
		gzprintf (ofile, "%d\n", n);
	    }
	    for (z = 0; z < sizeof(int); z++) {
		n = *(((unsigned char *) &MP_INFO(x,y).int_4) + z);
		gzprintf (ofile, "%d\n", n);
	    }
	    for (z = 0; z < sizeof(int); z++) {
		n = *(((unsigned char *) &MP_INFO(x,y).int_5) + z);
		gzprintf (ofile, "%d\n", n);
	    }
	    for (z = 0; z < sizeof(int); z++) {
		n = *(((unsigned char *) &MP_INFO(x,y).int_6) + z);
		gzprintf (ofile, "%d\n", n);
	    }
	    for (z = 0; z < sizeof(int); z++) {
		n = *(((unsigned char *) &MP_INFO(x,y).int_7) + z);
		gzprintf (ofile, "%d\n", n);
	    }
	    gzprintf (ofile, "%d\n", (int) MP_POL(x,y));
	    gzprintf (ofile, "%d\n", (int) MP_TYPE(x,y));
	}
	prog_box ("", (90 * x) / WORLD_SIDE_LEN);
    }
    check_endian ();		/* we have to put the byte order back. */

    gzprintf (ofile, "%d\n", main_screen_originx);
    gzprintf (ofile, "%d\n", main_screen_originy);
    gzprintf (ofile, "%d\n", total_time);
    for (x = 0; x < MAX_NUMOF_SUBSTATIONS; x++)
    {
	gzprintf (ofile, "%d\n", substationx[x]);
	gzprintf (ofile, "%d\n", substationy[x]);
    }
    prog_box ("", 92);
    gzprintf (ofile, "%d\n", numof_substations);
    for (x = 0; x < MAX_NUMOF_MARKETS; x++)
    {
	gzprintf (ofile, "%d\n", marketx[x]);
	gzprintf (ofile, "%d\n", markety[x]);
    }
    prog_box ("", 94);
    gzprintf (ofile, "%d\n", numof_markets);
    gzprintf (ofile, "%d\n", people_pool);
    gzprintf (ofile, "%d\n", total_money);
    gzprintf (ofile, "%d\n", income_tax_rate);
    gzprintf (ofile, "%d\n", coal_tax_rate);
    gzprintf (ofile, "%d\n", dole_rate);
    gzprintf (ofile, "%d\n", transport_cost_rate);
    gzprintf (ofile, "%d\n", goods_tax_rate);
    gzprintf (ofile, "%d\n", export_tax);
    gzprintf (ofile, "%d\n", export_tax_rate);
    gzprintf (ofile, "%d\n", import_cost);
    gzprintf (ofile, "%d\n", import_cost_rate);
    gzprintf (ofile, "%d\n", tech_level);
    gzprintf (ofile, "%d\n", tpopulation);
    gzprintf (ofile, "%d\n", tstarving_population);
    gzprintf (ofile, "%d\n", tunemployed_population);
    gzprintf (ofile, "%d\n", 0); /* waste_goods is obsolete */
    gzprintf (ofile, "%d\n", power_made);
    gzprintf (ofile, "%d\n", power_used);
    gzprintf (ofile, "%d\n", coal_made);
    gzprintf (ofile, "%d\n", coal_used);
    gzprintf (ofile, "%d\n", goods_made);
    gzprintf (ofile, "%d\n", goods_used);
    gzprintf (ofile, "%d\n", ore_made);
    gzprintf (ofile, "%d\n", ore_used);
    gzprintf (ofile, "%d\n", 0); /* Removed diff_old_population, version 1.12 */

    prog_box ("", 96);
    /* Changed, version 1.12 */
    gzprintf (ofile, "%d\n", monthgraph_size);
    for (x = 0; x < monthgraph_size; x++) {
	gzprintf (ofile, "%d\n", monthgraph_pop[x]);
	gzprintf (ofile, "%d\n", monthgraph_starve[x]);
	gzprintf (ofile, "%d\n", monthgraph_nojobs[x]);
	gzprintf (ofile, "%d\n", monthgraph_ppool[x]);
    }
    prog_box ("", 98);
    gzprintf (ofile, "%d\n", rockets_launched);
    gzprintf (ofile, "%d\n", rockets_launched_success);
    gzprintf (ofile, "%d\n", coal_survey_done);
    for (x = 0; x < PBAR_DATA_SIZE; x++)
	for (p = 0; p < NUM_PBARS; p++)
	    gzprintf(ofile, "%d\n", pbars[p].data[x]);

    prog_box ("", 99);

    for (p = 0; p < NUM_PBARS; p++) {
	gzprintf(ofile, "%d\n", pbars[p].oldtot);
	gzprintf(ofile, "%d\n", pbars[p].diff);
    }

    gzprintf (ofile, "%d\n", cheat_flag);
    gzprintf (ofile, "%d\n", total_pollution_deaths);
    gzprintf (ofile, "%f\n", pollution_deaths_history);
    gzprintf (ofile, "%d\n", total_starve_deaths);
    gzprintf (ofile, "%f\n", starve_deaths_history);
    gzprintf (ofile, "%d\n", total_unemployed_years);
    gzprintf (ofile, "%f\n", unemployed_history);
    gzprintf (ofile, "%d\n", max_pop_ever);
    gzprintf (ofile, "%d\n", total_evacuated);
    gzprintf (ofile, "%d\n", total_births);
    for (x = 0; x < NUMOF_MODULES; x++)
	gzprintf (ofile, "%d\n", module_help_flag[x]);
    gzprintf (ofile, "%d\n", 0);	/* dummy values */

    gzprintf (ofile, "%d\n", 0);	/* backward compatibility */

    if (strlen (given_scene) > 1)
	gzprintf (ofile, "%s\n", given_scene);
    else
	gzprintf (ofile, "dummy\n");	/* 1 */

    gzprintf (ofile, "%d\n", highest_tech_level);	/* 2 */

    gzprintf (ofile, "sust %d %d %d %d %d %d %d %d %d %d\n"
	     ,sust_dig_ore_coal_count, sust_port_count
	     ,sust_old_money_count, sust_old_population_count
	     ,sust_old_tech_count, sust_fire_count
	     ,sust_old_money, sust_old_population, sust_old_tech
	     ,sustain_flag);	/* 3 */

    gzprintf (ofile, "dummy\n");	/* 4 */

    gzprintf (ofile, "dummy\n");	/* 5 */

    gzprintf (ofile, "dummy\n");	/* 6 */

    gzprintf (ofile, "dummy\n");	/* 7 */

    gzprintf (ofile, "dummy\n");	/* 8 */

    gzprintf (ofile, "dummy\n");	/* 9 */

    gzprintf (ofile, "dummy\n");	/* 10 */

    gzclose (ofile);
    prog_box ("", 100);
}


void
save_city (char *cname)
{
    char *s;
    int l;

    if ((l = strlen (cname)) < 2)
	return;
    if ((s = (char *) malloc (lc_save_dir_len + l + 16)) == 0)
	malloc_failure ();

    sprintf (s, "%s%c%s", lc_save_dir, PATH_SLASH, cname);

    save_city_raw (s);
    free (s);
}

void
load_city (char *cname)
{
    unsigned long q;
    int i, x, y, z, n, p, ver;
    int num_pbars, pbar_data_size;
    int pbar_tmp;
    int dummy;
    gzFile gzfile;
    char s[256];
    gzfile = gzopen( cname, "rb" ); 
    if ( gzfile == NULL) {
	    printf (_("Can't open <%s> (gzipped)"), cname);
	    do_error ("Can't open it!");
    }
    sscanf( gzgets( gzfile, s, 256 ), "%d", &ver);
    if (ver < MIN_LOAD_VERSION) {
	    ok_dial_box ("too-old.mes", BAD, 0L);
	    gzclose( gzfile );
	return;
    }

    init_pbars();
    num_pbars = NUM_PBARS;
    pbar_data_size = PBAR_DATA_SIZE;

    init_inventory();
    
    print_time_for_year();
    q = (unsigned long) sizeof (Map_Point_Info);
    prog_box (_("Loading scene"), 0);

    for (x = 0; x < WORLD_SIDE_LEN; x++) {
	for (y = 0; y < WORLD_SIDE_LEN; y++) {
	    for (z = 0; z < sizeof(int); z++) {
		sscanf( gzgets( gzfile, s, 256 ), "%d", &n);
		*(((unsigned char *) &MP_INFO(x,y).population) + z) = n;
	    }
	    for (z = 0; z < sizeof(int); z++) {
		sscanf( gzgets( gzfile, s, 256 ), "%d", &n);
		*(((unsigned char *) &MP_INFO(x,y).flags) + z) = n;
	    }
	    for (z = 0; z < sizeof(unsigned short); z++) {
		sscanf( gzgets( gzfile, s, 256 ), "%d", &n);
		*(((unsigned char *) &MP_INFO(x,y).coal_reserve) + z) = n;
	    }
	    for (z = 0; z < sizeof(unsigned short); z++) {
		sscanf( gzgets( gzfile, s, 256 ), "%d", &n);
		*(((unsigned char *) &MP_INFO(x,y).ore_reserve) + z) = n;
	    }
	    for (z = 0; z < sizeof(int); z++) {
		sscanf( gzgets( gzfile, s, 256 ), "%d", &n);
		*(((unsigned char *) &MP_INFO(x,y).int_1) + z) = n;
	    }
	    for (z = 0; z < sizeof(int); z++) {
		sscanf( gzgets( gzfile, s, 256 ), "%d", &n);
		*(((unsigned char *) &MP_INFO(x,y).int_2) + z) = n;
	    }
	    for (z = 0; z < sizeof(int); z++) {
		sscanf( gzgets( gzfile, s, 256 ), "%d", &n);
		*(((unsigned char *) &MP_INFO(x,y).int_3) + z) = n;
	    }
	    for (z = 0; z < sizeof(int); z++) {
		sscanf( gzgets( gzfile, s, 256 ), "%d", &n);
		*(((unsigned char *) &MP_INFO(x,y).int_4) + z) = n;
	    }
	    for (z = 0; z < sizeof(int); z++) {
		sscanf( gzgets( gzfile, s, 256 ), "%d", &n);
		*(((unsigned char *) &MP_INFO(x,y).int_5) + z) = n;
	    }
	    for (z = 0; z < sizeof(int); z++) {
		sscanf( gzgets( gzfile, s, 256 ), "%d", &n);
		*(((unsigned char *) &MP_INFO(x,y).int_6) + z) = n;
	    }
	    for (z = 0; z < sizeof(int); z++) {
		sscanf( gzgets( gzfile, s, 256 ), "%d", &n);
		*(((unsigned char *) &MP_INFO(x,y).int_7) + z) = n;
	    }
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &n);
	    MP_POL(x,y) = (unsigned short) n;
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &n);
	    MP_TYPE(x,y) = (short) n;

	    if (get_group_of_type(MP_TYPE(x,y)) == GROUP_MARKET)
		inventory(x,y);
	}
	if (((93 * x) / WORLD_SIDE_LEN) % 3 == 0)
	    prog_box ("", (93 * x) / WORLD_SIDE_LEN);
    }
    check_endian ();
    set_map_groups ();

    sscanf( gzgets( gzfile, s, 256 ), "%d", &main_screen_originx);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &main_screen_originy);
    if (main_screen_originx > WORLD_SIDE_LEN - getMainWindowWidth() / 16 - 1)
	main_screen_originx = WORLD_SIDE_LEN - getMainWindowWidth() / 16 - 1;

    if (main_screen_originy > WORLD_SIDE_LEN - getMainWindowHeight() / 16 - 1)
	main_screen_originy = WORLD_SIDE_LEN - getMainWindowHeight() / 16 - 1;

    sscanf( gzgets( gzfile, s, 256 ), "%d", &total_time);
    if (ver <= MM_MS_C_VER)
	i = OLD_MAX_NUMOF_SUBSTATIONS;
    else
	i = MAX_NUMOF_SUBSTATIONS;
    for (x = 0; x < i; x++)
    {
	sscanf( gzgets( gzfile, s, 256 ), "%d", &substationx[x]);
	sscanf( gzgets( gzfile, s, 256 ), "%d", &substationy[x]);
    }
    prog_box ("", 92);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &numof_substations);
    if (ver <= MM_MS_C_VER)
	i = OLD_MAX_NUMOF_MARKETS;
    else
	i = MAX_NUMOF_MARKETS;
    for (x = 0; x < i; x++)
    {
	sscanf( gzgets( gzfile, s, 256 ), "%d", &marketx[x]);
	sscanf( gzgets( gzfile, s, 256 ), "%d", &markety[x]);
    }
    prog_box ("", 94);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &numof_markets);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &people_pool);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &total_money);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &income_tax_rate);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &coal_tax_rate);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &dole_rate);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &transport_cost_rate);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &goods_tax_rate);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &export_tax);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &export_tax_rate);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &import_cost);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &import_cost_rate);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &tech_level);
    if (tech_level > MODERN_WINDMILL_TECH)
	modern_windmill_flag = 1;
    sscanf( gzgets( gzfile, s, 256 ), "%d", &tpopulation);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &tstarving_population);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &tunemployed_population);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &x);  /* waste_goods obsolete */
    sscanf( gzgets( gzfile, s, 256 ), "%d", &power_made);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &power_used);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &coal_made);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &coal_used);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &goods_made);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &goods_used);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &ore_made);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &ore_used);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &dummy); /* &diff_old_population */

    /* Update variables calculated from those above */
    housed_population = tpopulation / NUMOF_DAYS_IN_MONTH;

    prog_box ("", 96);
    /* Get size of monthgraph array */
    if (ver <= MG_C_VER) {
	i = 120;
    } else {
	sscanf( gzgets( gzfile, s, 256 ), "%d", &i);
    }
    for (x = 0; x < i; x++) {
	/* If more entries in file than will fit on screen, 
	   then we need to skip past them. */
	if (x >= monthgraph_size) {
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &dummy); /* &monthgraph_pop[x] */
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &dummy); /* &monthgraph_starve[x] */
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &dummy); /* &monthgraph_nojobs[x] */
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &dummy); /* &monthgraph_ppool[x] */
	} else {
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &monthgraph_pop[x]);
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &monthgraph_starve[x]);
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &monthgraph_nojobs[x]);
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &monthgraph_ppool[x]);
	}
	/* If our save file is old, skip past obsolete diffgraph entries */
	if (ver <= MG_C_VER) {
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &dummy); /* &diffgraph_power[x] */
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &dummy); /* &diffgraph_coal[x] */
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &dummy); /* &diffgraph_goods[x] */
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &dummy); /* &diffgraph_ore[x] */
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &dummy); /* &diffgraph_population[x] */
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
    prog_box ("", 98);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &rockets_launched);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &rockets_launched_success);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &coal_survey_done);
    
    for (x = 0; x < pbar_data_size; x++) {
	for (p = 0; p < num_pbars; p++) {
	    sscanf( gzgets( gzfile, s, 256 ), "%d", &(pbar_tmp));
	    update_pbar(p,pbar_tmp,1);
/*	    sscanf( gzgets( gzfile, s, 256 ), "%d", &(pbars[p].data[x])); */
	}
    }

    for (p = 0; p < num_pbars; p++)
	pbars[p].data_size = pbar_data_size;

    prog_box ("", 99);

    for (p = 0; p < num_pbars; p++) {
	sscanf( gzgets( gzfile, s, 256 ), "%d", &(pbars[p].oldtot));
	sscanf( gzgets( gzfile, s, 256 ), "%d", &(pbars[p].diff));
    }


    sscanf( gzgets( gzfile, s, 256 ), "%d", &cheat_flag);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &total_pollution_deaths);
    sscanf( gzgets( gzfile, s, 256 ), "%f", &pollution_deaths_history);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &total_starve_deaths);
    sscanf( gzgets( gzfile, s, 256 ), "%f", &starve_deaths_history);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &total_unemployed_years);
    sscanf( gzgets( gzfile, s, 256 ), "%f", &unemployed_history);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &max_pop_ever);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &total_evacuated);
    sscanf( gzgets( gzfile, s, 256 ), "%d", &total_births);
    for (x = 0; x < NUMOF_MODULES; x++)
	sscanf( gzgets( gzfile, s, 256 ), "%d", &(module_help_flag[x]));
    sscanf( gzgets( gzfile, s, 256 ), "%d", &x);	/* just dummy reads */
    sscanf( gzgets( gzfile, s, 256 ), "%d", &x);	/* for backwards compatibility. */

    /* 10 dummy strings, for missed out things, have been put in save. */
    /* Input from this point uses them. */
    /* XXX: WCK: Huh? Missed out things? */

    sscanf( gzgets( gzfile, s, 256 ), "%128s", given_scene);
    if (strncmp (given_scene, "dummy", 5) == 0 || strlen (given_scene) < 3)
	given_scene[0] = 0;
    sscanf( gzgets( gzfile, s, 256 ), "%128s", s);
    if (strncmp (given_scene, "dummy", 5) != 0)
	sscanf (s, "%d", &highest_tech_level);
    else
	highest_tech_level = 0;
    gzgets( gzfile, s, 80 );		/* this is the CR */

    if (sscanf (s, "sust %d %d %d %d %d %d %d %d %d %d"
		,&sust_dig_ore_coal_count, &sust_port_count
		,&sust_old_money_count, &sust_old_population_count
		,&sust_old_tech_count, &sust_fire_count
		,&sust_old_money, &sust_old_population, &sust_old_tech
		,&sustain_flag) == 10)
    {
	sust_dig_ore_coal_tip_flag = sust_port_flag = 1;
	/* GCS FIX: Check after loading file if screen is drawn OK */
	/* draw_sustainable_window (); */
    }
    else
	sustain_flag = sust_dig_ore_coal_count = sust_port_count
		= sust_old_money_count = sust_old_population_count
		= sust_old_tech_count = sust_fire_count
		= sust_old_money = sust_old_population = sust_old_tech = 0;
    gzclose( gzfile );

    numof_shanties = count_groups (GROUP_SHANTY);
    numof_communes = count_groups (GROUP_COMMUNE);
    prog_box ("", 100);

    /* set up the university intake. */
    x = count_groups (GROUP_UNIVERSITY);
    if (x > 0) {
	university_intake_rate
		= (count_groups (GROUP_SCHOOL) * 20) / x;
	if (university_intake_rate > 100)
	    university_intake_rate = 100;
    }
    else
	university_intake_rate = 50;

    /*Al1 : What is this ? This does nothing, the value are saved in MP_INFO.int_ 
     * for (x = 0; x < WORLD_SIDE_LEN; x++)
     *   for (y = 0; y < WORLD_SIDE_LEN; y++)
     *     update_tech_dep (x, y);
     */

    unhighlight_module_button (selected_module);
    selected_module = sbut[7];	/* 7 is track.  Watch out though! */
    highlight_module_button (selected_module);
    set_selected_module (CST_TRACK_LR);

    print_total_money ();
    reset_animation_times ();
    /* update tech dep for compatibility with old games */
    for (x = 0; x < WORLD_SIDE_LEN; x++)
	for (y = 0; y < WORLD_SIDE_LEN; y++) {
            switch (MP_GROUP(x,y)) {
            case (GROUP_WINDMILL): 	 
                MP_INFO(x,y).int_1 = (int)(WINDMILL_POWER 	 
                        + (((double) MP_INFO(x,y).int_2 * WINDMILL_POWER)
                                / MAX_TECH_LEVEL));
                break;
            case (GROUP_COAL_POWER): 	 
                MP_INFO(x,y).int_1 = (int)(POWERS_COAL_OUTPUT 	 
                        + (((double) MP_INFO(x,y).int_4 * POWERS_COAL_OUTPUT) 
                                / MAX_TECH_LEVEL)); 	 
                break; 	 
            case (GROUP_SOLAR_POWER): 	 
                MP_INFO(x,y).int_1 = (int)(POWERS_SOLAR_OUTPUT 	 
                        + (((double) MP_INFO(x,y).int_2 * POWERS_SOLAR_OUTPUT) 	 
                                / MAX_TECH_LEVEL)); 	 
                break; 	 
            }
    }
 
    map_power_grid(true); /* WCK:  Is this safe to do here?
                           * AL1: No, in NG_1.1
                           * In case of error message with ok_dial_box
                           *    the dialog cannot appear because the screen
                           *    is not set up => crash.
                           * FIXME: move all initialisation elsewhere, in 
                           *    engine.cpp or simulate.cpp.
                           */
                                
}

void
load_saved_city (char *s)
{
    char *cname = (char *) malloc (strlen (lc_save_dir) + strlen (s) + 2);
    sprintf (cname, "%s%c%s", lc_save_dir, PATH_SLASH, s);
    load_city (cname);
    free (cname);
}

void
reset_animation_times (void)
{
    int x, y;
    for (y = 0; y < WORLD_SIDE_LEN; y++)
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    if (MP_GROUP_IS_RESIDENCE(x,y))
		MP_INFO(x,y).int_3 = 0;
	    else if (MP_GROUP(x,y) == GROUP_WINDMILL)
		MP_INFO(x,y).int_4 = 0;
	    else if (MP_GROUP(x,y) == GROUP_BLACKSMITH)
		MP_INFO(x,y).int_4 = 0;
	    else if (MP_GROUP(x,y) == GROUP_MILL)
		MP_INFO(x,y).int_4 = 0;
	    else if (MP_GROUP(x,y) == GROUP_POTTERY)
		MP_INFO(x,y).int_4 = 0;
	    else if (MP_GROUP(x,y) == GROUP_CRICKET)
		MP_INFO(x,y).int_4 = 0;
	    else if (MP_GROUP(x,y) == GROUP_FIRESTATION)
		MP_INFO(x,y).int_4 = 0;
	    else if (MP_GROUP(x,y) == GROUP_FIRE)
	    {
		MP_INFO(x,y).int_1 = 0;
		MP_INFO(x,y).int_3 = 0;
	    }
	    else if (MP_GROUP(x,y) == GROUP_COMMUNE)
		MP_INFO(x,y).int_1 = 0;
	    else if (MP_GROUP(x,y) == GROUP_ROCKET)
		MP_INFO(x,y).int_5 = 0;
	    else if (MP_GROUP(x,y) == GROUP_INDUSTRY_H)
		MP_INFO(x,y).int_6 = 0;
	    else if (MP_GROUP(x,y) == GROUP_INDUSTRY_L)
		MP_INFO(x,y).int_7 = 0;
	}
}

/* Returns 1 if the city is proper version */
int 
verify_city (char *cname)
{
    gzFile fp;
    char* s;
    char str[256];
    int v;

    if (strlen(cname) == 0) {
	return 0;
    }
    if ((s = (char *) malloc (lc_save_dir_len + strlen(cname) + 2)) == 0)
	malloc_failure ();
    sprintf (s, "%s%c%s", lc_save_dir, PATH_SLASH, cname);
    if (!file_exists(s)) {
	free (s);
	return 0;
    }
    fp = gzopen( s, "r" );
    if (fp == NULL) {
	v = 0;
    } else if (1 != sscanf ( gzgets( fp, str, 256 ) , "%d", &v)) {
	v = 0;
    }
    gzclose(fp);
    free (s);
    return v == VERSION_INT;
}

#ifdef MP_SANITY_CHECK
void
sanity_check (void)
{
    static int flag = 0;
    int x, y, xx, yy;
    for (x = 0; x < WORLD_SIDE_LEN; x++)
	for (y = 0; y < WORLD_SIDE_LEN; y++) {
	    if (MP_TYPE(x,y) == CST_USED) {
		xx = MP_INFO(x,y).int_1;
		yy = MP_INFO(x,y).int_2;
		if (xx < (x - 4) || yy < (y - 4) || xx > x || yy > y ||
		    xx < 0 || xx > WORLD_SIDE_LEN ||
		    yy < 0 || yy > WORLD_SIDE_LEN) {
		    printf ("Sanity failed at %d %d, points to %d %d\n", x, y, xx, yy);
		    if (flag == 0)
			yn_dial_box ("MP sanity check error",
				     "Please mail  lincity-users@lists.sourceforge.net",
				     "telling me what you just did.",
				     "Do you think I'll find this bug?");
		    flag = 1;
		}
	    }
	}
}
#endif


void
check_endian (void)
{
    static int flag = 0;
    char *cs;
    int t, x, y;
    t = 0;
    cs = (char *) &t;
    *cs = 1;
    if (t == 1)			/* little endian */
	return;
    if (flag == 0) {
	flag = 1;
    }
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    eswap32 (&(MP_INFO(x,y).population));
	    eswap32 (&(MP_INFO(x,y).flags));
	    if (sizeof (short) == 2) {
		eswap16 (&(MP_INFO(x,y).coal_reserve));
		eswap16 (&(MP_INFO(x,y).ore_reserve));
	    } else if (sizeof (short) == 4) {
		eswap32 ((int *) &(MP_INFO(x,y).coal_reserve));
		eswap32 ((int *) &(MP_INFO(x,y).ore_reserve));
	    } else {
		printf ("Strange size (%d) for short, please mail me.\n",
			sizeof (short));
	    }
	    eswap32 (&(MP_INFO(x,y).int_1));
	    eswap32 (&(MP_INFO(x,y).int_2));
	    eswap32 (&(MP_INFO(x,y).int_3));
	    eswap32 (&(MP_INFO(x,y).int_4));
	    eswap32 (&(MP_INFO(x,y).int_5));
	    eswap32 (&(MP_INFO(x,y).int_6));
	    eswap32 (&(MP_INFO(x,y).int_7));
	}
    }
}

void
eswap32 (int *i)
{
  char *cs, c1, c2, c3, c4;
  cs = (char *) i;
  c1 = *cs;
  c2 = *(cs + 1);
  c3 = *(cs + 2);
  c4 = *(cs + 3);
  *(cs++) = c4;
  *(cs++) = c3;
  *(cs++) = c2;
  *cs = c1;
}

void
eswap16 (unsigned short *i)
{
  char *cs, c1, c2;
  cs = (char *) i;
  c1 = *cs;
  c2 = *(cs + 1);
  *(cs++) = c2;
  *cs = c1;
}
