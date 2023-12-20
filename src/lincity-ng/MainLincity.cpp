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

#include "lincity/engglobs.h"
#include "lincity/init_game.h"
#include "lincity/lin-city.h"
#include "lincity/simulate.h"
#include "lincity/lc_locale.h"
#include "lincity/loadsave.h"
#include "lincity/modules/all_modules.h"

#include "gui_interface/screen_interface.h"
#include "gui_interface/mps.h"
#include "gui_interface/shared_globals.h"
#include "gui_interface/readpng.h"

#include "TimerInterface.hpp"

#include "GameView.hpp"
#include "Game.hpp"
#include "ScreenInterface.hpp"
#include "Dialog.hpp"
#include "Config.hpp"

extern void init_types(void);
extern void initFactories(void);

int lincitySpeed = MED_TIME_FOR_YEAR;
/******************************************/

void setLincitySpeed( int speed )
{
    lincitySpeed = speed;
}

/*
 * get Data form Lincity NG and Save City
 */
void saveCityNG( std::string newFilename ){
    if (getGame())
    {
        GameView* gv = getGameView();
        if( gv ){ gv->writeOrigin(); }
        save_city(const_cast<char*>( newFilename.c_str() ) );
    }
}

/*
 * Load City and do setup for Lincity NG.
 */
bool loadCityNG( std::string filename ){
    // FIXME: Follow symlinks if able. symlink target may be directory
    // FIXME: What to do with PHYSFS_FILETYPE_OTHER? Should we instead make
    //        sure the filetype is PHYSFS_FILETYPE_REGULAR?
    PHYSFS_Stat statFilename;
    int errorCode = PHYSFS_stat(filename.c_str(), &statFilename);
    if(errorCode == 0) {
        std::cerr << "could not stat file: " << filename << std::endl;
    }
    else if(statFilename.filetype == PHYSFS_FILETYPE_DIRECTORY)
    {   return false;}


        const char* directory = PHYSFS_getRealDir(filename.c_str());
    if (directory)
    {
        //FIXME PHYSFS_getWriteDir() does not work for built in scenarios
        std::string dir = directory;//PHYSFS_getWriteDir();
        filename = dir + PHYSFS_getDirSeparator() + filename;
        FILE *fp;
        fclose(fp = fopen(filename.c_str(), "r"));
        if( fp )
        {
            load_city_2(const_cast<char*>(filename.c_str()));
            update_avail_modules(0);
            GameView* gv = getGameView();
            if( gv ){ gv->readOrigin(); }
            return true;
        }
        else
        {
            std::cout << "could not locate: " << filename << std::endl;
        }
    }
    else
    {
        std::cout << "could not find: " << filename << std::endl;
    }
    return false;
}

void initLincity()
{
    /* I18n */
    lincity_set_locale();

    /* Set up the paths to certain files and directories */
    // init_path_strings();

    /* Make sure the save directory exists */
    // check_savedir();

    /*initialize Desktop Componenet Factories*/
    initFactories();

    /* Initialize random number generator */
    srand (time (0));

    initialize_monthgraph();
    //mps_init(); //CK no implemented

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
