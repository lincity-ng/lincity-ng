/* ---------------------------------------------------------------------- *
 * sermain.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "lcstring.h"

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

#if defined (WIN32)
#include <winsock.h>
#include <io.h>
#include <direct.h>
#include <process.h>
#endif

#include <ctype.h>
#include "common.h"
#include "lctypes.h"
#include "lin-city.h"
#include "cliglobs.h"
#include "engglobs.h"
#include "timer.h"
#include "fileutil.h"
#include "simulate.h"
#include "shrnet.h"
#include "sernet.h"

/* ---------------------------------------------------------------------- *
 * Private Global Variables
 * ---------------------------------------------------------------------- */
#if defined (WIN32)
char LIBDIR[_MAX_PATH];
#elif defined (__EMX__)
#ifdef LIBDIR
#undef LIBDIR   /* yes, I know I shouldn't ;-) */
#endif
char LIBDIR[256];
#endif

int zoom_originx, zoom_originy;
int let_one_through;


/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */
void init_connections(void);
int sniff_packets(void);
void debug_print_stats (void);


/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
void
initialize_server (void)
{
  init_connections ();
  net_init_interface ();
  server_open_socket ();
}

/* For now, we only need the filename of a savefile */
char* 
parse_server_args (int argc, char **argv)
{
#if !defined (WIN32) /* GCS FIX -- need to deliver getopt() w/ w32 version */
    int option;
    extern char *optarg;
    while ((option = getopt (argc, argv, "f:")) != EOF)
    {
	switch (option)
	{
	case 'f':
	    return optarg;
	    break;
	}
    }
#endif
    return 0;
}


int
main (int argc, char *argv[])
{
    char* load_filename;

    /* Initialize some global variables */
    initialize_server ();

    /* Set up the paths to certain files and directories */
    init_path_strings ();

#ifndef CS_PROFILE
#ifdef SEED_RAND
    srand (time (0));
#endif
#endif

    init_types ();
    initialize_tax_rates ();

    reset_start_time ();

    load_filename = parse_server_args (argc, argv);
    if (load_filename && file_exists(load_filename)) {
	printf ("Server is trying to load: %s\n", load_filename);
	load_city (load_filename);
	zoom_originx = main_screen_originx;
	zoom_originy = main_screen_originy;
    } else {
	engine_new_city (&zoom_originx, &zoom_originy, 1);
    }

    printf ("Server starting main loop!\n");
    while (1) {
	sniff_packets ();
	engine_do_time_step ();
	get_real_time ();
	send_periodic_messages ();
	if (total_time % 1000 == 0) {
	    debug_print_stats ();
	}
    }

    return 0;
}

void 
debug_print_stats (void)
{
  printf ("----------------\n");
  printf ("Time: %d\n", total_time);
  printf ("Population: %d\n", population + people_pool);
  printf ("Food: %d\n", food_in_markets / 1000);
}

