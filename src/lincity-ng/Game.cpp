/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>

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

#include "Game.hpp"

#include <SDL.h>                           // for Uint32, SDL_GetTicks, SDL_...
#include <physfs.h>                        // for PHYSFS_enumerateFiles, PHY...
#include <stddef.h>                        // for NULL, size_t
#include <algorithm>                       // for min
#include <iostream>                        // for basic_ostream, operator<<
#include <stdexcept>                       // for runtime_error

#include "ButtonPanel.hpp"                 // for getButtonPanel, ButtonPanel
#include "Config.hpp"                      // for getConfig, Config
#include "Dialog.hpp"                      // for closeAllDialogs, blockingD...
#include "EconomyGraph.hpp"                // for getEconomyGraph, EconomyGraph
#include "GameView.hpp"                    // for getGameView, GameView
#include "HelpWindow.hpp"                  // for HelpWindow
#include "MainLincity.hpp"                 // for saveCityNG, lincitySpeed
#include "MiniMap.hpp"                     // for MiniMap, getMiniMap
#include "ScreenInterface.hpp"             // for print_stats, updateDate
#include "TimerInterface.hpp"              // for get_real_time_with
#include "Util.hpp"                        // for getButton
#include "gui/Button.hpp"                  // for Button
#include "gui/Component.hpp"               // for Component
#include "gui/ComponentLoader.hpp"         // for loadGUIFile
#include "gui/Desktop.hpp"                 // for Desktop
#include "gui/Event.hpp"                   // for Event
#include "gui/Painter.hpp"                 // for Painter
#include "gui/callback/Callback.hpp"       // for makeCallback, Callback
#include "gui/callback/Signal.hpp"         // for Signal
#include "gui_interface/mps.h"             // for mps_refresh, mps_set, mps_...
#include "gui_interface/shared_globals.h"  // for main_screen_originx, main_...
#include "lincity/ConstructionCount.h"     // for ConstructionCount
#include "lincity/engglobs.h"              // for constructionCount, fast_ti...
#include "lincity/lin-city.h"              // for ANIMATE_DELAY
#include "lincity/lintypes.h"              // for Construction
#include "lincity/simulate.h"              // for do_animate, do_time_step

Game* gameptr = 0;

Game* getGame(){
  return gameptr;
}

Game::Game(SDL_Window* _window)
    : window(_window)
{
    gui.reset(loadGUIFile("gui/app.xml"));
    int width = 0, height = 0;
    SDL_GetWindowSize(window, &width, &height);
    gui->resize(width, height);

    Button* gameMenu = getButton( *gui, "GameMenuButton" );
    gameMenu->clicked.connect( makeCallback(*this, &Game::gameButtonClicked ));

    Button* helpButton = getButton( *gui, "HelpButton" );
    helpButton->clicked.connect( makeCallback(*this, &Game::gameButtonClicked ));

    Button* statButton = getButton( *gui, "StatButton" );
    statButton->clicked.connect( makeCallback(*this, &Game::gameButtonClicked ));

    Desktop* desktop = dynamic_cast<Desktop*> (gui.get());
    if(desktop == 0)
        throw std::runtime_error("Game UI is not a Desktop Component");
    helpWindow.reset(new HelpWindow(desktop));
    gameptr = this;
}

Game::~Game()
{
    if( gameptr == this ){
        gameptr = 0;
    }
}

void Game::showHelpWindow( std::string topic ){
    helpWindow->showTopic( topic );
}

void Game::backToMainMenu(){
    closeAllDialogs();
    getButtonPanel()->selectQueryTool();
    saveCityNG( "9_currentGameNG.scn" );
    running = false;
    quitState = MAINMENU;
}

void Game::gameButtonClicked( Button* button ){
    std::string name = button->getName();
    if( name == "GameMenuButton" ) {
        backToMainMenu();
    } else if( name == "HelpButton" ) {
        helpWindow->showTopic("help");
    } else if( name == "StatButton" ) {
        if( !blockingDialogIsOpen ){
            new Dialog( GAME_STATS );
        }
    } else {
         std::cerr << " Game::gameButtonClicked unknown button '" << name << "'.\n";
    }
}

void Game::quickLoad(){
    closeAllDialogs();

    //load file
    getGameView()->printStatusMessage( "quick load...");
    std::string filename;
    filename.append( "quicksave.scn" );
    if( loadCityNG( filename ) ){
          getGameView()->printStatusMessage( "quick load successful.");
    } else {
          getGameView()->printStatusMessage( "quick load failed!");
    }
}

