#include <stdio.h>
#include "lcstring.h"
#include "geometry.h"
#include "module_buttons.h"
#include "cliglobs.h"
#include "mouse.h"
#include "lctypes.h"

#define DEBUG_MODULES 1

static int module_rows;
static int module_cols;

static Mouse_Handle * mhandle;

static Rect * mbw = &scr.module_buttons;

int module_type[NUMOF_MODULES];
char *module_graphic[NUMOF_MODULES];
int module_tflag[NUMOF_MODULES];
char module_help[NUMOF_MODULES][20];

int module_help_flag[NUMOF_MODULES];

int sbut[NUMOF_MODULES];

int selected_module;             /* GCS:  Are this variable necessary? */
int old_selected_module = 0;

int selected_module_type;
int selected_module_group;
int selected_module_cost;



void
init_modules (void)
{
    int i;

    /* Get max number of rows for space allotted */
    module_rows = (mbw->h / (SELECT_BUTTON_INTERVAL));
    module_cols = (mbw->w / (SELECT_BUTTON_INTERVAL));

    /* Tell the mouse where we are */
    mhandle = mouse_register(&scr.module_buttons,&handle_module_buttons);

#ifdef DEBUG_MODULES
    printf("debug_modules:\t\tr=%d\tc=%d\n",module_rows, module_cols);
#endif

    /* sbut converts a group into a column major index of the button array. */
    sbut[0] = 16;		/* buldoze */
    sbut[1] = 13;		/* powerline */
    sbut[2] = 15;		/* solar power */
    sbut[3] = 14;		/* substation */
    sbut[4] = 0;		/* residence */
    sbut[5] = 1;		/* farm */
    sbut[6] = 2;		/* market */
    sbut[7] = 19;		/* track */
    sbut[8] = 10;		/* coalmine */
    sbut[9] = 28;		/* rail */
    sbut[10] = 29;		/* coal power */
    sbut[11] = 25;		/* road */
    sbut[12] = 27;		/* light industry */
    sbut[13] = 11;		/* university */
    sbut[14] = 3;		/* commune */
    sbut[15] = 4;		/* oremine */
    sbut[16] = 5;		/* tip */
    sbut[17] = 9;		/* export */
    sbut[18] = 12;		/* heavy industry */
    sbut[19] = 6;		/* parkland */
    sbut[20] = 30;		/* recycle */
    sbut[21] = 20;		/* water */
    sbut[22] = 26;		/* health */
    sbut[23] = 31;		/* rocket */
    sbut[24] = 24;		/* windmill */
    sbut[25] = 17;		/* monument */
    sbut[26] = 21;		/* school */
    sbut[27] = 22;		/* blacksmith */
    sbut[28] = 8;		/* mill */
    sbut[29] = 18;		/* pottery */
    sbut[30] = 23;		/* fire station */
    sbut[31] = 7;		/* cricket  */


    /* load graphics and initialize help */

    module_graphic[sbut[0]] = load_graphic ("buldoze-button.csi");
    module_type[sbut[0]] = CST_GREEN;
    strcpy (module_help[sbut[0]], "bulldoze.hlp");

    module_graphic[sbut[1]] = load_graphic ("powerline-button.csi");
    module_type[sbut[1]] = CST_POWERL_H_L;
    strcpy (module_help[sbut[1]], "powerline.hlp");

    module_graphic[sbut[2]] = load_graphic ("powerssolar-button.csi");
    module_type[sbut[2]] = CST_POWERS_SOLAR;
    strcpy (module_help[sbut[2]], "powerssolar.hlp");

    module_graphic[sbut[3]] = load_graphic ("substation-button.csi");
    module_type[sbut[3]] = CST_SUBSTATION_R;
    strcpy (module_help[sbut[3]], "substation.hlp");

    module_graphic[sbut[4]] = load_graphic ("residence-button.csi");
    module_type[sbut[4]] = CST_RESIDENCE_LL;
    strcpy (module_help[sbut[4]], "residential.hlp");

    module_graphic[sbut[5]] = load_graphic ("organic-farm-button.csi");
    module_type[sbut[5]] = CST_FARM_O0;
    strcpy (module_help[sbut[5]], "farm.hlp");

    module_graphic[sbut[6]] = load_graphic ("market-button.csi");
    module_type[sbut[6]] = CST_MARKET_EMPTY;
    strcpy (module_help[sbut[6]], "market.hlp");

    module_help_flag[sbut[7]] = 1;        /* No help for track */
    module_graphic[sbut[7]] = load_graphic ("track-button.csi");
    module_type[sbut[7]] = CST_TRACK_LR;
    strcpy (module_help[sbut[7]], "track.hlp");

    module_graphic[sbut[8]] = load_graphic ("coalmine-button.csi");
    module_type[sbut[8]] = CST_COALMINE_EMPTY;
    strcpy (module_help[sbut[8]], "coalmine.hlp");

    module_graphic[sbut[9]] = load_graphic ("rail-button.csi");
    module_type[sbut[9]] = CST_RAIL_LR;
    strcpy (module_help[sbut[9]], "rail.hlp");

    module_graphic[sbut[10]] = load_graphic ("powerscoal-button.csi");
    module_type[sbut[10]] = CST_POWERS_COAL_EMPTY;
    strcpy (module_help[sbut[10]], "powerscoal.hlp");

    module_graphic[sbut[11]] = load_graphic ("road-button.csi");
    module_type[sbut[11]] = CST_ROAD_LR;
    strcpy (module_help[sbut[11]], "road.hlp");

    module_graphic[sbut[12]] = load_graphic ("industryl-button.csi");
    module_type[sbut[12]] = CST_INDUSTRY_L_C;
    strcpy (module_help[sbut[12]], "industryl.hlp");

    module_graphic[sbut[13]] = load_graphic ("university-button.csi");
    module_type[sbut[13]] = CST_UNIVERSITY;
    strcpy (module_help[sbut[13]], "university.hlp");

    module_graphic[sbut[14]] = load_graphic ("commune-button.csi");
    module_type[sbut[14]] = CST_COMMUNE_1;
    strcpy (module_help[sbut[14]], "commune.hlp");

    module_graphic[sbut[15]] = load_graphic ("oremine-button.csi");
    module_type[sbut[15]] = CST_OREMINE_1;
    strcpy (module_help[sbut[15]], "oremine.hlp");

    module_graphic[sbut[16]] = load_graphic ("tip-button.csi");
    module_type[sbut[16]] = CST_TIP_0;
    strcpy (module_help[sbut[16]], "tip.hlp");

    module_graphic[sbut[17]] = load_graphic ("port-button.csi");
    module_type[sbut[17]] = CST_EX_PORT;
    strcpy (module_help[sbut[17]], "port.hlp");

    module_graphic[sbut[18]] = load_graphic ("industryh-button.csi");
    module_type[sbut[18]] = CST_INDUSTRY_H_C;
    strcpy (module_help[sbut[18]], "industryh.hlp");

    module_graphic[sbut[19]] = load_graphic ("parkland-button.csi");
    module_type[sbut[19]] = CST_PARKLAND_PLANE;
    strcpy (module_help[sbut[19]], "park.hlp");

    module_graphic[sbut[20]] = load_graphic ("recycle-button.csi");
    module_type[sbut[20]] = CST_RECYCLE;
    strcpy (module_help[sbut[20]], "recycle.hlp");

    module_graphic[sbut[21]] = load_graphic ("water-button.csi");
    module_type[sbut[21]] = CST_WATER;
    strcpy (module_help[sbut[21]], "river.hlp");

    module_graphic[sbut[22]] = load_graphic ("health-button.csi");
    module_type[sbut[22]] = CST_HEALTH;
    strcpy (module_help[sbut[22]], "health.hlp");

    module_graphic[sbut[23]] = load_graphic ("rocket-button.csi");
    module_type[sbut[23]] = CST_ROCKET_1;
    strcpy (module_help[sbut[23]], "rocket.hlp");

    module_graphic[sbut[24]] = load_graphic ("windmill-button.csi");
    module_type[sbut[24]] = CST_WINDMILL_1_R;
    strcpy (module_help[sbut[24]], "windmill.hlp");

    module_graphic[sbut[25]] = load_graphic ("monument-button.csi");
    module_type[sbut[25]] = CST_MONUMENT_0;
    strcpy (module_help[sbut[25]], "monument.hlp");

    module_graphic[sbut[26]] = load_graphic ("school-button.csi");
    module_type[sbut[26]] = CST_SCHOOL;
    strcpy (module_help[sbut[26]], "school.hlp");

    module_graphic[sbut[27]] = load_graphic ("blacksmith-button.csi");
    module_type[sbut[27]] = CST_BLACKSMITH_0;
    strcpy (module_help[sbut[27]], "blacksmith.hlp");

    module_graphic[sbut[28]] = load_graphic ("mill-button.csi");
    module_type[sbut[28]] = CST_MILL_0;
    strcpy (module_help[sbut[28]], "mill.hlp");

    module_graphic[sbut[29]] = load_graphic ("pottery-button.csi");
    module_type[sbut[29]] = CST_POTTERY_0;
    strcpy (module_help[sbut[29]], "pottery.hlp");

    module_graphic[sbut[30]] = load_graphic ("firestation-button.csi");
    module_type[sbut[30]] = CST_FIRESTATION_1;
    strcpy (module_help[sbut[30]], "firestation.hlp");

    module_graphic[sbut[31]] = load_graphic ("cricket-button.csi");
    module_type[sbut[31]] = CST_CRICKET_1;
    strcpy (module_help[sbut[31]], "cricket.hlp");



    /* disable all the buttons */
    for (i = 0; i < NUMOF_MODULES; i++) {
	module_tflag[i] = 0;
    }

    /* select track (which will be enabled later) */
#if defined (commentout)
    selected_module_type = CST_TRACK_LR;
    selected_module_cost = GROUP_TRACK_COST;
    selected_module_group = get_group_of_type(selected_module_type);
#endif
    old_selected_module = sbut[7];
    set_selected_module (CST_TRACK_LR);
}


