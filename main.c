/* ---------------------------------------------------------------------- *
 * main.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "lcstring.h"
#include "cliglobs.h"
#include "lchelp.h"
#include "dialbox.h"
#include <mps.h>

/* this is for OS/2 - RVI */
#ifdef __EMX__
#include <sys/select.h>
#include <X11/Xlibint.h>      /* required for __XOS2RedirRoot */
#define chown(x,y,z)
/* #define OS2_DEFAULT_LIBDIR "/XFree86/lib/X11/lincity" */
/* This was moved to fileutil.c */
#endif

#include <sys/types.h>
#include <fcntl.h>

#if defined (WIN32)
#include <winsock.h>
#include <io.h>
#include <direct.h>
#include <process.h>
#endif

#include <time.h>

#include <ctype.h>
#include "common.h"
#ifdef LC_X11
#include <X11/cursorfont.h>
#include <lcx11.h>
#endif

#include "lctypes.h"
#include "lin-city.h"
#include "cliglobs.h"
#include "engglobs.h"
#include "timer.h"
#include "ldsvgui.h"
#include "simulate.h"
#include "mouse.h"
#include "pixmap.h"
#include "screen.h"
#include "lcintl.h"
#include "engine.h"
#include "module_buttons.h"
#include "fileutil.h"

#if defined (WIN32) && !defined (NDEBUG)
#define START_FAST_SPEED 1
//#define SKIP_OPENING_SCENE 1
#endif

#define SI_BLACK 252
#define SI_RED 253
#define SI_GREEN 254
#define SI_YELLOW 255

#define DEBUG_KEYS 1

/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */
void dump_screen (void);
void verify_package (void);
char* current_month (int current_time);
int current_year (int current_time);
void process_keystrokes (int key);
int execute_timestep (void);

/* ---------------------------------------------------------------------- *
 * Private Global Variables
 * ---------------------------------------------------------------------- */
#if defined (commentout)          /* Moved to fileutil.c */
#if defined (WIN32)
char LIBDIR[_MAX_PATH];
#elif defined (__EMX__)
#ifdef LIBDIR
#undef LIBDIR   /* yes, I know I shouldn't ;-) */
#endif
/* GCS: Presumably I can do this, right? */
#if defined (commentout)
char LIBDIR[256];
#endif
char LIBDIR[LC_PATH_MAX];
#endif
#endif

char *lc_save_dir;
char *lc_temp_file;
char save_names[10][42];

#ifdef CS_PROFILE
int prof_countdown = PROFILE_COUNTDOWN;
#endif


/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
#if !defined (WIN32)
int
main (int argc, char *argv[])
{
    return lincity_main (argc, argv);
}
#endif

void
lincity_set_locale (void)
{
    char* locale = NULL;
    char* localem = NULL;
#if defined (WIN32)
#define MAX_LANG_BUF 1024
    char* language = NULL;
    char language_buf[MAX_LANG_BUF];
#endif

#if defined (ENABLE_NLS)
#if defined (WIN32)
    /* Some special stoopid way of setting locale for microsoft gettext */
    language = getenv ("LANGUAGE");
    if (language) {
	debug_printf ("Environment variable LANGUAGE is %s\n", language);
	snprintf (language_buf, MAX_LANG_BUF, "LANGUAGE=%s", language);
	gettext_putenv(language_buf);
    } else {
	debug_printf ("Environment variable LANGUAGE not set.\n");
    }
#else
    locale = setlocale (LC_ALL, "");
    debug_printf ("Setting entire locale to %s\n", locale);
    locale = setlocale (LC_MESSAGES, "");
    debug_printf ("Setting messages locale to %s\n", locale);
    localem = setlocale (LC_MESSAGES, NULL);
    debug_printf ("Query locale is %s\n", localem);
#endif
#endif /* ENABLE_NLS */
    return;
}

