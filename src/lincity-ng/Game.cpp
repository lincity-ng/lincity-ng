#include <config.h>

#include "Game.hpp"

#include "gui/TextureManager.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Component.hpp"
#include "gui/Desktop.hpp"
#include "gui/Event.hpp"
#include "gui/Button.hpp"
#include "gui/callback/Callback.hpp"

#include "MainLincity.hpp"
#include <iostream>
#include "Util.hpp"
#include "lincity/lin-city.h"
#include "GameView.hpp"
#include "HelpWindow.hpp"
#include "ButtonPanel.hpp"
#include "Dialog.hpp"

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
}

Game::~Game()
{
}

void Game::backToMainMenu(){
    getGameView()->writeOrigin();
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

MainState
Game::run()
{
    SDL_Event event;
    running = true;
    Uint32 fpsTicks = SDL_GetTicks();
    Uint32 lastticks = fpsTicks;
    Desktop* desktop = dynamic_cast<Desktop*> (gui.get());
    if(!desktop)
        throw std::runtime_error("Toplevel component is not a Desktop");

    int frame = 0;
    while(running) {
        getGameView()->scroll();
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_VIDEORESIZE:
                    initVideo(event.resize.w, event.resize.h);
                    gui->resize(event.resize.w, event.resize.h);
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
                     if( gui_event.keysym.sym == SDLK_F1 ){
                         helpWindow->showTopic("help");
                         break;
                     }
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
        if(desktop->needsRedraw()) {
            desktop->draw(*painter);
            flipScreenBuffer();
        } else {
            // give the CPU time to relax...
            SDL_Delay(10);
        }
        frame++;
        
        if(ticks - fpsTicks > 1000) {
            printf("FPS: %d.\n", frame);
            frame = 0;
            fpsTicks = ticks;
        }

        doLincityStep();
    }

    return quitState;
}
