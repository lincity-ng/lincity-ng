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

/* Constants */

#define PBAR_POP_X      (PBAR_AREA_X + 4)
#define PBAR_POP_Y      (PBAR_AREA_Y + 4)
#define PBAR_TECH_X     PBAR_POP_X
#define PBAR_TECH_Y     PBAR_POP_Y+(PBAR_H+1)
#define PBAR_FOOD_X     PBAR_POP_X
#define PBAR_FOOD_Y     PBAR_POP_Y+(PBAR_H+1)*2
#define PBAR_JOBS_X     PBAR_POP_X
#define PBAR_JOBS_Y     PBAR_POP_Y+(PBAR_H+1)*3
#define PBAR_COAL_X     PBAR_POP_X
#define PBAR_COAL_Y     PBAR_POP_Y+(PBAR_H+1)*4
#define PBAR_GOODS_X    PBAR_POP_X
#define PBAR_GOODS_Y    PBAR_POP_Y+(PBAR_H+1)*5
#define PBAR_ORE_X      PBAR_POP_X
#define PBAR_ORE_Y      PBAR_POP_Y+(PBAR_H+1)*6
#define PBAR_STEEL_X    PBAR_POP_X
#define PBAR_STEEL_Y    PBAR_POP_Y+(PBAR_H+1)*7
#define PBAR_MONEY_X    PBAR_POP_X
#define PBAR_MONEY_Y    PBAR_POP_Y+(PBAR_H+1)*8

/* Type:Position constants for pbars struct */

#define PPOP 0
#define PTECH 1
#define PFOOD 2
#define PJOBS 3
#define PMONEY 4
#define PCOAL 5
#define PGOODS 6
#define PORE 7
#define PSTEEL 8


#define NUM_PBARS 9

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