int
lincity_main (int argc, char *argv[])
{
#if defined (LC_X11)
    char *geometry = NULL;
#endif

#if defined (SVGALIB)
    int q;
    vga_init ();
#endif

#if !defined (WIN32)
    signal (SIGPIPE, SIG_IGN);    /* broken pipes are ignored. */
#endif

    /* Initialize some global variables */
    //make_dir_ok_flag = 1;
    main_screen_originx = 1;
    main_screen_originy = 1;
    given_scene[0] = 0;
    quit_flag = network_flag = load_flag = save_flag 
	    = prefs_flag = cheat_flag = monument_bul_flag
	    = river_bul_flag = shanty_bul_flag;
    prefs_drawn_flag = 0;
    kmouse_val = 8;

#ifdef LC_X11
    borderx = 0;
    bordery = 0;
    parse_xargs (argc, argv, &geometry);
#endif

    /* I18n */
    lincity_set_locale ();

    /* Set up the paths to certain files and directories */
    init_path_strings ();

    /* Make sure that things are installed where they should be */
    verify_package ();

    /* Make sure the save directory exists */
    check_savedir ();

    /* Load preferences */
    load_lincityrc ();

#ifndef CS_PROFILE
#ifdef SEED_RAND
    srand (time (0));
#endif
#endif

#ifdef LC_X11
#if defined (commentout)
    borderx = 0;
    bordery = 0;
    parse_xargs (argc, argv, &geometry);
#endif
    Create_Window (geometry);
    pirate_cursor = XCreateFontCursor (display.dpy, XC_pirate);
#elif defined (WIN32)
    /* Deal with all outstanding messages */
    ProcessPendingEvents ();
#else
    parse_args (argc, argv);
    q = vga_setmode (G640x480x256);
    gl_setcontextvga (G640x480x256);
#endif

#if defined (WIN32) || defined (LC_X11)
    initialize_pixmap ();
#endif

    init_fonts ();

#if defined (SKIP_OPENING_SCENE)
    skip_splash_screen = 1;
#endif
    if (!skip_splash_screen) {
	load_start_image ();
    }

#ifdef LC_X11
    unlock_window_size ();
#endif

    Fgl_setfont (8, 8, main_font);
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);

    initialize_geometry (&scr);

#if defined (SVGALIB)
    set_vga_mode ();
#endif

    initialize_monthgraph ();
    init_mouse_registry ();
    init_mini_map_mouse ();
    mps_init();

#ifdef LC_X11
    x_key_value = 0;
#elif defined (WIN32)
    RefreshScreen ();
#endif
    setcustompalette ();
    draw_background ();
    prog_box (_("Loading the game"), 1);
    init_types ();
    init_modules();
    init_mappoint_array ();
    initialize_tax_rates ();
    prog_box ("", 95);
    mouse_hide_count = 0;
    suppress_ok_buttons = 0;
    prog_box ("", 100);
#ifdef USE_PIXMAPS
    prog_box (_("Creating pixmaps"), 1);
    init_pixmaps ();
    prog_box ("", 100);
#endif
    //draw_normal_mouse (1, 1);
#if defined (LC_X11)
    init_x_mouse ();
#endif
    init_timer_buttons();
    mouse_initialized = 1;
    //set_selected_module (CST_TRACK_LR);
    screen_setup ();

    /* Main loop! */
    client_main_loop ();

#if defined (SVGALIB)
    mouse_close ();
    vga_setmode (TEXT);
#endif

    print_results ();

#if defined (WIN32) || defined (LC_X11)
    free_pixmap ();
#endif

#if defined (WIN32)
    return 0;
#else
    exit (0);
#endif
}

