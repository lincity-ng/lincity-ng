#include <config.h>

#include "Game.hpp"

#include "gui/TextureManager.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Component.hpp"
#include "gui/Event.hpp"
#include "gui/Painter.hpp"

#include "MainLincity.hpp"

Game::Game()
{
    painter.reset(new Painter(SDL_GetVideoSurface()));
    gui.reset(loadGUIFile("gui/app.xml"));
    gui->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}

Game::~Game()
{
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

        gui->draw(*painter);
        SDL_Flip(SDL_GetVideoSurface());
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
