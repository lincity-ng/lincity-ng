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
#include <physfs.h>

#include "lincity/init_game.h"
#include "lincity/simulate.h"
#include "lincity/lin-city.h"
#include "lincity/lc_locale.h"
#include "lincity/fileutil.h"
#include "lincity/loadsave.h"

#include "gui_interface/screen_interface.h"
#include "gui_interface/mps.h"
#include "gui_interface/shared_globals.h"
#include "gui_interface/readpng.h"

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

    if( lincitySpeed == 0 || blockingDialogIsOpen ) {
        SDL_Delay(10); //don't burn cpu in active loop
        return;
    } else if ( lincitySpeed == fast_time_for_year) {
        if ( (total_time % (10 - fast_time_for_year)) == 0 ) {
            SDL_Delay(10);
            /* fast = 1   => wait once for each 9 loop     => 1.3 real second / game year
             *               beware it can warm hardware (nearly always active).
             *
             * fast = 9 = default  => wait at each step  => 12 real seconds / game year
             *          = nearly old behavior, except we no more skeep frames.
             *
             * On athlon-xp 2200+ (1600MHz) 750 MB + Nvidia geforce 420 MX (16 MB)
             * this is the limiting factor for max_speed
             * Removing it gives approximately the same speed as old-ng = 4.0 s/year
             * instead of 24s/year with delay 10 (default fast = 9)
             *
             * SDL doc says to rely on at least 10 ms granurality on all OS without
             * real time ability (Windows, Linux, MacOS X...) hence the trick
             * of waiting 1/n loop.
             */
        }
    } else
        SDL_Delay(lincitySpeed);

    // Do the simulation. Remember 1 month = 100 days, only the display fits real life :)
    do_time_step();

    //draw the updated city
    //in FAST-Mode, update at the last day in Month, so print_stats will work.
    if( ( lincitySpeed != fast_time_for_year ) ||
        ( total_time % ( NUMOF_DAYS_IN_MONTH * getConfig()->skipMonthsFast ) ) == NUMOF_DAYS_IN_MONTH - 1 ){
        print_stats ();
        updateDate();
        print_total_money();
        getGameView()->requestRedraw();
    } else if (fast_time_for_year != FAST_TIME_FOR_YEAR) { // The point of fast mode is to be really fast. So skip frames for speed by default.
        getGameView()->requestRedraw();                    // Users with fast machines who prefer nice animations in fast mode can set fast speed manually to get them back.
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
    /* TODO use PhysFS directly to load file instead of getRealDir hack */
    const char* directory = PHYSFS_getRealDir(filename.c_str());
    if( !directory ){
        return false;
    }
    std::string dir = directory;
    filename = dir + PHYSFS_getDirSeparator() + filename;
    if( file_exists( const_cast<char*>( filename.c_str()) ) ){
        load_city_2(const_cast<char*>(filename.c_str()));
        update_avail_modules(0);
        GameView* gv = getGameView();
        if( gv ){ gv->readOrigin(); }
        return true;
    }
    return false;
}

void initLincity()
{
    /* I18n */
    lincity_set_locale ();

    /* Set up the paths to certain files and directories */
    init_path_strings ();

    /* Make sure the save directory exists */
    check_savedir ();

    /* Initialize random number generator */
    srand (time (0));

    initialize_monthgraph ();
    mps_init();

    // init_types ();
    load_png_graphics();

    main_types[CST_USED].group = GROUP_USED;
    main_types[CST_USED].graphic = 0;   /* Won't be dereferenced! */

    // animation time
    reset_start_time ();

    screen_full_refresh ();

    //load current game if it exists
    if( ! loadCityNG( std::string( "9_currentGameNG.scn" ) ) ) {
        //create a new City with village just in case
        new_city( &main_screen_originx, &main_screen_originy, 1 );
    }
}


void doLincityStep()
{
  /* Get timestamp for this iteration */
  get_real_time();

  execute_timestep ();

  //  return true;
}