void
client_main_loop (void)
{
    int quit = 0;

    /* Set up the game */
    reset_start_time ();

    update_avail_modules (0);

    screen_full_refresh ();

    if (no_init_help == 0) {
	block_help_exit = 1;
	help_flag = 1;
#if defined (commentout)
	if (make_dir_ok_flag) {
	    activate_help ("ask-dir.hlp");
	    make_dir_ok_flag = 0;
	} else {
	    activate_help ("opening.hlp");
	}
#endif
	activate_help ("opening.hlp");
    }

    /* Set speed */
#if defined (CS_PROFILE) || defined (START_FAST_SPEED)
    select_fast ();
#else
    select_medium ();
#endif
    /* Main Loop */
    do {
	int key;

	/* Get timestamp for this iteration */
	get_real_time();

	/* Process events */
#if defined (LC_X11)
	call_event ();
	key = x_key_value;
	x_key_value = 0;
#elif defined (WIN32)
	call_event ();
	key = GetKeystroke ();
#else
	mouse_update ();
	key = vga_getkey ();
#endif
	/* nothing happened if key == 0 XXX: right? */
	/* GCS: I'm not sure */
	if (key != 0) {
            process_keystrokes (key);
	}
	/* Simulate the timestep */
	quit = execute_timestep ();
    } while (quit == 0);
}

void
process_keystrokes (int key)
{

#if defined (commentout)	/* KBR 10/14/2002 - Cleanup MSVC warning */
    int retval;
#endif

    switch (key)
    {
    case 0: printf("dead!"); return;
    case ' ':   /* Space */
    case 10:    /* Linefeed/Return */
    case 13:    /* Enter */
    case 127:   /* Backspace */
	if (key == 127) {
	    cs_mouse_handler (LC_MOUSE_RIGHTBUTTON | LC_MOUSE_PRESS,
			      0, 0);
	    cs_mouse_handler (LC_MOUSE_RIGHTBUTTON | LC_MOUSE_RELEASE,
			      0, 0);
	} else {
	    cs_mouse_handler (LC_MOUSE_LEFTBUTTON | LC_MOUSE_PRESS,
			      0, 0);
	    cs_mouse_handler (LC_MOUSE_LEFTBUTTON | LC_MOUSE_RELEASE,
			      0, 0);
	}
	if (help_flag) {
	    draw_help_page ("return-2");
	}
	if (prefs_flag) {
	    close_prefs_screen ();
	    refresh_main_screen ();
	}
	break;

#if defined (SVGALIB)
    case 91:
	{
	    int w = vga_getkey ();
	    switch (w)
	    {
	    case ('A'):
		cs_mouse_handler (0, 0, -kmouse_val);
		break;
	    case ('B'):
		cs_mouse_handler (0, 0, kmouse_val);
		break;
	    case ('C'):
		cs_mouse_handler (0, kmouse_val, 0);
		break;
	    case ('D'):
		cs_mouse_handler (0, -kmouse_val, 0);
		break;
	    }
	}
	break;
#endif

#if defined (WIN32) || defined (LC_X11)
    case 1:
	/* Scroll left */
	if (x_key_shifted) {
	    adjust_main_origin (main_screen_originx - RIGHT_MOUSE_MOVE_VAL,
				main_screen_originy,
				TRUE);
	} else {
	    adjust_main_origin (main_screen_originx - 1,
				main_screen_originy,
				TRUE);
	}
	break;

    case 2:
	/* Scroll down */
	if (x_key_shifted) {
	    adjust_main_origin (main_screen_originx,
				main_screen_originy + RIGHT_MOUSE_MOVE_VAL,
				TRUE);
	} else {
	    adjust_main_origin (main_screen_originx,
				main_screen_originy + 1,
				TRUE);
	}
	break;

    case 3:
	/* Scroll up */
	if (x_key_shifted) {
	    adjust_main_origin (main_screen_originx,
				main_screen_originy - RIGHT_MOUSE_MOVE_VAL,
				TRUE);
	} else {
	    adjust_main_origin (main_screen_originx,
				main_screen_originy - 1,
				TRUE);
	}
	break;

    case 4:
	/* Scroll right */
	if (x_key_shifted) {
	    adjust_main_origin (main_screen_originx + RIGHT_MOUSE_MOVE_VAL,
				main_screen_originy,
				TRUE);
	} else {
	    adjust_main_origin (main_screen_originx + 1,
				main_screen_originy,
				TRUE);
	}
	break;
#endif

    case 'P':
    case 'p':
	select_pause ();
	break;

#ifdef DEBUG_KEYS
    case 'e':
	if (cheat () != 0)
	    people_pool += 100;
	break;

    case 'd':
	if (cheat () != 0)
	    dump_screen ();
	break;
	  
    case 'D':
	/*	dump_tcore (); */
	break;

    case 't':
	if (cheat () != 0)
	    tech_level += 1000;
	break;

    case 'T':
	if (cheat () != 0)
	    tech_level += 10000;
	break;

    case 'm':
	if (cheat () != 0) 
	    adjust_money(1000000);
	break;
#endif

    case 'f':
	do_random_fire (-1, -1, 1);
	break;

    case 'L':
    case 'l':
	load_flag = 1;
	break;

    case 'H':
    case 'h':
	activate_help ("index.hlp");
	break;

	/* Escape Key */
#ifdef LC_X11
    case 27:
#else
    case 5:
#endif
	if (help_flag) {
	    /* exit help */
	    draw_help_page("return-2"); 
	} else if (prefs_flag) {
	    close_prefs_screen();
	    refresh_main_screen ();
	} else {
	    activate_help ("menu.hlp");
	}
	break;

    case 'S':
    case 's':
	save_flag = 1;
	break;

    case 'v':
    case 'V':
	/* Toggle overlay */
	rotate_main_screen();
	break;

    case 'o':
    case 'O':
	prefs_flag = 1;
	break;

    case 'r':
        window_results();
	break;

    case 'q':
    case 'Q':
	quit_flag = 1;
	break;

    } /* end switch on keystroke */
}

