/* ---------------------------------------------------------------------- *
 * climsg.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __climsg_h__
#define __climsg_h__

void network_join_game (void);
void network_new_city (int* originx, int* originy, int random_village);
void network_place_item (int x, int y, int selected_type);
void network_bulldoze_item (int x, int y);
void network_request_mappoint_stats (int x, int y);
void network_unrequest_mappoint_stats (void);
void network_request_main_screen (void);
void network_unrequest_main_screen (void);
void network_send_flags (int x, int y);
void network_request_mini_screen (int mini_type);
void network_unrequest_mini_screen (void);
void network_do_coal_survey (void);
void network_launch_rocket (int x, int y);


#endif	/* __climsg_h__ */
