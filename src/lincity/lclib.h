/* ---------------------------------------------------------------------- *
 * lclib.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * Portions copyright (c) Corey Keasling, 2001.
 * ---------------------------------------------------------------------- */

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