/* The "guts" of main loop is here. */
int 
execute_timestep (void)
{
    static int next_time_step = 0;
    int real_quit_flag = 0;

    if (market_cb_flag == 0 && help_flag == 0 
	&& port_cb_flag == 0 && prefs_flag == 0)
    {

	if ((real_time < next_time_step || pause_flag || mt_flag)
	    && save_flag == 0 && load_flag == 0)
	{
	    if ((let_one_through == 0) || mt_flag)
	    {
		lc_usleep (1);
		return 0;
	    }
	    else
		let_one_through = 0;
	}

	if (slow_flag)
	    next_time_step = real_time + (SLOW_TIME_FOR_YEAR
					  * 1000 / NUMOF_DAYS_IN_YEAR);
	else if (fast_flag)
	    next_time_step = real_time + (FAST_TIME_FOR_YEAR
					  * 1000 / NUMOF_DAYS_IN_YEAR);
	else if (med_flag)
	    next_time_step = real_time + (MED_TIME_FOR_YEAR
					  * 1000 / NUMOF_DAYS_IN_YEAR);

	do_time_step ();

#ifdef CS_PROFILE
	if (--prof_countdown <= 0)
	    real_quit_flag = 1;
#endif

	update_main_screen (0);

	/* XXX: Shouldn't the rest be handled in update_main_screen()? */
	/* GCS: No, I don't think so.  These remaining items are 
		outside of the main screen */

	print_stats ();

	if (market_cb_flag)
	    draw_market_cb ();
	else if (port_cb_flag)	/* else- can't have both */
	    draw_port_cb ();
    }
    else /* if game is "stalled" */
    {
	if (market_cb_flag != 0 && market_cb_drawn_flag == 0)
	    draw_market_cb ();
	if (port_cb_flag != 0 && port_cb_drawn_flag == 0)
	    draw_port_cb ();
#if defined (SVGALIB)
	mouse_update ();
#endif
    }

#if defined (NETWORK_ENABLE)
    if (network_flag != 0) {
	do_network_screen ();
	network_flag = 0;
	let_one_through = 1;	/* if we are paused we need */
    }			        /* this to redraw the screen */
#endif

    if (prefs_flag != 0 && prefs_drawn_flag == 0) {
	do_prefs_screen ();
	let_one_through = 1;	/* if we are paused we need */
    }			        /* this to redraw the screen */

    if (load_flag != 0) {
#if defined (WIN32)
	DisableWindowsMenuItems ();
#endif
	if (help_flag == 0)	/* block loading when in help */
	    do_load_city ();
	load_flag = 0;
	let_one_through = 1;	/* if we are paused we need */
    }			        /* this to redraw the screen */

    else if (save_flag != 0) {
#if defined (WIN32)
	DisableWindowsMenuItems ();
#endif
	if (help_flag == 0)
	    do_save_city ();
	save_flag = 0;
	let_one_through = 1;
    }

    else if (quit_flag != 0) {
#if defined (WIN32)
	DisableWindowsMenuItems ();
#endif
	if (yn_dial_box (_("Quit The Game?")
			 ,_("Do you really want to quit?")
			 ,_("If you want to save the game select NO.")
			 ,""     /* GCS: This can't be translated!. */
			 ) != 0)
	    real_quit_flag = 1;
	else
	    quit_flag = 0;
    }

    if (help_flag != 0)
	lc_usleep (1);

#if defined (commentout)
    if (make_dir_ok_flag)
	make_savedir ();	/* sorry a bit crude :( */
#endif
    return real_quit_flag;
}

