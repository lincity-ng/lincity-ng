/* ---------------------------------------------------------------------- *
 * geometry.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __geometry_h__
#define __geometry_h__

struct rect_struct
{
    int x;
    int y;
    int w;
    int h;
};
typedef struct rect_struct Rect;

struct screen_geometry_struct
{
    int border_x;
    int border_y;

    /* Main window */
    Rect main_win;

    /* Speed buttons */
    Rect pause_button;
    Rect slow_button;
    Rect med_button;
    Rect fast_button;

    /* Menu buttons */
    Rect menu_button;

    /* Load/Save buttons */
    Rect load_button;
    Rect save_button;
    Rect quit_button;
    Rect help_button;

    /* Misc buttons */
    Rect results_button;
    Rect tover_button;
    Rect confine_button;

    /* Icon palette (a.k.a. "select buttons") */
    Rect select_buttons;

    /* Progress bars */
    Rect pbar_area;
    Rect pbar_pop;
    Rect pbar_tech;
    Rect pbar_food;
    Rect pbar_jobs;
    Rect pbar_money;
    Rect pbar_coal;
    Rect pbar_goods;
    Rect pbar_ore;
    Rect pbar_steel;

    /* Info screens */
    Rect sust;

    /* Strings */
    Rect select_message;
    Rect date;
    Rect time_for_year;
    Rect status_message;

    /* Mini map */
    Rect mini_map;
    Rect mini_map_aux;
    Rect mini_map_area;    /* contains both mini_map and mini_map_aux */
#if defined (commentout)
    Rect ms_normal_button;
    Rect ms_pollution_button;
    Rect ms_fire_cover_button;
    Rect ms_ub40_button;
    Rect ms_coal_button;
    Rect ms_health_cover_button;
    Rect ms_cricket_cover_button;
    Rect ms_power_button;
    Rect ms_starve_button;
    Rect ms_ocost_button;
#endif

    /* Statistics */
    Rect monthgraph;
    Rect mappoint_stats;

    /* Market CB */
    Rect market_cb;
};
typedef struct screen_geometry_struct Screen_Geometry;



void initialize_geometry (Screen_Geometry* scr);
void resize_geometry (int new_width, int new_height);
int mouse_in_rect (Rect* b, int x, int y);
int pixel_to_mappoint(int px, int py, int *mpx, int *mpy);
int pixel_to_winpoint(int px, int py, int *wpx, int *wpy);
void adjust_main_origin (int new_origin_x, int new_origin_y, int refresh);
void draw_pause (int active);
void draw_slow (int active);
void draw_med (int active);
void draw_fast (int active);
void draw_menu (void);
void draw_load (void);
void draw_save (void);
void draw_quit (void);
void draw_help (void);
void draw_results (void);
#if defined (commentout)
void draw_tover (int active);
#if defined (LC_X11)
void draw_confine (int active);
#endif
#endif
void draw_select_button_graphic (int button, char *graphic);
void select_fast (void);
void select_medium (void);
void select_slow (void);
void select_pause (void);
#if defined (commentout)
void select_tover (void);
#if defined (LC_X11)
void select_confine (void);
#endif
#endif
#ifdef commentout /* WCK: now in pbar.h */
void init_pbar_text (void);
void draw_pbar_pop (void);
void draw_pbar_tech (void);
void draw_pbar_food (void);
void draw_pbar_jobs (void);
void draw_pbar_money (void);
void draw_pbar_coal (void);
void draw_pbar_goods (void);
void draw_pbar_ore (void);
void draw_pbar_steel (void);
void update_pbar_pop (int pop);
void update_pbar_tech (int tech);
void update_pbar_food (int food);
void update_pbar_jobs (int jobs);
void update_pbar_coal (int coal);
void update_pbar_goods (int goods);
void update_pbar_ore (int ore);
void update_pbar_steel (int steel);
void update_pbar_money (int money);
void refresh_pbars (void);
#endif /* commentout */
void draw_ms_button (char* graphic);
void draw_ms_text (char* txt);
void draw_small_bezel (int x, int y, int w, int h, int colour);

/* Main window */
#define MAIN_WIN_W 432
#define MAIN_WIN_X 56+8+1
#define MAIN_WIN_Y 8
#define MAIN_WIN_H 400

