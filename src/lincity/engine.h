/* ---------------------------------------------------------------------- *
 * engine.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __engine_h__
#define __engine_h__

int adjust_money(int value);
int place_item(int x, int y, short type);
int is_allowed_here(int x, int y, short type);
int bulldoze_item(int x, int y);
void do_bulldoze_area(short fill, int xx, int yy);
void do_coal_survey(void);
void do_random_fire(int x, int y, int pwarning);
void fire_area(int x, int y);
void desert_frontier(int originx, int originy, int w, int h);
void connect_rivers(void);

void launch_rocket(int x, int y);

/* called only by simulate.cpp */
void do_daily_ecology(void);
void do_pollution(void);
void do_fire_health_cricket_power_cover(void);

extern long real_time;

#endif /* __engine_h__ */

/** @file lincity/engine.h */

