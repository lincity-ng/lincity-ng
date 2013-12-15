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
#include <config.h>

#include "Game.hpp"

#include "gui/TextureManager.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Component.hpp"
#include "gui/Desktop.hpp"
#include "gui/Event.hpp"
#include "gui/Button.hpp"
#include "gui/callback/Callback.hpp"
#include "lincity/fileutil.h"
#include "lincity/init_game.h"
#include "gui_interface/shared_globals.h"
#include "gui_interface/mps.h"

#include "MainLincity.hpp"
#include <iostream>
#include <physfs.h>
#include "Util.hpp"
#include "GameView.hpp"
#include "HelpWindow.hpp"
#include "ButtonPanel.hpp"
#include "Dialog.hpp"
#include "EconomyGraph.hpp"
#include "Config.hpp"

extern int lincitySpeed;
extern void execute_timestep(void);

Game* gameptr = 0;

Game* getGame(){
 return gameptr;
}

Game::Game()
{
    gui.reset(loadGUIFile("gui/app.xml"));
    gui->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);

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

        if(PHYSFS_isDirectory(fullname.c_str()))
            continue;

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
    Uint32 fpsTicks = SDL_GetTicks();
    Uint32 lastticks = fpsTicks;
    Desktop* desktop = dynamic_cast<Desktop*> (gui.get());
    if(!desktop)
    {   throw std::runtime_error("Toplevel component is not a Desktop");}
    gui->resize(getConfig()->videoX, getConfig()->videoY);
    int frame = 0;
    while(running) {
        getGameView()->scroll();
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_VIDEORESIZE:
                    initVideo(event.resize.w, event.resize.h);
                    gui->resize(event.resize.w, event.resize.h);
                    getConfig()->videoX = event.resize.w;
                    getConfig()->videoY = event.resize.h;
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
                     gui->event(gui_event);
                     break;
                }
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_KEYDOWN: {
                    Event gui_event(event);
                    gui->event(gui_event);
                    break;
                }
                case SDL_ACTIVEEVENT:
                    if( event.active.gain == 1 ){
                        gui->resize( gui->getWidth(), gui->getHeight() );
                    }
                    break;
                case SDL_VIDEOEXPOSE:
                    gui->resize( gui->getWidth(), gui->getHeight() );
                    break;
                case SDL_QUIT:
                    saveCityNG( "9_currentGameNG.scn" );
                    running = false;
                    quitState = QUIT;
                    break;
                default:
                    break;
            }
        }

        // create update Event
        Uint32 ticks = SDL_GetTicks();
        float elapsedTime = ((float) (ticks - lastticks)) / 1000.0;
        gui->event(Event(elapsedTime));
        lastticks = ticks;

        helpWindow->update();
        if(desktop->needsRedraw())
        {
            desktop->draw(*painter);
            flipScreenBuffer();
        }

        frame++;
        // Slow down cpu consumption in pause mode
        if(ticks - fpsTicks > 1000 && lincitySpeed)
        {
#ifdef DEBUG_FPS
            printf("FPS: %d.\n", (frame*1000) / (ticks - fpsTicks));
#endif
            getEconomyGraph()->newFPS( frame );
            frame = 0;
            fpsTicks = ticks;
        }
        else if(!lincitySpeed)
        {   frame = 0;}
        /* SDL_Delay is done in execute_timestep */
        execute_timestep ();
    }
    return quitState;
}

/** @file lincity-ng/Game.cpp */