/* Speed buttons */
#define SPEED_BUTTONS_X 0
#define SPEED_BUTTONS_Y 416
#define SPEED_BUTTONS_H 16
#define SPEED_BUTTONS_W 32
#define PAUSE_BUTTON_X SPEED_BUTTONS_X
#define PAUSE_BUTTON_Y SPEED_BUTTONS_Y
#define PAUSE_BUTTON_H 16
#define PAUSE_BUTTON_W 32
#define SLOW_BUTTON_X SPEED_BUTTONS_X
#define SLOW_BUTTON_Y SPEED_BUTTONS_Y + SPEED_BUTTONS_H
#define SLOW_BUTTON_H 16
#define SLOW_BUTTON_W 32
#define MED_BUTTON_X SPEED_BUTTONS_X
#define MED_BUTTON_Y SPEED_BUTTONS_Y + 2 * SPEED_BUTTONS_H
#define MED_BUTTON_H 16
#define MED_BUTTON_W 32
#define FAST_BUTTON_X SPEED_BUTTONS_X
#define FAST_BUTTON_Y SPEED_BUTTONS_Y + 3 * SPEED_BUTTONS_H
#define FAST_BUTTON_H 16
#define FAST_BUTTON_W 32

#define HELP_BUTTON_X 608
#define HELP_BUTTON_Y 448
#define HELP_BUTTON_W 32
#define HELP_BUTTON_H 32
#define QUIT_BUTTON_X 608
#define QUIT_BUTTON_Y 416
#define QUIT_BUTTON_W 32
#define QUIT_BUTTON_H 32
#define LOAD_BUTTON_X 576
#define LOAD_BUTTON_Y 416
#define LOAD_BUTTON_W 32
#define LOAD_BUTTON_H 32
#define SAVE_BUTTON_X 576
#define SAVE_BUTTON_Y 448
#define SAVE_BUTTON_W 32
#define SAVE_BUTTON_H 32

/* Stats button */
#define RESULTS_BUTTON_X 640 - 32
#define RESULTS_BUTTON_Y 480 - 16
#define RESULTS_BUTTON_W 32
#define RESULTS_BUTTON_H 16

/* Misc buttons */
#define MISC_BUTTONS_X 0
#define MISC_BUTTONS_Y 400
#define TOVER_BUTTON_X MISC_BUTTONS_X + 32
#define TOVER_BUTTON_Y MISC_BUTTONS_Y
#define TOVER_BUTTON_W 32
#define TOVER_BUTTON_H 16
#define CONFINE_BUTTON_X MISC_BUTTONS_X + 64
#define CONFINE_BUTTON_Y MISC_BUTTONS_Y
#define CONFINE_BUTTON_W 16
#define CONFINE_BUTTON_H 16

#define MENU_BUTTON_X 0
#define MENU_BUTTON_Y 0
#define MENU_BUTTON_W 56
#define MENU_BUTTON_H 24

#define SELECT_BUTTON_WIN_X 0
#define SELECT_BUTTON_WIN_W 56
#define SELECT_BUTTON_WIN_Y 24
#define SELECT_BUTTON_WIN_H 392
#define SELECT_BUTTON_WIDTH 16
#define SELECT_BUTTON_DISTANCE 8
#define SELECT_BUTTON_INTERVAL (SELECT_BUTTON_WIDTH + SELECT_BUTTON_DISTANCE)

/* Progress bars */
//#define PBAR_W 16
//#define PBAR_H 56
#define PBAR_W 56
#define PBAR_H 16

#define PBAR_AREA_X     (56+MAIN_WIN_W+16+2)
#define PBAR_AREA_Y     0
#define PBAR_AREA_H     ((PBAR_H * 9) + 8 + 8)
#define PBAR_AREA_W     (640 - PBAR_AREA_X + 1)

#define PBAR_TEXT_W     (PBAR_AREA_W - 8 - PBAR_W)


#ifdef commentout

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
#endif /* commentout */


#define DATE_X                    100
#define DATE_Y                    470
#define TIME_FOR_YEAR_X           320
#define TIME_FOR_YEAR_Y           470
#define SELECT_BUTTON_MESSAGE_X   100
#define SELECT_BUTTON_MESSAGE_Y   460
#define STATUS_MESSAGE_X          100
#define STATUS_MESSAGE_Y          450

#define SUST_SCREEN_X 96
#define SUST_SCREEN_Y 416
#define SUST_SCREEN_W (60+8)
#define SUST_SCREEN_H 20

