/* ---------------------------------------------------------------------- *
 * cliglobs.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "cliglobs.h"
#include "common.h"
#include "geometry.h"
#include "lcintl.h"

Screen_Geometry scr;
short mappointoldtype[WORLD_SIDE_LEN][WORLD_SIDE_LEN];

int lc_mouse_type;
char progbox[(PROGBOXW + 16) * (PROGBOXH + 16)];

int no_init_help;

int select_button_type[NUMOF_SELECT_BUTTONS];
char *select_button_graphic[NUMOF_SELECT_BUTTONS];
int select_button_tflag[NUMOF_SELECT_BUTTONS];
char select_button_help[NUMOF_SELECT_BUTTONS][20];

int main_screen_flag = MAIN_SCREEN_NORMAL_FLAG;

int mappoint_stats_flag = 0, mappoint_stats_time = 0;
int mini_screen_flags = MINI_SCREEN_NORMAL_FLAG;
int mini_screen_time = 0;
int mini_screen_port_x, mini_screen_port_y;

float gamma_correct_red = 0.0, gamma_correct_green = 0.0, gamma_correct_blue = 0.0;

int mouse_initialized = 0;
int cs_mouse_x, cs_mouse_y, cs_mouse_button;	/* current mouse status */
int cs_mouse_shifted = 0;	/* shift key pressed with mouse. */
int cs_mouse_xmax, cs_mouse_ymax, omx, omy, mox = 10, moy = 10;

int mouse_hide_count;
#if defined (WIN32)
/* int cs_current_mouse_x, cs_current_mouse_y, cs_current_mouse_button; */
int cs_square_mouse_visible = 0;
#endif
int kmousex, kmousey, kmouse_val, reset_mouse_flag, mt_flag = 0;
int overwrite_transport_flag = 0;

char under_square_mouse_pointer_top[20 * 2 * 4];
char under_square_mouse_pointer_left[18 * 2 * 4];
char under_square_mouse_pointer_right[18 * 2 * 4];
char under_square_mouse_pointer_bottom[20 * 2 * 4];
int mouse_type = MOUSE_TYPE_NORMAL;

int mt_start_posx, mt_start_posy, mt_current_posx, mt_current_posy;

int screen_refresh_flag = 0;

char *months[] =
{N_("Jan"), N_("Feb"), N_("Mar"), N_("Apr"),
N_("May"), N_("Jun"), N_("Jul"), N_("Aug"),
N_("Sep"), N_("Oct"), N_("Nov"), N_("Dec")};

char *up_pbar1_graphic, *up_pbar2_graphic;
char *down_pbar1_graphic, *down_pbar2_graphic, *pop_pbar_graphic;
char *tech_pbar_graphic, *food_pbar_graphic, *jobs_pbar_graphic;
char *money_pbar_graphic, *coal_pbar_graphic, *goods_pbar_graphic;
char *ore_pbar_graphic, *steel_pbar_graphic;
char *pause_button1_off, *pause_button2_off;
char *pause_button1_on, *pause_button2_on;
char *fast_button1_off, *fast_button2_off;
char *fast_button1_on, *fast_button2_on;
char *med_button1_off, *med_button2_off;
char *med_button1_on, *med_button2_on;
char *slow_button1_off, *slow_button2_off;
char *slow_button1_on, *slow_button2_on;
char *results_button1, *results_button2;
char *toveron_button1, *toveron_button2;
char *toveroff_button1, *toveroff_button2;
#ifdef LC_X11
char *confine_button, *unconfine_button;
int confine_flag = 0;
#endif
int pause_flag = 0, slow_flag = 0, med_flag = 0, fast_flag = 0;

char *ms_pollution_button_graphic, *ms_normal_button_graphic;
char *ms_fire_cover_button_graphic, *ms_health_cover_button_graphic;
char *ms_cricket_cover_button_graphic;
char *ms_ub40_button_graphic, *ms_coal_button_graphic;
char *ms_starve_button_graphic, *ms_ocost_button_graphic;
char *ms_power_button_graphic;
char *checked_box_graphic, *unchecked_box_graphic;

