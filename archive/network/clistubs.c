/* ---------------------------------------------------------------------- *
 * clistubs.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lin-city.h"
#include "engglobs.h"
#include "lctypes.h"
/*#include "clinet.h"*/
/*#include "climsg.h"*/
#include "simulate.h"
/*#include "animate.h"*/
#include "engine.h"
#include "screen.h"

/* ---------------------------------------------------------------------- *
 * External Global Variables
 * ---------------------------------------------------------------------- */
extern int network_game;
//extern int make_dir_ok_flag;
extern long real_time;

/* ---------------------------------------------------------------------- *
 * Private Function Prototypes
 * ---------------------------------------------------------------------- */
void network_do_time_step (void);


/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
void 
broadcast_map_types_region (int x, int y, int size)
{
}

void 
broadcast_rocket_built (int x, int y)
{
}

void 
broadcast_rocket_fired (int x, int y, int result)
{
    display_rocket_result_dialog (result);
}

void 
new_city (int* originx, int* originy, int random_village)
{
  if (network_game) {
    network_new_city (originx, originy, random_village);
  } else {
    engine_new_city (originx, originy, random_village);
  }
}

int 
place_item (int x, int y, short selected_type)
{
  if (network_game) {
    network_place_item (x, y, selected_type);
    return 0;
  } else {
    return engine_place_item (x, y, selected_type);
  }
}

void 
bulldoze_item (int x, int y)
{
  if (network_game) {
    network_bulldoze_item (x, y);
  } else {
    engine_bulldoze_item (x, y);
  }
}

void 
request_mappoint_stats (int x, int y)
{
  if (network_game) {
    network_request_mappoint_stats (x, y);
  }
}

void 
unrequest_mappoint_stats (void)
{
  if (network_game) {
    network_unrequest_mappoint_stats ();
  }
}

void 
request_main_screen (void)
{
  if (network_game) {
    network_request_main_screen ();
  }
}

void 
unrequest_main_screen (void)
{
  if (network_game) {
    network_unrequest_main_screen ();
  }
}

void 
send_flags (int x, int y)
{
    if (network_game) {
	network_send_flags (x, y);
    }
}

void 
request_mini_screen (int mini_type)
{
  if (network_game) {
    network_request_mini_screen (mini_type);
  }
}

void 
unrequest_mini_screen (void)
{
  if (network_game) {
    network_unrequest_mini_screen ();
  }
}

void 
do_coal_survey (void)
{
    if (network_game) {
	/* Need to update engine locally because this will set the 
	   coal_survey_done flag, which allows immediate use of the 
	   mini-map and also suppresses the informational message 
	   on the client */
	engine_do_coal_survey ();
	network_do_coal_survey ();
    } else {
	engine_do_coal_survey ();
    }
}

void 
launch_rocket (int x, int y)
{
    if (network_game) {
	network_launch_rocket (x,y);
    } else {
	engine_launch_rocket (x,y);
    }
}

int 
do_time_step (void)
{
    if (network_game) {
	int rv = net_handle_messages ();
	network_do_time_step ();
	return rv;
    } else {
	engine_do_time_step ();
	return 1;
    }
}


/* ---------------------------------------------------------------------- *
 * Private Functions
 * ---------------------------------------------------------------------- */
void
network_do_time_step (void)
{
    int x, y;
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    short t = MP_TYPE(x,y);
	    if (t == CST_USED || t == CST_GREEN)
		continue;

	    switch ( get_group_of_type(t) ) {
	    case (GROUP_TRACK):
		/* Nothing to animate */
		break;
	    case (GROUP_RAIL):
		/* Nothing to animate */
		break;
	    case (GROUP_ROAD):
		/* Nothing to animate */
		break;
	    case (GROUP_ORGANIC_FARM):
		animate_organic_farm (x, y);
		break;
	    case (GROUP_MARKET):
		animate_market (x, y);
		break;
	    case GROUP_RESIDENCE_LL:
	    case GROUP_RESIDENCE_ML:
	    case GROUP_RESIDENCE_HL:
	    case GROUP_RESIDENCE_LH:
	    case GROUP_RESIDENCE_MH:
	    case GROUP_RESIDENCE_HH:
		/* Nothing to animate */
		break;
#if defined (commentout)
	    case (GROUP_POWER_LINE):
		do_power_line (x, y);
		break;
#endif
	    case GROUP_SOLAR_POWER:
		/* Nothing to animate */
		break;
#if defined (commentout)
	    case (GROUP_SUBSTATION):
		do_power_substation (x, y);
		break;
#endif
	    case (GROUP_COALMINE):
		animate_coalmine (x, y);
		break;
	    case GROUP_COAL_POWER:
	        animate_power_source_coal (x, y);
		break;
	    case (GROUP_INDUSTRY_L):
		animate_industry_l (x, y);
		break;
	    case (GROUP_INDUSTRY_H):
		animate_industry_h (x, y);
		break;
	    case (GROUP_COMMUNE):
		animate_commune (x, y);
		break;
	    case (GROUP_OREMINE):
		animate_oremine (x, y);
		break;
	    case (GROUP_PORT):
		/* Nothing to animate */
		break;
	    case (GROUP_TIP):
		animate_tip (x, y);
		break;
	    case (GROUP_PARKLAND):
		/* Nothing to animate */
		break;
	    case (GROUP_UNIVERSITY):
		/* Nothing to animate */
		break;
	    case (GROUP_RECYCLE):
		/* Nothing to animate */
		break;
	    case (GROUP_HEALTH):
		/* Nothing to animate */
		break;
	    case (GROUP_ROCKET):
		animate_rocket (x, y);
		break;
	    case (GROUP_WINDMILL):
		animate_windmill (x, y);
		break;
	    case (GROUP_MONUMENT):
		animate_monument (x, y);
		break;
	    case (GROUP_SCHOOL):
		/* Nothing to animate */
		break;
	    case (GROUP_BLACKSMITH):
		animate_blacksmith (x, y);
		break;
	    case (GROUP_MILL):
		animate_mill (x, y);
		break;
	    case (GROUP_POTTERY):
		animate_pottery (x, y);
		break;
	    case (GROUP_FIRESTATION):
		animate_firestation (x, y);
		break;
	    case (GROUP_CRICKET):
		animate_cricket (x, y);
		break;
	    case (GROUP_FIRE):
		animate_fire (x, y);
		break;
	    case (GROUP_SHANTY):
		/* Nothing to animate */
		break;
	    }
	}
    }
}
