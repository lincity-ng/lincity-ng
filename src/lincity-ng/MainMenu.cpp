#include <config.h>

#include <stdexcept>
#include <sstream>
#include <typeinfo>

#include "gui/TextureManager.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Component.hpp"
#include "gui/Event.hpp"
#include "gui/Painter.hpp"
#include "gui/Button.hpp"
#include "gui/callback/Callback.hpp"

#include "MainMenu.hpp"
#include "Util.hpp"

MainMenu::MainMenu()
{
    painter.reset(new Painter(SDL_GetVideoSurface()));

    loadMainMenu();
    currentMenu = mainMenu.get();
}

MainMenu::~MainMenu()
{
}

void
MainMenu::loadMainMenu()
{
    if(mainMenu.get() == 0) {
        mainMenu.reset(loadGUIFile("gui/mainmenu.xml"));

        // connect signals
        Button* quitButton = getButton(*mainMenu, "QuitButton");
        quitButton->clicked.connect(
                makeCallback(*this, &MainMenu::quitButtonClicked));
        Button* continueButton = getButton(*mainMenu, "ContinueButton"); 
        continueButton->clicked.connect(
                makeCallback(*this, &MainMenu::continueButtonClicked));
        Button* newGameButton = getButton(*mainMenu, "NewGameButton");
        newGameButton->clicked.connect(
                makeCallback(*this, &MainMenu::newGameButtonClicked));
        Button* loadGameButton = getButton(*mainMenu, "LoadButton");
        loadGameButton->clicked.connect(
                makeCallback(*this, &MainMenu::loadGameButtonClicked));
    }

    mainMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}

void
MainMenu::loadNewGameMenu()
{
    if(newGameMenu.get() == 0) {
        newGameMenu.reset(loadGUIFile("gui/newgame.xml"));

        // connect signals
        Button* startButton = getButton(*newGameMenu, "StartButton");
        startButton->clicked.connect(
                makeCallback(*this, &MainMenu::newGameStartButtonClicked));
        Button* backButton = getButton(*newGameMenu, "BackButton");
        backButton->clicked.connect(
                makeCallback(*this, &MainMenu::newGameBackButtonClicked));
    }

    newGameMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}

void
MainMenu::loadLoadGameMenu()
{
    if(loadGameMenu.get() == 0) {
        loadGameMenu.reset(loadGUIFile("gui/loadgame.xml"));

        // connect signals
        Button* loadButton = getButton(*loadGameMenu, "LoadButton");
        loadButton->clicked.connect(
                makeCallback(*this, &MainMenu::loadGameLoadButtonClicked));
        Button* backButton = getButton(*loadGameMenu, "BackButton");
        backButton->clicked.connect(
                makeCallback(*this, &MainMenu::loadGameBackButtonClicked));
    }

    loadGameMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}
    
void
MainMenu::quitButtonClicked(Button* )
{
    quitState = QUIT;
    running = false;
}

void
MainMenu::continueButtonClicked(Button* )
{
    quitState = INGAME;
    running = false;
}

void
MainMenu::newGameButtonClicked(Button* )
{
    loadNewGameMenu();
    currentMenu = newGameMenu.get();
}

void
MainMenu::loadGameButtonClicked(Button* )
{
    loadLoadGameMenu();
    currentMenu = loadGameMenu.get();
}

void
MainMenu::newGameStartButtonClicked(Button* )
{
    quitState = INGAME;
    running = false;
}

void
MainMenu::newGameBackButtonClicked(Button* )
{
    loadMainMenu();
    currentMenu = mainMenu.get();
}

void
MainMenu::loadGameBackButtonClicked(Button* )
{
    loadMainMenu();
    currentMenu = mainMenu.get();
}

void
MainMenu::loadGameLoadButtonClicked(Button *)
{
    quitState = INGAME;
    running = false;
}

MainState
MainMenu::run()
{
    SDL_Event event;
    running = true;
    quitState = QUIT;
    Uint32 ticks = SDL_GetTicks();
    int frame = 0;
    while(running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_VIDEORESIZE:
                    initVideo(event.resize.w, event.resize.h);
                    painter.reset(new Painter(SDL_GetVideoSurface()));
                    currentMenu->resize(event.resize.w, event.resize.h);
                    break;
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                    Event gui_event(event);
                    currentMenu->event(gui_event);
                    break;
                }
                case SDL_QUIT:
                    running = false;
                    quitState = QUIT;
                    break;
                default:
                    break;
            }
        }

        //SDL_FillRect(SDL_GetVideoSurface(), 0, 0);
        currentMenu->draw(*painter);
        SDL_Flip(SDL_GetVideoSurface());

        frame++;
        if(SDL_GetTicks() - ticks > 1000) {
            printf("FPS: %d.\n", frame);
            frame = 0;
            ticks = SDL_GetTicks();
        }
    }

    return quitState;
}