/* update_avail_modules: Determine which modules are available, based on
   tech level */

void
update_avail_modules (void)
{
    int i;
    int f; /* remembers if a change is made to what is available, so we can
	      draw if necessary */

    for (i = 0; i < NUMOF_MODULES; i++)
    {
	int g = inv_sbut(i);
	f = module_tflag[i];
	if (tech_level >= main_groups[g].tech * MAX_TECH_LEVEL/1000)
	{
	    if (module_tflag[i] == 0)
		activate_module (i);
	    f = 1;
	}
	else if (module_tflag[i] != 0 &&
		 tech_level
                 < ((main_groups[g].tech - (main_groups[g].tech/10)) * MAX_TECH_LEVEL/1000) )
	    f = 0;
	if (module_tflag[i] != f)
	{
	    module_tflag[i] = f;
	    draw_module (i, module_graphic[i]);
	}
    }
    /* XXX: Why is this here? Should be with rest of tech gained messages! */
    if (tech_level > MODERN_WINDMILL_TECH && modern_windmill_flag == 0)
    {
	ok_dial_box ("mod_wind_up.mes", GOOD, 0L);
	modern_windmill_flag = 1;
    }
}

/* Display message for module when it is activated (see above) */

void
activate_module (int module)
{
    module = inv_sbut (module);

    if (module == GROUP_WINDMILL)
	ok_dial_box ("windmillup.mes", GOOD, 0L);
    else if (module == GROUP_COAL_POWER)
	ok_dial_box ("coalpowerup.mes", GOOD, 0L);
    else if (module == (GROUP_SOLAR_POWER - 1))
      /* XXX: */
	/* -1 a hack to make it work. Really dirty :( 
	   Caused by the fact that groups and buttons are different until 
	   after the bulldoze button, then they are the same.
	*/
	ok_dial_box ("solarpowerup.mes", GOOD, 0L);
    else if (module == GROUP_COALMINE)
	ok_dial_box ("coalmineup.mes", GOOD, 0L);
    else if (module == GROUP_RAIL)
	ok_dial_box ("railwayup.mes", GOOD, 0L);
    else if (module == GROUP_ROAD)
	ok_dial_box ("roadup.mes", GOOD, 0L);
    else if (module == GROUP_INDUSTRY_L)
	ok_dial_box ("ltindustryup.mes", GOOD, 0L);
    else if (module == GROUP_UNIVERSITY)
	ok_dial_box ("universityup.mes", GOOD, 0L);
    else if (module == GROUP_OREMINE)
    {
	if (GROUP_OREMINE_TECH > 0)
	    ok_dial_box ("oremineup.mes", GOOD, 0L);
    }
    else if (module == GROUP_PORT)	/* exports are the same */
	ok_dial_box ("import-exportup.mes", GOOD, 0L);
    else if (module == GROUP_INDUSTRY_H)
	ok_dial_box ("hvindustryup.mes", GOOD, 0L);
    else if (module == GROUP_PARKLAND)
    {
	if (GROUP_PARKLAND_TECH > 0)
	    ok_dial_box ("parkup.mes", GOOD, 0L);
    }
    else if (module == GROUP_RECYCLE)
	ok_dial_box ("recycleup.mes", GOOD, 0L);
    else if (module == GROUP_RIVER)
    {
	if (GROUP_WATER_TECH > 0)
	    ok_dial_box ("riverup.mes", GOOD, 0L);
    }
    else if (module == GROUP_HEALTH)
	ok_dial_box ("healthup.mes", GOOD, 0L);
    else if (module == GROUP_ROCKET)
	ok_dial_box ("rocketup.mes", GOOD, 0L);
    else if (module == GROUP_SCHOOL)
    {
	if (GROUP_SCHOOL_TECH > 0)
	    ok_dial_box ("schoolup.mes", GOOD, 0L);
    }
    else if (module == GROUP_BLACKSMITH)
    {
	if (GROUP_BLACKSMITH_TECH > 0)
	    ok_dial_box ("blacksmithup.mes", GOOD, 0L);
    }
    else if (module == GROUP_MILL)
    {
	if (GROUP_MILL_TECH > 0)
	    ok_dial_box ("millup.mes", GOOD, 0L);
    }
    else if (module == GROUP_POTTERY)
    {
	if (GROUP_POTTERY_TECH > 0)
	    ok_dial_box ("potteryup.mes", GOOD, 0L);
    }
    else if (module == GROUP_FIRESTATION)
	ok_dial_box ("firestationup.mes", GOOD, 0L);
    else if (module == GROUP_CRICKET)
	ok_dial_box ("cricketup.mes", GOOD, 0L);
}
    