void
do_error (char *s)
{
#if defined (LC_X11) || defined (WIN32)
    HandleError (s, FATAL);
#else
    vga_setmode (TEXT);
    printf ("%s\n", s);
    exit (1);
#endif
}

int
cheat (void)
{
    if (cheat_flag != 0)
	return (1);
    /* TRANSLATORS: Test mode is like using "cheat codes" */
    if (yn_dial_box (_("TEST"), _("You have pressed a test key"),
		     _("You will only see this message once"),
		     _("Do you really want to play in test mode..."))!= 0)
    {
	cheat_flag = 1;
	print_time_for_year(); /* Displays TEST MODE or not */
	return (1);
    }
    return (0);
}

int
compile_results (void)
{
    char *s;
    FILE *outf;
    int group_count[NUM_OF_GROUPS];

    if ((s = (char *) malloc (lc_save_dir_len + strlen (LC_SAVE_DIR)
			      + strlen (RESULTS_FILENAME) + 64)) == 0)
	malloc_failure ();

    sprintf (s, "%s%c%s", lc_save_dir, PATH_SLASH, RESULTS_FILENAME);

    count_all_groups (group_count);
    if ((outf = fopen (s, "w")) == 0)
    {
	printf (_("Unable to open %s\n"), RESULTS_FILENAME);
	free (s);
	return (0);
    }
    if (cheat_flag)
	fprintf (outf, _("----- IN TEST MODE -------\n"));
    fprintf (outf, _("Game statistics from LinCity Version %s\n"), VERSION);
    if (strlen (given_scene) > 3)
	fprintf (outf, _("Initial loaded scene - %s\n"), given_scene);
    if (sustain_flag)
	fprintf (outf, _("Economy is sustainable\n"));
    fprintf (outf, _("Population  %d  of which  %d  are not housed.\n")
	     ,housed_population + people_pool, people_pool);
    fprintf (outf,
	     _("Max population %d  Number evacuated %d Total births %d\n")
	     ,max_pop_ever, total_evacuated, total_births);
    fprintf (outf,
	     _(" Date  %s %04d   Money %8d   Tech-level %5.1f (%5.1f)\n"),
	     current_month(total_time), current_year(total_time), total_money,
	     (float) tech_level * 100.0 / MAX_TECH_LEVEL,
	     (float) highest_tech_level * 100.0 / MAX_TECH_LEVEL);
    fprintf (outf,
	     _(" Deaths by starvation %7d   History %8.3f\n"),
	     total_starve_deaths, starve_deaths_history);
    fprintf (outf,
	     _("Deaths from pollution %7d   History %8.3f\n"),
	     total_pollution_deaths, pollution_deaths_history);
    fprintf (outf, _("Years of unemployment %7d   History %8.3f\n"),
	     total_unemployed_years, unemployed_history);
    fprintf (outf, _("Rockets launched %2d  Successful launches %2d\n"),
	     rockets_launched, rockets_launched_success);
    fprintf (outf, "\n");
    fprintf (outf, _("    Residences %4d         Markets %4d            Farms %4d\n"),
	     group_count[GROUP_RESIDENCE_LL] + 
	     group_count[GROUP_RESIDENCE_ML] + 
	     group_count[GROUP_RESIDENCE_HL] + 
	     group_count[GROUP_RESIDENCE_LH] + 
	     group_count[GROUP_RESIDENCE_MH] + 
	     group_count[GROUP_RESIDENCE_HH],
	     group_count[GROUP_MARKET],
	     group_count[GROUP_ORGANIC_FARM]);
    fprintf (outf, _("        Tracks %4d           Roads %4d             Rail %4d\n")
	     ,group_count[GROUP_TRACK], group_count[GROUP_ROAD]
	     ,group_count[GROUP_RAIL]);
    fprintf (outf, _("     Potteries %4d     Blacksmiths %4d            Mills %4d\n")
	     ,group_count[GROUP_POTTERY], group_count[GROUP_BLACKSMITH]
	     ,group_count[GROUP_MILL]);
    fprintf (outf, _("     Monuments %4d         Schools %4d     Universities %4d\n")
	     ,group_count[GROUP_MONUMENT], group_count[GROUP_SCHOOL]
	     ,group_count[GROUP_UNIVERSITY]);
    fprintf (outf, _(" Fire stations %4d           Parks %4d     Cricket gnds %4d\n")
	     ,group_count[GROUP_FIRESTATION], group_count[GROUP_PARKLAND]
	     ,group_count[GROUP_CRICKET]);
    fprintf (outf, _("    Coal mines %4d       Ore mines %4d         Communes %4d\n")
	     ,group_count[GROUP_COALMINE], group_count[GROUP_OREMINE]
	     ,group_count[GROUP_COMMUNE]);
    fprintf (outf, _("     Windmills %4d     Coal powers %4d     Solar powers %4d\n"),
	     group_count[GROUP_WINDMILL],
	     group_count[GROUP_COAL_POWER],
	     group_count[GROUP_SOLAR_POWER]);
    fprintf (outf, _("   Substations %4d     Power lines %4d            Ports %4d\n")
	     ,group_count[GROUP_SUBSTATION], group_count[GROUP_POWER_LINE]
	     ,group_count[GROUP_PORT]);
    fprintf (outf, _("    Light inds %4d      Heavy inds %4d        Recyclers %4d\n")
	     ,group_count[GROUP_INDUSTRY_L], group_count[GROUP_INDUSTRY_H]
	     ,group_count[GROUP_RECYCLE]);
    fprintf (outf, _("Health centres %4d            Tips %4d         Shanties %4d\n"),
	     group_count[GROUP_HEALTH], group_count[GROUP_TIP],
	     group_count[GROUP_SHANTY]);
    fclose (outf);
    free (s);
    return (1);
}


