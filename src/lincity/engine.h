/* ---------------------------------------------------------------------- *
 * engine.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __engine_h__
#define __engine_h__

int adjust_money(int value);
int place_item(int x, int y);
int bulldoze_item(int x, int y);
void do_bulldoze_area(int x, int y);
void do_coal_survey(void);
void do_random_fire(int x, int y, int pwarning);
void fire_area(int x, int y);
void desert_frontier(int originx, int originy, int w, int h);
void connect_rivers(int x, int y);
int check_group(int x, int y);
int check_topgroup(int x, int y);
bool check_water(int x, int y);

int find_group(int x, int y, unsigned short group);
bool is_bare_area(int x, int y, int size);
int find_bare_area(int x, int y, int size);

/* called only by simulate.cpp */
void do_daily_ecology(void);
void do_pollution(void);
void scan_pollution(void);
void do_fire_health_cricket_power_cover(void);

extern long real_time;

#endif /* __engine_h__ */

/** @file lincity/engine.h */

