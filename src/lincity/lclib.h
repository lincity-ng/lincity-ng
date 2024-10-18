/* ---------------------------------------------------------------------- *
 * src/lincity/lclib.h
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

#ifndef _LCLIB_H
#define _LCLIB_H

// FIXME: is this necessary to export this?
extern char *months[];

const char *current_month(int current_time);
int current_year(int current_time);
//void format_number5(char *str, int num);
//void format_pos_number4(char *str, int num);
//void format_power(char *str, size_t size, long power);
void *lcalloc(size_t size);
int commify(char *str, size_t size, int argnum);
void pad_with_blanks(char *str, int size);
void num_to_ansi(char *s, size_t size, long num);

int min_int(int i1, int i2);
int max_int(int i1, int i2);

#endif

/** @file lincity/lclib.h */
