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

#define pbar_adjust_pop(diff) 2 * diff
#define pbar_adjust_tech(diff) diff > 0 ? diff / 4 + 1 : -((-diff+1)/ 2)
#define pbar_adjust_food(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_jobs(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_coal(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_goods(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_ore(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_steel(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_money(diff) diff  > 0 ? diff / 800 + 1 : diff / 400 


/* XXX: wck: write_pbar_* changes font colours every time its called; only
   need to do this once.  Maybe it should be folded in.*/

void
refresh_population_text (void)
{
  /* GCS: This function is kind of a hack, but I need the population 
     to be refreshed immediately after the rocket is launched.
     Therefore, this function! */
    Rect * b;
    update_pbar (PPOP, housed_population + people_pool, 0);
    b = &scr.pbar_pop;
    write_pbar_int (b, PPOP);
}

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

    int i;

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

    for (i = 0; i < (pbar->data_size - 1); i++) {
	if (month_flag) 
	    pbar->tot += (pbar->data[i] = pbar->data[i+1]);
	else
	    pbar->tot += pbar->data[i];
    }


    pbar->tot += pbar->data[i] = value;
    pbar->diff = pbar->tot - pbar->oldtot;
}

void
update_pbars_daily()
{
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
  if (button != LC_MOUSE_RIGHTBUTTON)
    return;

  /* check for help with pbars */
  activate_help ("pbar.hlp");
}