void Game::quickSave(){
    //save file
    getGameView()->printStatusMessage( "quick save...");
    saveCityNG( "quicksave.scn" );
}

void Game::testAllHelpFiles(){
    getGameView()->printStatusMessage( "Testing Help Files...");

    std::string filename;
    std::string directory = "help/en";
    std::string fullname;
    char **rc = PHYSFS_enumerateFiles( directory.c_str() );
    char **i;
    size_t pos;
    for (i = rc; *i != NULL; i++) {
        fullname = directory;
        fullname.append( *i );
        filename.assign( *i );

        // FIXME: Follow symlinks if able. symlink target may be directory
        // NOTE: Do we really need to check if it's a directory? Unlikely that
        //       a directory will have a '.xml' suffix anyway.
        // FIXME: What to do with PHYSFS_FILETYPE_OTHER? Should we instead make
        //        sure the filetype is PHYSFS_FILETYPE_REGULAR?
        PHYSFS_Stat stat;
        int errorCode = PHYSFS_stat(fullname.c_str(), &stat);
        if(errorCode == 0) {
            std::cerr << "could not stat file: " << filename << std::endl;
        }
        else if(stat.filetype == PHYSFS_FILETYPE_DIRECTORY) {
            continue;
        }

        pos = filename.rfind( ".xml" );
        if( pos != std::string::npos ){
            filename.replace( pos, 4 ,"");
            std::cerr << "--- Examining " << filename << "\n";
            helpWindow->showTopic( filename );
            std::cerr << "\n";
        }
    }
    PHYSFS_freeList(rc);
}

