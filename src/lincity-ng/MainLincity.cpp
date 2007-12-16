/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <config.h>

#include "MainLincity.hpp"

#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <stdio.h>

#include "lincity/simulate.h"
#include "lincity/lin-city.h"
#include "lincity/lc_locale.h"
#include "lincity/fileutil.h"
#include "lincity/ldsvguts.h"

#include "gui_interface/screen_interface.h"
#include "gui_interface/mps.h"
#include "gui_interface/shared_globals.h"

#include "TimerInterface.hpp"

#include "GameView.hpp"
#include "ScreenInterface.hpp"
#include "Dialog.hpp"
#include "Config.hpp"

extern void print_total_money(void);
extern void init_types(void);

int lincitySpeed = MED_TIME_FOR_YEAR;
/******************************************/

void setLincitySpeed( int speed )
{
    lincitySpeed = speed;
}

void execute_timestep ()
{
  static Uint32 oldTime = SDL_GetTicks();
  Uint32 now=SDL_GetTicks();
  Uint32 mStepTime=( lincitySpeed *1000/NUMOF_DAYS_IN_YEAR); 

  if( lincitySpeed == 0 || blockingDialogIsOpen
        || ( (now - oldTime < (mStepTime-10)) && (lincitySpeed != FAST_TIME_FOR_YEAR)) ) {
      SDL_Delay(10); //don't burn cpu in active loop
      return;
  }
  if ( (now - oldTime < mStepTime) && lincitySpeed != FAST_TIME_FOR_YEAR  )
    return; // skip frame
  oldTime = now;

  //  TRACE;
  do_time_step();

  //draw the updated city
  //in FAST-Mode, update at the last day in Month, so print_stats will work.
  if( ( lincitySpeed != FAST_TIME_FOR_YEAR ) || 
          ( total_time % ( NUMOF_DAYS_IN_MONTH * getConfig()->skipMonthsFast ) == (NUMOF_DAYS_IN_MONTH - 1) ) ){
    //update_main_screen (0); //does nothing in NG
    print_stats ();
    updateDate();
    print_total_money();
    getGameView()->requestRedraw();
  }
}

/*
 * get Data form Lincity NG and Save City
 */
void saveCityNG( std::string newFilename ){
    GameView* gv = getGameView();
    if( gv ){ gv->writeOrigin(); }
    save_city(const_cast<char*>( newFilename.c_str() ) );
}

/*
 * Load City and do setup for Lincity NG.
 */
bool loadCityNG( std::string filename ){
    if( file_exists( const_cast<char*>( filename.c_str()) ) ){
        if (false)
            load_city(const_cast<char*>(filename.c_str()));
        else
            load_city_2(const_cast<char*>(filename.c_str()));
        update_avail_modules(0);
        GameView* gv = getGameView();
        if( gv ){ gv->readOrigin(); }
        return true;
    }
    return false;
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

    initialize_monthgraph ();
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

    //load current game if it exists
    //ldsvguts.cpp load_saved_city (char *s)
    //does not work if file is missing...

    char* s = "9_currentGameNG.scn";
    char *cname = (char *) malloc (strlen (lc_save_dir) + strlen (s) + 2);
    sprintf (cname, "%s%c%s", lc_save_dir, PATH_SLASH, s);
    if( ! loadCityNG( std::string( cname ) ) ) {   
        //create a new City with village just in case 
        new_city( &main_screen_originx, &main_screen_originy, 1 );
    }
    free (cname);
}


void doLincityStep()
{
  /* Get timestamp for this iteration */
  get_real_time();
  
  execute_timestep ();
  
  //  return true;
}
