#include <config.h>

#include <stdexcept>
#include <sstream>

#include "gui/TextureManager.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Component.hpp"
#include "gui/Event.hpp"
#include "gui/Painter.hpp"
#include "gui/Button.hpp"
#include "gui/callback/Callback.hpp"

#include "MainMenu.hpp"

static inline Button* getButton(Component& top, const std::string& name)
{
    Component* component = top.findComponent(name);
    if(!component) {
        std::stringstream msg;
        msg << "GUI didn't define Button '" << name << "'.";
        throw std::runtime_error(msg.str());
    }
    Button* button = dynamic_cast<Button*> (component);
    if(!button) {
        std::stringstream msg;
        msg << "Component '" << name << "' is not a Button.";
        throw std::runtime_error(msg.str());
    }

    return button;
}

MainMenu::MainMenu()
{
    painter.reset(new Painter(SDL_GetVideoSurface()));
    gui.reset(loadGUIFile("gui/mainmenu.xml"));
    gui->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);

    // connect signals
    Button* quitButton = getButton(*gui, "QuitButton");
    quitButton->clicked.connect(
            makeCallback(*this, &MainMenu::quitButtonClicked));
    Button* continueButton = getButton(*gui, "ContinueButton"); 
    continueButton->clicked.connect(
            makeCallback(*this, &MainMenu::continueButtonClicked));
    Button* newGameButton = getButton(*gui, "NewGameButton");
    newGameButton->clicked.connect(
            makeCallback(*this, &MainMenu::newGameButtonClicked));
}

MainMenu::~MainMenu()
{
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
    quitState = INGAME;
    running = false;
}

MainState
MainMenu::run()
{
    SDL_Event event;
    running = true;
    quitState = QUIT;
    while(running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_VIDEORESIZE:
                    initVideo(event.resize.w, event.resize.h);
                    painter.reset(new Painter(SDL_GetVideoSurface()));
                    gui->resize(event.resize.w, event.resize.h);
                    break;
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                    Event gui_event(event);
                    gui->event(gui_event);
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

        SDL_FillRect(SDL_GetVideoSurface(), 0, 0);
        gui->draw(*painter);
        SDL_Flip(SDL_GetVideoSurface());
    }

    return quitState;
}
