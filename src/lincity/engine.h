/* ---------------------------------------------------------------------- *
 * engine.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __engine_h__
#define __engine_h__

int place_item (int x, int y, short type);
int bulldoze_item (int x, int y);
void do_coal_survey (void);
void do_bulldoze_area (short fill, int xx, int yy);
void launch_rocket (int x, int y);
void connect_rivers (void);
int adjust_money(int value);
void fire_area (int x, int y);

extern long real_time;

#endif	/* __engine_h__ */
