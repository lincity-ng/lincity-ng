#include <config.h>

#include "MainLincity.hpp"

#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "lincity/simulate.h"
#include "lincity/lin-city.h"
#include "lincity/lc_locale.h"
#include "lincity/fileutil.h"

#include "gui_interface/screen_interface.h"
#include "gui_interface/mps.h"

#include "TimerInterface.hpp"

#include "GameView.hpp"
#include "ScreenInterface.hpp"

int lincitySpeed = MED_TIME_FOR_YEAR;

void setLincitySpeed( int speed )
{
    lincitySpeed = speed;
}

void execute_timestep ()
{
  static Uint32 oldTime = SDL_GetTicks();
  Uint32 now=SDL_GetTicks();
  Uint32 mStepTime=( lincitySpeed *1000/NUMOF_DAYS_IN_YEAR); 

  if( lincitySpeed == 0 )
      return; // Pause

  if (now - oldTime < mStepTime)
    return; // skip frame
  oldTime = now;

  //  TRACE;
  do_time_step();

  update_main_screen (0);

  /* XXX: Shouldn't the rest be handled in update_main_screen()? */
  /* GCS: No, I don't think so.  These remaining items are
     outside of the main screen */

  print_stats ();
  updateMessageTitle();//show new Date

  //GameView has to draw the updated city
  getGameView()->requestRedraw();
  
}


void initLCengine()
{
  /* I18n */
  lincity_set_locale ();

  /* Set up the paths to certain files and directories */
  init_path_strings ();

  /* Make sure that things are installed where they should be */
  //  verify_package ();

  /* Make sure the save directory exists */
  check_savedir ();

  /* Load preferences */
  load_lincityrc ();

  /* Initialize random number generator */
  srand (time (0));

  /* Save preferences */
    save_lincityrc ();

    //    init_fonts ();

    //    initialize_monthgraph ();
    //    init_mouse_registry ();
    //    init_mini_map_mouse ();
    mps_init();




    //    setcustompalette ();
    //    draw_background ();
    //    prog_box (_("Loading the game"), 1);
    init_types ();
    // means "init buttons"    init_modules();
    init_mappoint_array ();
    initialize_tax_rates ();
}


void initLincity()
{
  initLCengine();

  reset_start_time ();
  
  screen_full_refresh ();
  
  
  load_city("data/opening/good_times.scn");
  
}


void doLincityStep()
{
  /* Get timestamp for this iteration */
  get_real_time();
  
  execute_timestep ();
  
  //  return true;
}