/* handle_module_buttons:  mouse handler for module window */

void 
handle_module_buttons (int x, int y, int mbutton)
{
    int module;
    int row, col;

    /* Figure out which row and column the click came in */
    row = (y / (SELECT_BUTTON_INTERVAL));
    if ((y % SELECT_BUTTON_INTERVAL) <= 8) 
      return;  /* in the gap */
    
    col = (x / (SELECT_BUTTON_INTERVAL));
    if ((x % SELECT_BUTTON_INTERVAL) <= 8) 
      return;
    
    module = row + (module_rows * col);

    select_module (module, mbutton);
}


/* select_module: low level click handler;  display help or change to
   module, depending on mbutton (right or left click) */

void
select_module (int module, int mbutton)
{
    if (module_tflag[module] == 0 && mbutton != LC_MOUSE_RIGHTBUTTON) {
	ok_dial_box ("not_enough_tech.mes", BAD, 0L);
	return;
    }
    if (mbutton == LC_MOUSE_RIGHTBUTTON 
	|| module_help_flag[module] == 0)
    {
	activate_help (module_help[module]);
	if (mbutton != LC_MOUSE_RIGHTBUTTON)
	    module_help_flag[module] = 1;
	if (mbutton == LC_MOUSE_RIGHTBUTTON)
	    return;
    }

    unhighlight_module_button (old_selected_module);
    highlight_module_button (module);
    old_selected_module = module;

    set_selected_module (module_type[module]);

    /* GCS: Moved below for code reuse w/ setting residences. */
#if defined (commentout)
    selected_module_type = module_type[module];
    selected_module_group = get_group_of_type(selected_module_type);
    if (selected_module_type == CST_RESIDENCE_LL) {
	choose_residence ();
    }

#ifdef LC_X11  /* XXX: WCK: shouldn't be any platform specific code here */
    if (selected_module_group == GROUP_BARE) 
	XDefineCursor (display.dpy, display.win, pirate_cursor);
    else
	XDefineCursor (display.dpy, display.win, None);
#endif

    draw_selected_module_cost();

    if (selected_module_type == CST_GREEN) {
	draw_main_window_box (red (8));
    } else {
	draw_main_window_box (green (8));
	monument_bul_flag = 0;
	river_bul_flag = 0;
    }
#endif
}


