/* pbar.c: handles rate-of-change indicators 
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * Portions copyright (c) 2001 Corey Keasling.
 * ---------------------------------------------------------------------- */

#include <math.h>
#include "cliglobs.h"
#include "lchelp.h"
#include "mouse.h"
#include "shrglobs.h"
#include "pbar.h"
#include "lin-city.h"


void
init_pbars (void)
{
    int i;
    for (i = 0; i < 12; i++) {
	/* Initial population is 100 for empty board or 200 
	   for random villiage. */
	pbar_pop[i] = 100;
	pbar_tech[i] = 0;
	pbar_food[i] = 0;
	pbar_jobs[i] = 0;
	pbar_money[i] = 0;
	pbar_coal[i] = 0;
	pbar_goods[i] = 0;
	pbar_ore[i] = 0;
	pbar_steel[i] = 0;
    }
#ifdef SCREEN_SETUP_DRAWS
    pbars_full_refresh ();
#endif
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
    format_number5 (s, val);
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

inline int 
pbar_adjust_money (int diff)
{
    return diff > 0 ? diff / 2 + 1 : diff;
}

void 
refresh_pbars (void)
{
    Rect * b;
    char s[10];

    /* Population */
    b = &scr.pbar_pop;
    draw_pbar_new (b, pbar_pop[11], pbar_adjust_pop(pop_diff), 
		   pbar_adjust_pop(pbar_pop_olddiff));
    write_pbar_int (b, pbar_pop[11]);

    /* Technology */
    b = &scr.pbar_tech;
    draw_pbar_new (b, pbar_tech[11], pbar_adjust_tech(tech_diff), 
		   pbar_adjust_tech(pbar_tech_olddiff));
    snprintf (s, 10, "%5.1f", (float) pbar_tech[11] * 100.0 / MAX_TECH_LEVEL);
    write_pbar_text (b, s);

    /* Food */
    b = &scr.pbar_food;
    draw_pbar_new (b, pbar_food[11], pbar_adjust_food(food_diff),
		   pbar_adjust_food(pbar_food_olddiff));
    write_pbar_int (b, pbar_food[11]);

    /* Jobs */
    b = &scr.pbar_jobs;
    draw_pbar_new (b, pbar_jobs[11], pbar_adjust_jobs(jobs_diff),
		   pbar_adjust_jobs(pbar_jobs_olddiff));
    write_pbar_int (b, pbar_jobs[11]);

    /* Coal */
    b = &scr.pbar_coal;
    draw_pbar_new (b, pbar_coal[11], pbar_adjust_coal(coal_diff),
		   pbar_adjust_coal(pbar_coal_olddiff));
    write_pbar_int (b, pbar_coal[11]);

    /* Goods */
    b = &scr.pbar_goods;
    draw_pbar_new (b, pbar_goods[11], pbar_adjust_goods(goods_diff),
		   pbar_adjust_goods(pbar_goods_olddiff));
    write_pbar_int (b, pbar_goods[11]);

    /* Ore */
    b = &scr.pbar_ore;
    draw_pbar_new (b, pbar_ore[11], pbar_adjust_ore(ore_diff),
		   pbar_adjust_ore(pbar_ore_olddiff));
    write_pbar_int (b, pbar_ore[11]);

    /* Steel */
    b = &scr.pbar_steel;
    draw_pbar_new (b, pbar_steel[11], pbar_adjust_steel(steel_diff),
		   pbar_adjust_steel(pbar_steel_olddiff));
    write_pbar_int (b, pbar_steel[11]);

    /* Money */
    b = &scr.pbar_money;
    draw_pbar_new (b, pbar_money[11], pbar_adjust_money(money_diff),
		   pbar_adjust_money(pbar_money_olddiff));
    write_pbar_int (b, total_money);
}

/* XXX: WCK: I hate redundant code!  This could be made generic */

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


int 
compute_pbar_offset (Rect* b, int val)
{
    int offset;
    int val_abs = val > 0 ? val : -val;

    offset = (int) sqrt (val_abs);
    if (offset > (b->w / 2) - 8) {
	offset = (b->w / 2) - 8;
    }
    offset = val > 0 ? offset : -offset;
    //    printf("cpbar2o: %d\n",offset);
    return offset;
}


void
draw_pbar_new (Rect* b, int total, int val, int oldval)
{

    int offset; //, oldoffset;
    int spike_start, spike_end;

    /* offset, oldoffset are the size of spike in pixels */
    offset = compute_pbar_offset (b, val);

    /* Figure out pos/neg and length, draw, and clear other side */
    if (offset > 0) {
      /* Right */
      spike_start = b->x + (b->w / 2) + 8;
      spike_end = spike_start + offset;
      Fgl_fillbox (spike_start, b->y+2, spike_end - spike_start, b->h-4, 
		   (green(12)));
      Fgl_fillbox (b->x, b->y, b->w / 2 - 8, b->h, 0);
    } else if (offset < 0) {
      /* Left */
      spike_end = b->x + (b->w / 2) - 8;
      spike_start = spike_end + offset;
      Fgl_fillbox (spike_start, b->y+2, spike_end - spike_start, b->h-4, 
		   (red(12))); 
      Fgl_fillbox (b->x + (b->w / 2) + 8, b->y, b->w / 2 - 8, b->h, 0);
    } else { /* 0: Clear both sides */
      Fgl_fillbox (b->x + (b->w / 2) + 8, b->y, b->w / 2 - 8, b->h, 0);
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
