/* pbar.c: handles rate-of-change indicators 
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * Portions copyright (c) 2001 Corey Keasling.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <math.h>
#include "cliglobs.h"
#include "lchelp.h"
#include "mouse.h"
#include "shrglobs.h"
#include "pbar.h"
#include "lin-city.h"
#include "lclib.h"
#include "stats.h"
#include "engglobs.h"

struct pbar_st pbars[NUM_PBARS];

#ifdef commentout
int pbar_pop[12], pbar_pop_oldtot, pop_diff, pbar_pop_olddiff;
int pbar_tech[12], pbar_tech_oldtot, tech_diff, pbar_tech_olddiff;
int pbar_food[12], pbar_food_oldtot, food_diff, pbar_food_olddiff;
int pbar_jobs[12], pbar_jobs_oldtot, jobs_diff, pbar_jobs_olddiff;
int pbar_coal[12], pbar_coal_oldtot, coal_diff, pbar_coal_olddiff;
int pbar_goods[12], pbar_goods_oldtot, goods_diff, pbar_goods_olddiff;
int pbar_ore[12], pbar_ore_oldtot, ore_diff, pbar_ore_olddiff;
int pbar_steel[12], pbar_steel_oldtot, steel_diff, pbar_steel_olddiff;
int pbar_money[12], pbar_money_oldtot, money_diff, pbar_money_olddiff;
#endif

void
init_pbars (void)
{
    int i, p;
    for (p = 0; p < NUM_PBARS; p++) {
	pbars[p].data_size = 0;
	pbars[p].oldtot = 0;
	pbars[p].tot = 0;
	pbars[p].diff = 0;
	for (i = 0; i < PBAR_DATA_SIZE; i++)
	    pbars[p].data[i] = 0;
    }

    /* Initial population is 100 for empty board or 200 
       for random villiage. */
    for (i = 0; i < PBAR_DATA_SIZE; i++)
	pbars[PPOP].data[i] = 100;
}

void
pbars_full_refresh (void)
{
    Rect* pba = &scr.pbar_area;
    draw_small_bezel (pba->x+4, pba->y+4, pba->w-8, pba->h-8, yellow(0));
    init_pbar_text ();
    draw_pbars ();
}

void
init_pbar_text (void)
{
    clear_pbar_text (&scr.pbar_pop);
    clear_pbar_text (&scr.pbar_tech);
    clear_pbar_text (&scr.pbar_food);
    clear_pbar_text (&scr.pbar_jobs);
    clear_pbar_text (&scr.pbar_money);
    clear_pbar_text (&scr.pbar_coal);
    clear_pbar_text (&scr.pbar_goods);
    clear_pbar_text (&scr.pbar_ore);
    clear_pbar_text (&scr.pbar_steel);
}

/* ---------------------------------------------------------------------- *
 * Pbar drawing function
 * ---------------------------------------------------------------------- */

void 
draw_pbar (Rect* b, char* graphic)
/* XXX: WCK: why not just make the graphic include the black? */
/* GCS: Good idea, but xpicedit is painful to use! */
{
    Fgl_fillbox (b->x, b->y, b->w, b->h, 0);
    Fgl_putbox (b->x + (b->w / 2) - 8, b->y, 16, 16, graphic);
}

void
draw_pbars (void)
{
    draw_pbar (&scr.pbar_pop, pop_pbar_graphic);
    draw_pbar (&scr.pbar_tech, tech_pbar_graphic);
    draw_pbar (&scr.pbar_food, food_pbar_graphic);
    draw_pbar (&scr.pbar_jobs, jobs_pbar_graphic);
    draw_pbar (&scr.pbar_money, money_pbar_graphic);
    draw_pbar (&scr.pbar_coal, coal_pbar_graphic);
    draw_pbar (&scr.pbar_goods, goods_pbar_graphic);
    draw_pbar (&scr.pbar_ore, ore_pbar_graphic);
    draw_pbar (&scr.pbar_steel, steel_pbar_graphic);
}

/* Text functions */

void
clear_pbar_text (Rect* pbar)
{
    Fgl_fillbox (pbar->x + pbar->w + 1, pbar->y, PBAR_TEXT_W, pbar->h, 0);
}




/* ---------------------------------------------------------------------- *
 * Horizontal pbar functions (pbar2)
 * ---------------------------------------------------------------------- */
void 
write_pbar_int (Rect* b, int val)
{
    char s[16];
    format_number5 (s, pbars[val].data[pbars[val].data_size-1]);
    Fgl_setfontcolors (0, 255);
    Fgl_write (b->x + b->w + 25, b->y + 4, s);
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
}