/* highlight_module_button: draw a nice border around a selected module */

void
highlight_module_button (int module)
{
    int x, y, q;
    int row, col;

    if (module == 0) {
	row = 0;
	col = 0;
    } else {
	row = module % module_rows;
	col = module / module_rows;
    }

    x = 8 + (col * 24) + mbw->x;
    y = 8 + (row * 24) + mbw->y;

    hide_mouse ();
    draw_small_bezel(x + 1, y + 1,
		     SELECT_BUTTON_WIDTH - 2, SELECT_BUTTON_WIDTH - 2,
		     yellow(0));
    redraw_mouse ();
}


/* unhighlight_module_button: replace highlight border with regular one */

void
unhighlight_module_button (int module)
{
    int x, y, q;

    int row, col;

    if (module == 0) {
	row = 0;
	col = 0;
    } else {
	row = module % module_rows;
	col = module / module_rows;
    }

    x = 8 + (col * 24) + mbw->x;
    y = 8 + (row * 24) + mbw->y;

    hide_mouse ();
    draw_small_bezel(x + 1, y + 1, 
		     SELECT_BUTTON_WIDTH - 2, SELECT_BUTTON_WIDTH - 2,
		     blue(0));
    redraw_mouse ();
}


/* draw_selected_module_cost: write info about the module to the status area */