char market_cb_gbuf[MARKET_CB_W * MARKET_CB_H];
int market_cb_flag = 0, mcbx, mcby, market_cb_drawn_flag = 0;
int port_cb_flag = 0, port_cb_drawn_flag = 0;

/* GCS -- this one is difficult to deal with.
   LinCity will give your money back if you bulldoze
   the most recently constructed building (i.e. if 
   you make a mistake.  How does this fit into a 
   multiplayer game?  Not at all, I suppose... */

int help_flag, numof_help_buttons, help_history_count, help_return_val;
int block_help_exit;
int help_button_x[MAX_NUMOF_HELP_BUTTONS];
int help_button_y[MAX_NUMOF_HELP_BUTTONS];
int help_button_w[MAX_NUMOF_HELP_BUTTONS];
int help_button_h[MAX_NUMOF_HELP_BUTTONS];
char help_button_s[MAX_NUMOF_HELP_BUTTONS][MAX_LENOF_HELP_FILENAME];
char help_button_history[MAX_HELP_HISTORY][MAX_LENOF_HELP_FILENAME];
char *help_button_graphic, help_graphic[MAX_ICON_LEN];
char *quit_button_graphic, *load_button_graphic, *save_button_graphic;
char *menu_button_graphic;
int quit_flag, network_flag, load_flag, save_flag;
int prefs_flag = 0, prefs_drawn_flag = 0;
int monument_bul_flag, river_bul_flag;
int must_release_button = 0, let_one_through = 0;

int db_yesbox_x1, db_yesbox_x2, db_yesbox_y1, db_yesbox_y2;
int db_nobox_x1, db_nobox_x2, db_nobox_y1, db_nobox_y2;
int db_flag, db_yesbox_clicked, db_nobox_clicked;
char okmessage[22][74];
int db_okbox_x1, db_okbox_x2, db_okbox_y1, db_okbox_y2, db_okflag, db_okbox_clicked;
int suppress_ok_buttons;


#ifdef LC_X11
int winX, winY, mouse_button;
/* unsigned int winW, winH; */  /* GCS: Moved to display structure */
disp display;

char *bg_color = NULL;
char dummy1[1024];
char dummy2[1024];
int verbose = FALSE;		/* display settings if TRUE */
int stay_in_front = FALSE;	/* Try to stay in clear area of the screen. */
int text_bg = 0;
int text_fg = 255;
int x_key_value;
int x_key_shifted = 0;	/* Is the key shifted? */
int xclip_x1, xclip_y1, xclip_x2, xclip_y2, clipping_flag = 0;
int borderx, bordery;
long unsigned int colour_table[256];
unsigned char *open_font;
int open_font_height, suppress_next_expose = 0;
Cursor pirate_cursor;

#ifdef USE_PIXMAPS
Pixmap icon_pixmap[NUM_OF_TYPES];
char icon_pixmap_flag[NUM_OF_TYPES];
#endif
#endif

#ifdef WIN32
int mouse_button;
disp display;
char *bg_color = NULL;
char dummy1[1024];
char dummy2[1024];
int verbose = FALSE;		/* display settings if TRUE */
int stay_in_front = FALSE;	/* Try to stay in clear area of the screen. */
int text_bg = 0;
int text_fg = 255;
int x_key_value = 0;		/* GCS: Add initialization value */
BOOL x_key_shifted = FALSE;	/* Is the key shifted? */
int xclip_x1, xclip_y1, xclip_x2, xclip_y2, clipping_flag = 0;
int borderx, bordery;
long unsigned int colour_table[256];
unsigned char *open_font;
int open_font_height, suppress_next_expose = 0;

#if defined (USE_PIXMAPS)
HBITMAP icon_pixmap[NUM_OF_TYPES];
char icon_pixmap_flag[NUM_OF_TYPES];
#endif
#endif /* WIN32 */
