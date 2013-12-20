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
#include "lincity/lin-city.h"
#include "lincity/simulate.h"
#include "lincity/lc_locale.h"
#include "lincity/fileutil.h"
#include "lincity/loadsave.h"
#include "lincity/modules/all_modules.h"

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
//FIXME only slows down ?
//extern void initFactories();

int lincitySpeed = MED_TIME_FOR_YEAR;
/******************************************/

void setLincitySpeed( int speed )
{
    lincitySpeed = speed;
}

void execute_timestep ()
{
    static int dontskip = 0;

    /* Get timestamp for this iteration */
    get_real_time();

    if( lincitySpeed == 0 || blockingDialogIsOpen ) {
        SDL_Delay(10); //don't burn cpu in active loop
        return;
    }

    // Do the simulation. Remember 1 month = 100 days, only the display fits real life :)
    do_time_step();

    //draw the updated city
    if ( lincitySpeed != fast_time_for_year) {
        SDL_Delay(lincitySpeed); // This is the limiting factor for speed

        print_stats();
        updateDate();
        print_total_money();
        getGameView()->requestRedraw();

    } else {
        /* SDL doc says to rely on at least 10 ms granurality on all OS without
         * real time ability (Windows, Linux, MacOS X...)
         * So, as we cannot wait 1ms, we just don't wait when we need speed.
         */

        //in FAST-Mode, update at the last day in Month, so print_stats will work.
        if( ( total_time % NUMOF_DAYS_IN_MONTH ) == NUMOF_DAYS_IN_MONTH - 1 ){
            print_stats ();
            updateDate();
            print_total_money();
        }
        if (dontskip++ == fast_time_for_year ) {
            // The point of fast mode is to be really fast. So skip frames for speed
            // fast_time_for_year is read from config file = parameter named "quickness"
            dontskip = 0;
            getGameView()->requestRedraw();
        }
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
    if( PHYSFS_isDirectory( filename.c_str() )){
        return false;
    }
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
    lincity_set_locale();

    /* Set up the paths to certain files and directories */
    init_path_strings();

    /* Make sure the save directory exists */
    check_savedir();

    /*initialize Desktop Componenet Factories*/
    //initFactories();

    /* Initialize random number generator */
    srand (time (0));

    initialize_monthgraph();
    mps_init();

    // initialize constructions
    initializeModules();

    // init_types ();
    load_png_graphics();
    main_types[CST_USED].group = GROUP_USED;
    main_types[CST_USED].graphic = 0;   /* Won't be dereferenced! */

    // animation time
    reset_start_time ();

}


/** @file lincity-ng/MainLincity.cpp */