void
print_results (void)
{
#if !defined (WIN32)		/* GCS FIX: How should I do this? */
    char *s;
    if (compile_results () == 0)
	return;
    if ((s = (char *) malloc (lc_save_dir_len + strlen (LC_SAVE_DIR)
			      + strlen (RESULTS_FILENAME) + 64)) == 0)
	malloc_failure ();

    strcpy (s, "cat ");
    strcat (s, lc_save_dir);
    strcat (s, "/");
    strcat (s, RESULTS_FILENAME);
    printf ("\n");
    system (s);
    printf ("\n");
#endif
}

#if defined (commentout)
void mail_results(void)
{
    char s[256];
    if (compile_results()==0)
	return;
    strcpy(s,"mail -s 'LinCity results' lc-results@floot.demon.co.uk < ");
    strcat(s,getenv("HOME"));
    strcat(s,"/");
    strcat(s,LC_SAVE_DIR);
    strcat(s,"/");
    strcat(s,RESULTS_FILENAME);
    system(s);
}
#endif

void
window_results (void)
{
    char *s;
    if (compile_results () == 0)
	return;
    if ((s = (char *) malloc (lc_save_dir_len + strlen (LC_SAVE_DIR)
			      + strlen (RESULTS_FILENAME) + 64)) == 0)
	malloc_failure ();
    sprintf (s, "%s%c%s", lc_save_dir, PATH_SLASH, RESULTS_FILENAME);
    ok_dial_box (s, RESULTS, 0L);
}

