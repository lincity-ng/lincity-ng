/* ---------------------------------------------------------------------- *
 * stats.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2002.
 * Portions copyright (c) Corey Keasling, 2000-2002.
 * ---------------------------------------------------------------------- */

#ifndef __stats_h__
#define __stats_h__

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
#include "lintypes.h"



/* monthly */
extern int tpopulation;
extern int thousing;
extern int tstarving_population;
extern int tunemployed_population;



/* yearly */
extern int income_tax;
extern int coal_tax;
extern int goods_tax;
extern int export_tax;
extern int import_cost;
extern int unemployment_cost;
extern int transport_cost;
extern int windmill_cost;
extern int university_cost;
extern int recycle_cost;
extern int deaths_cost;
extern int health_cost;
extern int rocket_pad_cost;
extern int school_cost;
extern int fire_cost;
extern int cricket_cost;
extern int other_cost;

/* yearly */
extern int ly_income_tax;
extern int ly_coal_tax;
extern int ly_goods_tax;
extern int ly_export_tax;
extern int ly_import_cost;
extern int ly_other_cost;
extern int ly_unemployment_cost;
extern int ly_transport_cost;
extern int ly_fire_cost;
extern int ly_university_cost;
extern int ly_recycle_cost;
extern int ly_school_cost;
extern int ly_deaths_cost;
extern int ly_health_cost;
extern int ly_rocket_pad_cost;
extern int ly_interest;
extern int ly_windmill_cost;
extern int ly_cricket_cost;

/* Averaging variables */
extern int data_last_month;

/* Function prototypes */
void init_inventory(void);
void inventory(int x, int y);
void init_daily(void);
void init_monthly(void);
void init_census(void);
void init_yearly(void);
void add_daily_to_monthly(void);

#endif

/** @file lincity/stats.h */

