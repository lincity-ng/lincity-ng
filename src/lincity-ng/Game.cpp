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

Game::Game()
{
    gui.reset(loadGUIFile("gui/app.xml"));
    gui->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);

    Button* gameMenu = getButton( *gui, "GameMenuButton" );
    gameMenu->clicked.connect( makeCallback(*this, &Game::gameButtonClicked ));
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
    }
    else {
         std::cerr << " Game::gameButtonClicked unknown button '" << name << "'.\n";
    }
}

void Game::openHelpWindow(){
    Component* root = getGameView();
    if(!root) {
        std::cerr << "Root not found.\n";
        return;
    }
    while( root->getParent() )
        root = root->getParent();
    Desktop* desktop = dynamic_cast<Desktop*> (root);
    if(!desktop) {
        std::cerr << "Root not a desktop!?!\n";
        return;
    }
    try {
        //test if Help-Windows is open
        Component* messageTextComponent = 0;
        messageTextComponent = root->findComponent( "HelpWindowTitle" );
        if(messageTextComponent == 0) {
            messageTextComponent = loadGUIFile("gui/helpwindow.xml");
            desktop->addChildComponent(messageTextComponent);
        }
    } catch(std::exception& e) {
        std::cerr << "Couldn't open HelpWindow"
            << e.what() << "\n";
        return;
    }
}

MainState
Game::run()
{
    SDL_Event event;
    running = true;
    Uint32 ticks = SDL_GetTicks();
    int frame = 0;
    while(running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_VIDEORESIZE:
                    initVideo(event.resize.w, event.resize.h);
                    gui->resize(event.resize.w, event.resize.h);
                    break;
                case SDL_KEYUP: {
                     Event gui_event(event);
                     if( gui_event.keysym.sym == SDLK_ESCAPE ){
                         backToMainMenu();
                         break;
                     }
                     if( gui_event.keysym.sym == SDLK_F1 ){
                         openHelpWindow();
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
        gui->event(Event(Event::UPDATE));
        gui->draw(*painter);
        flipScreenBuffer();
        frame++;
        
        if(SDL_GetTicks() - ticks > 1000) {
            printf("FPS: %d.\n", frame);
            frame = 0;
            ticks = SDL_GetTicks();
        }

        doLincityStep();
    }

    return quitState;
}