#if defined (MAPPOINT_STATS_X)
#undef MONTHGRAPH_X
#undef MONTHGRAPH_Y
#undef MAPPOINT_STATS_X
#undef MAPPOINT_STATS_Y
#undef MAPPOINT_STATS_W
#endif
#define MAPPOINT_STATS_X PBAR_AREA_X + 4
#define MAPPOINT_STATS_Y PBAR_AREA_Y + PBAR_AREA_H + 4
#define MAPPOINT_STATS_W PBAR_AREA_W - 8
#define MAPPOINT_STATS_H (11*8)

#define MONTHGRAPH_X PBAR_AREA_X + 4
#define MONTHGRAPH_Y MAPPOINT_STATS_Y + MAPPOINT_STATS_H + 8
/* MONTHGRAPH_W must match MAPPOINT_STATS_W as used in shrglobs.c */
#define MONTHGRAPH_W PBAR_AREA_W - 8
#define MONTHGRAPH_H 64

#define MINI_MAP_AUX_X PBAR_AREA_X + 4
#define MINI_MAP_AUX_Y MONTHGRAPH_Y + MONTHGRAPH_H + 8
#define MINI_MAP_AUX_W PBAR_AREA_W - 8
#define MINI_MAP_AUX_H 16

#define MINI_SCREEN_W WORLD_SIDE_LEN
#define MINI_SCREEN_H WORLD_SIDE_LEN
#define MINI_SCREEN_X MINI_MAP_AUX_X + ((MINI_MAP_AUX_W - MINI_SCREEN_W) / 2)
#define MINI_SCREEN_Y MINI_MAP_AUX_Y + MINI_MAP_AUX_H + 2

#define MINI_MAP_AREA_X PBAR_AREA_X + 4
#define MINI_MAP_AREA_Y MINI_MAP_AUX_Y
#define MINI_MAP_AREA_W PBAR_AREA_W - 8
#define MINI_MAP_AREA_H MINI_SCREEN_H + MINI_MAP_AUX_H + 2

#define MS_BUTTON_H 16
#define MS_BUTTON_W 16
#define MS_NORMAL_BUTTON_X (MINI_SCREEN_X + MINI_SCREEN_W)
#define MS_NORMAL_BUTTON_Y (MINI_SCREEN_Y)
#define MS_POLLUTION_BUTTON_X (MINI_SCREEN_X + MINI_SCREEN_W)
#define MS_POLLUTION_BUTTON_Y (MINI_SCREEN_Y + 1*16)
#define MS_FIRE_COVER_BUTTON_X (MINI_SCREEN_X + MINI_SCREEN_W)
#define MS_FIRE_COVER_BUTTON_Y (MINI_SCREEN_Y + 2*16)
#define MS_UB40_BUTTON_X (MINI_SCREEN_X + MINI_SCREEN_W)
#define MS_UB40_BUTTON_Y (MINI_SCREEN_Y + 3*16)
#define MS_HEALTH_COVER_BUTTON_X (MINI_SCREEN_X + MINI_SCREEN_W)
#define MS_HEALTH_COVER_BUTTON_Y (MINI_SCREEN_Y + 4*16)
#define MS_COAL_BUTTON_X (MINI_SCREEN_X + MINI_SCREEN_W)
#define MS_COAL_BUTTON_Y (MINI_SCREEN_Y + 5*16)
#define MS_CRICKET_COVER_BUTTON_X (MINI_SCREEN_X + MINI_SCREEN_W + 1*16)
#define MS_CRICKET_COVER_BUTTON_Y (MINI_SCREEN_Y + 1*16)
#define MS_POWER_BUTTON_X (MINI_SCREEN_X + MINI_SCREEN_W + 1*16)
#define MS_POWER_BUTTON_Y (MINI_SCREEN_Y + 2*16)
#define MS_STARVE_BUTTON_X (MINI_SCREEN_X + MINI_SCREEN_W + 1*16)
#define MS_STARVE_BUTTON_Y (MINI_SCREEN_Y + 3*16)
#define MS_OCOST_BUTTON_X (MINI_SCREEN_X + MINI_SCREEN_W + 1*16)
#define MS_OCOST_BUTTON_Y (MINI_SCREEN_Y + 4*16)

#define MARKET_CB_X (PBAR_AREA_X)
#define MARKET_CB_Y (PBAR_AREA_Y)

//#define MARKET_CB_H (23*8)
//#define MARKET_CB_W (18*8)

#endif /* __geometry_h__ */
