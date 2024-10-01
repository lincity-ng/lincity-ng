/* ---------------------------------------------------------------------- *
 * src/lincity/engine.h
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#ifndef __engine_h__
#define __engine_h__

int adjust_money(int value);
int place_item(int x, int y);
int bulldoze_item(int x, int y);
void do_bulldoze_area(int x, int y);
void do_coal_survey(void);
void do_random_fire(int x, int y, int pwarning);
void fire_area(int x, int y);
void desert_water_frontiers(int originx, int originy, int w, int h);
void connect_rivers(int x, int y);
int check_group(int x, int y);
int check_topgroup(int x, int y);
int check_lvgroup(int x, int y);
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
