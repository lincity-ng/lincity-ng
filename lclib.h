/* ---------------------------------------------------------------------- *
 * lclib.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * Portions copyright (c) Corey Keasling, 2001.
 * ---------------------------------------------------------------------- */

#ifndef _LCLIB_H
#define _LCLIB_H
char * current_month(int current_time);
int current_year (int current_time);

void format_number5 (char* str, int num);

void format_pos_number4 (char* str, int num);

void format_money (char* str);

/* format_power calls snprintf, hence size */
void format_power (char * str, size_t size, long power, char unit);
#endif