void
write_pbar_text (Rect* b, char * s)
{
    Fgl_setfontcolors (0, 255); 
    Fgl_write (b->x + b->w + 25, b->y + 4, s);
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
}


/* XXX: WCK: Macros anyone? */
/* GCS: I thought I might like to change the "sensitivity" of the pbars
   on a case-by-case basis, but never got around to it. */
/* WCK: sure, but the preprocessor can still do some of the work, so: */

#ifndef old_adjusts

#define pbar_adjust_pop(diff) 2 * diff
#define pbar_adjust_tech(diff) diff > 0 ? diff / 4 + 1 : -((-diff+1)/ 2)
#define pbar_adjust_food(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_jobs(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_coal(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_goods(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_ore(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_steel(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_money(diff) diff  > 0 ? diff / 800 + 1 : diff / 400 

#else

inline int 
pbar_adjust_pop (int diff)
{
    return 2*diff;
} 

inline int 
pbar_adjust_tech (int diff)
{
    return diff > 0 ? diff / 4 + 1 : -((-diff+1)/ 2);
}

inline int 
pbar_adjust_food (int diff)
{
    return diff > 0 ? diff / 2 + 1 : diff;
}

inline int 
pbar_adjust_jobs (int diff)
{
    return diff > 0 ? diff / 2 + 1 : diff;
}

inline int 
pbar_adjust_coal (int diff)
{
    return diff > 0 ? diff / 2 + 1 : diff;
}

inline int 
pbar_adjust_goods (int diff)
{
    return diff > 0 ? diff / 2 + 1 : diff;
}

inline int 
pbar_adjust_ore ( diff)
{
    return diff > 0 ? diff / 2 + 1 : diff;
}

inline int 
pbar_adjust_steel (int diff)
{
    return diff > 0 ? diff / 2 + 1 : diff;
}

#endif


/* XXX: wck: write_pbar_* changes font colours every time its called; only
   need to do this once.  Maybe it should be folded in.*/

void 
refresh_pbars (void)
{
    Rect * b;
    char s[10];

    /* Population */
    b = &scr.pbar_pop;
    draw_pbar_new (b, pbar_adjust_pop(pbars[PPOP].diff));
    write_pbar_int (b, PPOP);

    /* Technology */
    b = &scr.pbar_tech;
    draw_pbar_new (b, pbar_adjust_tech(pbars[PTECH].diff));

    snprintf (s, 10, "%5.1f", 
	      (float) pbars[PTECH].data[pbars[PTECH].data_size - 1] * 
	      100.0 / MAX_TECH_LEVEL);

    write_pbar_text (b, s);

    /* Food */
    b = &scr.pbar_food;
    draw_pbar_new (b, pbar_adjust_food(pbars[PFOOD].diff));
    write_pbar_int (b, PFOOD);

    /* Jobs */
    b = &scr.pbar_jobs;
    draw_pbar_new (b, pbar_adjust_jobs(pbars[PFOOD].diff));
    write_pbar_int (b, PJOBS);

    /* Coal */
    b = &scr.pbar_coal;
    draw_pbar_new (b, pbar_adjust_coal(pbars[PCOAL].diff));
    write_pbar_int (b, PCOAL);

    /* Goods */
    b = &scr.pbar_goods;
    draw_pbar_new (b, pbar_adjust_goods(pbars[PGOODS].diff));
    write_pbar_int (b, PGOODS);

    /* Ore */
    b = &scr.pbar_ore;
    draw_pbar_new (b, pbar_adjust_ore(pbars[PORE].diff));
    write_pbar_int (b, PORE);

    /* Steel */
    b = &scr.pbar_steel;
    draw_pbar_new (b, pbar_adjust_steel(pbars[PSTEEL].diff));
    write_pbar_int (b, PSTEEL);

    /* Money */
    b = &scr.pbar_money;
    draw_pbar_new (b, pbar_adjust_money(pbars[PMONEY].diff));
    write_pbar_int (b, PMONEY);
}


/* 
   update_pbar: add a new value to the array used to calculate the
   pbar display.  If month_flag is 1, the oldtotal is updated, all
   values are shifted up (dropping the first one), and the new value
   is added at the end. If 0, the new value replaces the most recently
   updated value.  The data is summed and the result compared to the
   old total.

*/