void
draw_selected_module_cost ()
{
  Rect* b = &scr.select_message;
  char s[100];

  selected_module_cost = get_type_cost (selected_module_type);

  if (selected_module_group == GROUP_BARE) 
    sprintf (s, "Bulldoze - cost varies");
  else
    sprintf (s, "%s %d  Bulldoze %d", main_groups[selected_module_group].name,
	    selected_module_cost, main_groups[selected_module_group].bul_cost);

  Fgl_fillbox (b->x, b->y, 42 * 8, 8, TEXT_BG_COLOUR);
  Fgl_write (b->x, b->y, s);
}


/* draw_modules:  fill a box and then draw all graphics */

void
draw_modules (void)
{
    int i;
    Fgl_fillbox (mbw->x, mbw->y, mbw->w, mbw->h, white (20));

    for (i = 0; i < NUMOF_MODULES; i++) 
      draw_module (sbut[i], module_graphic[sbut[i]]);

    highlight_module_button(old_selected_module);

}

void
draw_module (int module, char *graphic)
{
    int x, y, xx, yy;
    int row, col;
    
    if (module == 0) {
	row = 0;
	col = 0;
    } else {
	row = module % module_rows;
	col = module / module_rows;
    }

    x = 8 + (col * 24);
    y = 8 + (row * 24);

    hide_mouse ();

    Fgl_putbox (x + mbw->x, y + mbw->y, 16, 16, graphic);
    unhighlight_module_button (module);

    /* Modules we don't have enough tech for are greyed out. */
    if (module_tflag[module] == 0)
    {
	for (yy = -3; yy < 19; yy++)
	    for (xx = -3; xx < 19; xx += 2)
		Fgl_setpixel (x + xx + (yy % 2) + mbw->x,
			      y + yy + mbw->y, white (15));
    }
    redraw_mouse ();
}


/* inv_sbut:  the order and the number of the module aren't the same; 
   inv_sbut converts between a geographical location and an array location */

int
inv_sbut (int button)
{
    int i, j;
    for (i = 0; i < 32; i++)	
	if (sbut[i] == button)
	{
	    j = i;
	    return (j);
	}
    /* XXX: do we need this debug stuff? */
    /* GCS: This is a fatal error.  Probably OK. */
    printf ("Button=%d\n", button);
    for (i = 0; i < 32; i++)
	printf ("%5d", sbut[i]);
    printf ("\n");
    do_error ("An inv_sbut error has happened.");
    return (-1);			/* can't get here */
}

void
set_selected_module (int type)
{
    selected_module_type = type;
    selected_module_group = get_group_of_type(selected_module_type);
    if (selected_module_type == CST_RESIDENCE_LL) {
	choose_residence ();
    }

#ifdef LC_X11  /* XXX: WCK: shouldn't be any platform specific code here */
    if (selected_module_group == GROUP_BARE) 
	XDefineCursor (display.dpy, display.win, pirate_cursor);
    else
	XDefineCursor (display.dpy, display.win, None);
#endif

    draw_selected_module_cost();

    if (selected_module_type == CST_GREEN) {
	draw_main_window_box (red (8));
    } else {
	draw_main_window_box (green (8));
	monument_bul_flag = 0;
	river_bul_flag = 0;
    }


    selected_module_type = type;
    selected_module_group = get_group_of_type(selected_module_type);
    draw_selected_module_cost ();   /* sets module cost */
}
