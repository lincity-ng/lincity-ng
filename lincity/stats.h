/* ---------------------------------------------------------------------- *
 * stats.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2002.
 * Portions copyright (c) Corey Keasling, 2000-2002.
 * ---------------------------------------------------------------------- */

#ifndef __stats_h__
#define __stats_h__ 

#include "lincity.h"

/* Statistics, Accumulators and Counters all reside here */

/* Daily accumulators */

/*
  Note on variables (GCS):
  --
  Variables that begin with a "t" (e.g. tpopulation) are monthly 
  accumulators.  They are initialized to zero on the first day of 
  the month.
  --
  The yearly accumulators have no prefix (e.g. income_tax).
  --
  The daily accumulators have no prefix either (e.g. population).
  --
  Variables that begin with a "ly" (Last Year;  e.g. ly_university_cost)
  are yearly display variables.  They will be displayed in the mini-map
  when the user clicks on the pound sterling icon.
*/

/* Function prototypes */
void init_inventory(void);
void inventory(int x, int y);
void init_daily(void);
void init_monthly(void);
void init_yearly(void);
void add_daily_to_monthly(void);

#endif