void
update_pbar (int pbar_num, int value, int month_flag)
{

    int i, tot = 0;

    struct pbar_st * pbar = &pbars[pbar_num];

    if (month_flag) {
	pbar->oldtot = pbar->tot;

	/* If the dataset isn't full, just add it and forget month_flag */
	if (pbar->data_size < PBAR_DATA_SIZE) {
	    pbar->data_size++;
	    month_flag = 0;
	}
    }

    pbar->tot = 0;

    if (pbar_num == PFOOD)
	printf ("%d (%d) [%d]:\t", pbar_num, pbar->data_size, value);


    for (i = 0; i < (pbar->data_size - 1); i++) {
	if (month_flag) 
	    pbar->tot += (pbar->data[i] = pbar->data[i+1]);
	else
	    pbar->tot += pbar->data[i];

	if (pbar_num == PFOOD)
	    printf("%d:%d\t",i,pbar->data[i]);
    }

//    i++;

    pbar->tot += pbar->data[i] = value;

    pbar->diff = pbar->tot - pbar->oldtot;

    if (pbar_num == PFOOD)
	printf("%d:%d\n",i,pbar->data[i]);

}

void
update_pbars_daily()
{
    printf("daily:\t");
    update_pbar (PPOP, housed_population + people_pool, 0);
    update_pbar (PTECH, tech_level, 0);
    update_pbar (PFOOD, food_in_markets / 1000, 0);
    update_pbar (PJOBS, jobs_in_markets / 1000, 0);
    update_pbar (PCOAL, coal_in_markets / 250, 0);
    update_pbar (PGOODS, goods_in_markets / 500, 0);
    update_pbar (PORE, ore_in_markets / 500, 0);
    update_pbar (PSTEEL, steel_in_markets / 25, 0);
    update_pbar (PMONEY, total_money, 0);
}

void
update_pbars_monthly()
{
    printf("monthly:\t");
    update_pbar (PPOP, housed_population + people_pool, 1);
    update_pbar (PTECH, tech_level, 1);
    update_pbar (PFOOD, tfood_in_markets / data_last_month, 1);
    update_pbar (PJOBS, tjobs_in_markets / data_last_month, 1);
    update_pbar (PCOAL, tcoal_in_markets / data_last_month, 1);
    update_pbar (PGOODS, tgoods_in_markets / data_last_month, 1);
    update_pbar (PORE, tore_in_markets / data_last_month, 1);
    update_pbar (PSTEEL, tsteel_in_markets / data_last_month, 1);
    update_pbar (PMONEY, total_money, 1);
}


/* XXX: WCK: I hate redundant code!  This could be made generic */
	
#ifdef OLD_PBARS
void
update_pbar_pop (int pop)
{
    int i, tot = 0;

    /* go on, it's only 11 :) */
    for (i = 0; i < 11; i++)
	tot += (pbar_pop[i] = pbar_pop[i + 1]);
    tot += (pbar_pop[11] = pop);
    pop_diff = tot - pbar_pop_oldtot;

    pbar_pop_oldtot = tot;
    pbar_pop_olddiff = pop_diff;
}

void
update_pbar_tech (int tech)
{
    int i, tot = 0;

    /* go on, it's only 11 :) */
    for (i = 0; i < 11; i++)
	tot += (pbar_tech[i] = pbar_tech[i + 1]);
    tot += (pbar_tech[11] = tech); /* was (tech / 8) here, now below */
    tot /= 8;
    tech_diff = tot - pbar_tech_oldtot;

    pbar_tech_oldtot = tot;
    pbar_tech_olddiff = tech_diff;
}

void
update_pbar_food (int food)
{
    int i, tot = 0;

    /* go on, it's only 11 :) */
    for (i = 0; i < 11; i++)
	tot += (pbar_food[i] = pbar_food[i + 1]);
    tot += (pbar_food[11] = food);
    food_diff = tot - pbar_food_oldtot;

    pbar_food_oldtot = tot;
    pbar_food_olddiff = food_diff;
}

void
update_pbar_jobs (int jobs)
{
    int i, tot = 0;

    /* go on, it's only 11 :) */
    for (i = 0; i < 11; i++)
	tot += (pbar_jobs[i] = pbar_jobs[i + 1]);
    tot += (pbar_jobs[11] = jobs);
    jobs_diff = tot - pbar_jobs_oldtot;

    pbar_jobs_oldtot = tot;
    pbar_jobs_olddiff = jobs_diff;
}

void
update_pbar_coal (int coal)
{
    int i, tot = 0;

    /* go on, it's only 11 :) */
    for (i = 0; i < 11; i++)
	tot += (pbar_coal[i] = pbar_coal[i + 1]);
    tot += (pbar_coal[11] = coal);
    coal_diff = tot - pbar_coal_oldtot;

    pbar_coal_oldtot = tot;
    pbar_coal_olddiff = coal_diff;
}