MainState
Game::run()
{
    SDL_Event event;
    running = true;
    Desktop* desktop = dynamic_cast<Desktop*> (gui.get());
    if(!desktop)
    {   throw std::runtime_error("Toplevel component is not a Desktop");}
    gui->resize(getConfig()->videoX, getConfig()->videoY);
    int frame = 0;

    Uint32 next_execute = 0, next_animate = 0, next_gui = 0, next_fps = 0;
    __attribute__((unused))
    Uint32 prev_execute = 0, prev_animate = 0, prev_gui = 0, prev_fps = 0;
    Uint32 next_task;
    Uint32 tick;
    while(running) {
        next_task = std::min({next_execute, next_animate, next_gui, next_fps});
        while(true) {
            int event_timeout = next_task - SDL_GetTicks();
            if(event_timeout < 0) event_timeout = 0;
            int status = SDL_WaitEventTimeout(&event, event_timeout);
            if(!status) break; // timed out

            switch(event.type) {
                case SDL_WINDOWEVENT:
                    switch(event.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        videoSizeChanged(event.window.data1, event.window.data2);
                        gui->resize(event.window.data1, event.window.data2);
                        getConfig()->videoX = event.window.data1;
                        getConfig()->videoY = event.window.data2;
                        break;
                    case SDL_WINDOWEVENT_ENTER:
                    case SDL_WINDOWEVENT_LEAVE:
                        Event gui_event(event);
                        gui->event(gui_event);
                        break;
                    }
                    break;
                case SDL_KEYUP: {
                     Event gui_event(event);
                     if( gui_event.keysym.sym == SDLK_ESCAPE ){
                         getButtonPanel()->selectQueryTool();
                         break;
                     }
                     if( gui_event.keysym.sym == SDLK_b ){
                         getButtonPanel()->toggleBulldozeTool();
                         break;
                     }
/*                  //FIXME hack for monitoring constructionCount
                    if( gui_event.keysym.sym == SDLK_c ){
                         std::cout << "ConstructionCount.size() = " << constructionCount.size() << std::endl;
                         int i, j;
                         for (i = 0, j = 0; i < constructionCount.size(); i++) {constructionCount[i]?j++:j;}
                         std::cout << "for a total of " << j << " active constructions" << std::endl;
                         break;
                     }
*/
                     if( gui_event.keysym.sym == SDLK_p ){

                            static int i = 0;
                            while(i < constructionCount.size() && !constructionCount[i]) {i++;}
                            if (i < constructionCount.size())
                            {
                                main_screen_originx = constructionCount[i]->x;
                                main_screen_originy = constructionCount[i]->y;
                                getGameView()->readOrigin(true);
                                mps_set( main_screen_originx, main_screen_originy, MPS_MAP);
                                mps_update();
                                mps_refresh();
                                i++;
                            }
                            else
                            {
                                i = 0;
                            }
                         break;
                     }
                     if( gui_event.keysym.sym == SDLK_F1 ){
                         helpWindow->showTopic("help");
                         break;
                     }
                     if( gui_event.keysym.sym == SDLK_F12 ){
                         quickSave();
                         break;
                     }
                     if( gui_event.keysym.sym == SDLK_F9 ){
                         quickLoad();
                         break;
                     }
#ifdef DEBUG
                     if( gui_event.keysym.sym == SDLK_F5 ){
                         testAllHelpFiles();
                         break;
                     }
#endif
                     int need_break=true;
                     switch(gui_event.keysym.sym) {
                       case SDLK_BACKQUOTE: getMiniMap()->mapViewChangeDisplayMode(MiniMap::NORMAL); break;
                       case SDLK_1: getMiniMap()->mapViewChangeDisplayMode(MiniMap::STARVE); break;
                       case SDLK_2: getMiniMap()->mapViewChangeDisplayMode(MiniMap::UB40); break;
                       case SDLK_3: getMiniMap()->mapViewChangeDisplayMode(MiniMap::POWER); break;
                       case SDLK_4: getMiniMap()->mapViewChangeDisplayMode(MiniMap::FIRE); break;
                       case SDLK_5: getMiniMap()->mapViewChangeDisplayMode(MiniMap::CRICKET); break;
                       case SDLK_6: getMiniMap()->mapViewChangeDisplayMode(MiniMap::HEALTH); break;
                       case SDLK_7: getMiniMap()->mapViewChangeDisplayMode(MiniMap::TRAFFIC); break;
                       case SDLK_8: getMiniMap()->mapViewChangeDisplayMode(MiniMap::POLLUTION); break;
                       case SDLK_9: getMiniMap()->mapViewChangeDisplayMode(MiniMap::COAL); break;
                       case SDLK_0: getMiniMap()->mapViewChangeDisplayMode(MiniMap::COMMODITIES); break;
                       default:  need_break=false;
                     }
                     if (need_break) break;

                     gui->event(gui_event);
                     break;
                }
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEWHEEL:
                case SDL_KEYDOWN: {
                    Event gui_event(event);
                    gui->event(gui_event);
                    break;
                }
                case SDL_QUIT:
                    saveCityNG( "9_currentGameNG.scn" );
                    running = false;
                    quitState = QUIT;
                    break;
                default:
                    break;
            }

            if(desktop->needsRedraw())
              next_task = tick;
        }

        tick = SDL_GetTicks();
        get_real_time_with(tick);
        frame++;

        if(tick >= next_gui) { // gui update
            // fire update event
            gui->event(Event((tick - prev_gui) / 1000.0f));

            // update the help window
            // TODO: Why is this not triggered by the gui update?
            helpWindow->update();

            // other updates
            print_stats();
            updateDate();
            updateMoney();

            // reschedule
            next_gui = tick + 1000/30; // 30 FPS
            prev_gui = tick;
        }
        if(tick >= next_execute) { // execute
            // simulation timestep
            do_time_step();

            // reschedule
            if(lincitySpeed == fast_time_for_year)
              next_execute = tick;
            else
              next_execute = tick + lincitySpeed;
            prev_execute = tick;
        }
        if(tick >= next_animate) { // game animation
            // animate
            do_animate();

            // reschedule
            next_animate = tick + ANIMATE_DELAY;
            prev_animate = tick;
        }
        if(tick >= next_fps) { // fps
#ifdef DEBUG_FPS
            printf("FPS: %d\n", (frame*1000) / (ticks - fpsTicks));
#endif
            getEconomyGraph()->newFPS( frame );
            frame = 0;

            // reschedule
            next_fps = tick + 1000;
            prev_fps = tick;
        }

        if(desktop->needsRedraw()) { // redraw
            desktop->draw(*painter);
            painter->updateScreen();
        }

        // this is kind of janky, but it works for now
        if( lincitySpeed == 0 || blockingDialogIsOpen ) {
            // deschedule execute and animate
            next_execute = ~0;
            next_animate = ~0;
        }
        else if(next_execute == (Uint32)~0 || next_animate == (Uint32)~0) {
            // reschedule execute and animate
            next_execute = tick;
            next_animate = tick;
        }
    }
    return quitState;
}

/** @file lincity-ng/Game.cpp */
