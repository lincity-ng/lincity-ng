#include <config.h>

#include "gui_interface/screen_interface.h"
#include "gui_interface/pbar_interface.h"

#include "lincity/engglobs.h"

short mappointoldtype[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
int selected_module_cost; // this must be changed, when module (or celltype-button) is changed


int getMainWindowWidth();
int getMainWindowHeight();


/* This is on in screen_full_refresh, used in *_refresh() */
char screen_refreshing;

char* current_month (int current_time);
void draw_cb_box (int row, int col, int checked);
int ask_launch_rocket_click (int x, int y);

int ask_launch_rocket_now (int x, int y)
{
  return 0;
}
void display_rocket_result_dialog (int result)
{
}
void draw_background (void);
void screen_full_refresh (void);
void init_fonts (void);
void initialize_monthgraph (void);
void rotate_mini_screen (void);
void advance_mps_style (void);
/*void update_main_screen (int full_refresh)
{
}*/
void connect_transport_main_screen (void);

void refresh_mps (void);
void advance_monthgraph_style (void);
void refresh_monthgraph (void);
void draw_small_yellow_bezel (int x, int y, int h, int w);
void init_pbars (void);
void mini_screen_help (void);
void status_message(char *m1, char* m2);
void print_time_for_year (void);
void rotate_main_screen (void);
void screen_setup (void);

/* Miniscreen */
void init_mini_map_mouse(void);
void mini_map_handler(int x, int y, int button);
void mini_aux_handler(int x, int y, int button);
/* Message area */
void display_info_message (int colour, char* ss, char* xs);
void reset_status_message (void);


 void ok_dial_box (char *, int, char *)
{
}

int dialog_box(int colour, int argc, ...)
{
  return 0;
}
void prog_box (char *, int)
{
}

void print_total_money (void)
{
}

void refresh_population_text (void)
{
  //  TRACE;
  update_pbar (PPOP, housed_population + people_pool, 0);

}


void update_avail_modules (int popup)
{
}



void refresh_main_screen()
{
}

void screen_full_refresh ()
{
  update_main_screen (true);
}

void print_stats ()
{
  // this show update the financy window or mps

  if (total_time % NUMOF_DAYS_IN_MONTH == (NUMOF_DAYS_IN_MONTH - 1))
    update_pbars_monthly();

}
void update_main_screen_normal (int full_refresh)
{
  return;
}


 void update_main_screen (int full_refresh)
{
  update_main_screen_normal (full_refresh);
}









void print_time_for_year (void)
{
}


int getMainWindowWidth()
{
  // FIXME: insert some function to return screen size
  return 800;
  //  return getScreen().getRect().w;
}
int getMainWindowHeight()
{
  // FIXME: insert some function to return screen size
  return 800;
  //  return getScreen().getRect().h;
}


int yn_dial_box (char *, char *, char *, char *)
{
  return 0;
}