void
update_pbar_goods (int goods)
{
    int i, tot = 0;

    /* go on, it's only 11 :) */
    for (i = 0; i < 11; i++)
	tot += (pbar_goods[i] = pbar_goods[i + 1]);
    tot += (pbar_goods[11] = goods);
    goods_diff = tot - pbar_goods_oldtot;

    pbar_goods_oldtot = tot;
    pbar_goods_olddiff = goods_diff;
}

void
update_pbar_ore (int ore)
{
    int i, tot = 0;

    /* go on, it's only 11 :) */
    for (i = 0; i < 11; i++)
	tot += (pbar_ore[i] = pbar_ore[i + 1]);
    tot += (pbar_ore[11] = ore);
    ore_diff = tot - pbar_ore_oldtot;

    pbar_ore_oldtot = tot;
    pbar_ore_olddiff = ore_diff;
}

void
update_pbar_steel (int steel)
{
    int i, tot = 0;

    /* go on, it's only 11 :) */
    for (i = 0; i < 11; i++)
	tot += (pbar_steel[i] = pbar_steel[i + 1]);
    tot += (pbar_steel[11] = steel);
    steel_diff = tot - pbar_steel_oldtot;

    pbar_steel_oldtot = tot;
    pbar_steel_olddiff = steel_diff;

}

void
update_pbar_money (int money)
{
    int i, tot = 0;

    /* go on, it's only 11 :) */
    for (i = 0; i < 11; i++)
	tot += (pbar_money[i] = pbar_money[i + 1]);
    tot += (pbar_money[11] = money);
    money_diff = tot - pbar_money_oldtot;

    pbar_money_oldtot = tot;
    pbar_money_olddiff = money_diff;
}

#endif


int 
compute_pbar_offset (Rect* b, int val)
{
    int offset;
    int val_abs = val > 0 ? val : -val;

    if (!val) 
	return 0;

    offset = (int) log (val_abs);
    if (offset > (b->w / 2) - 8) {
	offset = (b->w / 2) - 8;
    }
    offset = val > 0 ? offset : -offset;

    return offset;
}


void
/* was: draw_pbar_new (Rect* b, int total, int val) */
draw_pbar_new (Rect* b, int val)
{

    int offset;
    int spike_start, spike_end;

    /* offset, oldoffset are the size of spike in pixels */
    offset = compute_pbar_offset (b, val);

    /* Clear both sides of the pbar */
    Fgl_fillbox (b->x, b->y, b->w / 2 - 8, b->h, 0);
    Fgl_fillbox (b->x + (b->w / 2) + 8, b->y, b->w / 2 - 8, b->h, 0);


    /* Figure out pos/neg and length and draw */
    if (offset > 0) {
/* Right/Positive */
      spike_start = b->x + (b->w / 2) + 8;
      spike_end = spike_start + offset;
      Fgl_fillbox (spike_start, b->y+2, spike_end - spike_start, b->h-4, 
		   (green(12)));
    } else if (offset < 0) {
/* Left/Negative */
      spike_end = b->x + (b->w / 2) - 8;
      spike_start = spike_end + offset;
      Fgl_fillbox (spike_start, b->y+2, spike_end - spike_start, b->h-4, 
		   (red(12))); 
    } 
}



void
pbar_mouse(int rawx, int rawy, int button) 
{
  int x = rawx, y = rawy; /* Eventually might use internal coords */

  if (button != LC_MOUSE_RIGHTBUTTON)
    return;

  /* check for help with pbars */
  activate_help ("pbar.hlp");

#if defined (commentout)
  if (mouse_in_rect (&scr.pbar_pop,x,y)) {
    activate_help ("pbar-pop.hlp");
    return;
  }
  else if (mouse_in_rect (&scr.pbar_tech,x,y)) {
    activate_help ("pbar-tech.hlp");
    return;
  }
  else if (mouse_in_rect (&scr.pbar_food,x,y)) {
    activate_help ("pbar-food.hlp");
    return;
  }
  else if (mouse_in_rect (&scr.pbar_jobs,x,y)) {
    activate_help ("pbar-jobs.hlp");
    return;
  }
  else if (mouse_in_rect (&scr.pbar_money,x,y)) {
    activate_help ("pbar-money.hlp");
    return;
  }
  else if (mouse_in_rect (&scr.pbar_coal,x,y)) {
    activate_help ("pbar-coal.hlp");
    return;
  }
  else if (mouse_in_rect (&scr.pbar_goods,x,y)) {
    activate_help ("pbar-goods.hlp");
    return;
  }
  else if (mouse_in_rect (&scr.pbar_ore,x,y)) {
    activate_help ("pbar-ore.hlp");
    return;
  }
  else if (mouse_in_rect (&scr.pbar_steel,x,y)) {
    activate_help ("pbar-steel.hlp");
    return;
  }
#endif
}
