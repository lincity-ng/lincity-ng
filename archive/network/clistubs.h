/* ---------------------------------------------------------------------- *
 * clistubs.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __clistubs_h__
#define __clistubs_h__

int place_item (int x, int y, int selected_type);
void bulldoze_item (int x, int y);

void request_mappoint_stats (int x, int y);
void unrequest_mappoint_stats (void);
void request_main_screen (void);
void unrequest_main_screen (void);
void send_flags (int x, int y);
void request_mini_screen (int mini_type);
void unrequest_mini_screen (void);
void do_coal_survey (void);
void request_main_screen (void);
void broadcast_rocket_fired (int x, int y, int result);

#endif	/* __clistubs_h__ */
