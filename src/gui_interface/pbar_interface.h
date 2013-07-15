/* pbar.h: handles rate-of-change indicators
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * Portions copyright (c) 2001 Corey Keasling.
 * ---------------------------------------------------------------------- */

#ifndef _pbar_h
#define _pbar_h

//#include "geometry.h"

void init_pbars (void);
void draw_pbars (void);
void refresh_pbars (void);
void refresh_population_text (void);

void update_pbar (int pbar_num, int value, int month_flag);
void update_pbars_monthly(void);

#define NUM_PBARS 15
#define OLD_NUM_PBARS 9

/* Position constants aka linenumbers for pbars struct */

#define PPOP    0
#define PTECH   1
#define PMONEY  2
#define PFOOD   3
#define PJOBS   4
#define PGOODS  5
#define PCOAL   6 
#define PORE    7
#define PSTEEL  8

#define PBAR_PAGE_SHIFT 6

#define PPOL    9
#define PKWH   10
#define PMWH   11
#define PWATER 12
#define PWASTE 13
#define PHOUSE 14

/* Number of elements per pbar */
#define PBAR_DATA_SIZE 12

struct pbar_st
{
    int oldtot;
    int tot;
    int diff;

    int data_size;
    int data[PBAR_DATA_SIZE];
};

extern struct pbar_st pbars[NUM_PBARS];

#endif





/** @file gui_interface/pbar_interface.h */

