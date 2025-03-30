/* ---------------------------------------------------------------------- *
 * src/lincity-ng/MainLincity.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      David Kamphausen <david.kamphausen@web.de>
 * Copyright (C) 2025      David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "MainLincity.hpp"

#include <stdlib.h>                          // for srand
#include <time.h>                            // for time
#include <cassert>                           // for assert
#include <iostream>                          // for char_traits, basic_ostream
#include <stdexcept>                         // for runtime_error

#include "Config.hpp"                        // for getConfig, Config
#include "Game.hpp"                          // for getGame
#include "GameView.hpp"                      // for getGameView, GameView
#include "TimerInterface.hpp"                // for reset_start_time
#include "gui/DialogBuilder.hpp"             // for DialogBuilder
#include "gui_interface/screen_interface.h"  // for initialize_monthgraph
#include "gui_interface/shared_globals.h"    // for update_avail_modules
#include "lincity/lc_locale.h"               // for lincity_set_locale
#include "lincity/lin-city.h"                // for SIM_DELAY_SLOW
#include "lincity/modules/all_modules.h"     // for initializeModules
#include "lincity/xmlloadsave.h"             // for loadGame, saveGame

extern void init_types(void);
extern void initFactories(void);

int simDelay = SIM_DELAY_SLOW;
/******************************************/

void setSimulationDelay( int speed )
{
    simDelay = speed;
}

/*
 * get Data form Lincity NG and Save City
 */
void saveCityNG(const World& world, const std::filesystem::path& filename) {
  std::filesystem::path fullname = filename;
  try {
    world.save(fullname);
    std::cout << "saved game to '" << fullname << "'" << std::endl;
  } catch(std::runtime_error err) {
    std::cerr << "error: failed to save game to '" << fullname << "': "
      << err.what() << std::endl;
    assert(false);
  }
}

/*
 * Load City and do setup for Lincity NG.
 */
std::unique_ptr<World> loadCityNG(const std::filesystem::path& filename) {
  std::unique_ptr<World> world;
  try {
    world = World::load(filename);
    std::cout << "loaded game from " << filename << std::endl;
  } catch(std::runtime_error& err) {
    std::cerr << "error: failed to load game from '" << filename
      << "': " << err.what() << std::endl;
    DialogBuilder()
      .titleText("Error!")
      .messageAddTextBold("Error: Failed to load game.")
      .messageAddText(std::string("Could not load '") + filename.string() +
        "'.")
      .messageAddText(err.what())
      .imageFile("images/gui/dialogs/error.png")
      .buttonSet(DialogBuilder::ButtonSet::OK)
      .build();
  }

  return world;
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

    //mps_init(); //CK no implemented

    // initialize constructions
    initializeModules();

    // animation time
    reset_start_time ();

}


/** @file lincity-ng/MainLincity.cpp */
