#include <config.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <SDL.h>
#include <SDL_ttf.h>

#include "gui/TextureManager.hpp"
#include "gui/ComponentFactory.hpp"
#include "gui/Component.hpp"
#include "gui/Event.hpp"
#include "gui/Painter.hpp"

#include "MainLincity.hpp"

void initSDL()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::stringstream msg;
        msg << "Couldn't initialize SDL: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }
}

void initTTF()
{
    if(TTF_Init() < 0) {
        std::stringstream msg;
        msg << "Couldn't initialize SDL_ttf: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }
}

void initVideo(int width = 800, int height = 600)
{
    int bpp = 32;

    SDL_Surface* screen 
        = SDL_SetVideoMode(width, height, bpp, SDL_DOUBLEBUF | SDL_RESIZABLE);
    if(!screen) {
        std::stringstream msg;
        msg << "Couldn't set video mode ("
            << width << "x" << height << "-" << bpp << "bpp) : "
            << SDL_GetError();
        throw std::runtime_error(msg.str());
    }
}

void mainLoop()
{
    std::auto_ptr<Painter> painter (new Painter(SDL_GetVideoSurface()));
    std::auto_ptr<Component> component (loadGUIFile("gui/app.xml"));
    component->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);

    SDL_Event event;
    bool done = false;
    while(!done) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_VIDEORESIZE:
                    initVideo(event.resize.w, event.resize.h);
                    painter.reset(new Painter(SDL_GetVideoSurface()));
                    component->resize(event.resize.w, event.resize.h);
                    break;
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                    Event component_event(event);
                    component->event(component_event);
                    break;
                }
                case SDL_QUIT:
                    done = true;
                    break;
                default:
                    break;
            }
        }

        SDL_FillRect(SDL_GetVideoSurface(), 0, 0);
        component->draw(*painter);
        SDL_Flip(SDL_GetVideoSurface());

	doLincityStep();

        SDL_Delay(20);
    }
}

int main(int argc, char** argv)
{
    int result = 0;

    // TODO: parse commandline args
    
#ifndef DEBUG //in debug mode we wanna have a backtrace
    try {
#endif
        initSDL();
        initTTF();
        initVideo();
	initLincity();

        texture_manager = new TextureManager();

        mainLoop();
#ifndef DEBUG
    } catch(std::exception& e) {
        std::cerr << "Unexpected exception: " << e.what() << "\n";
        result = 1;
    } catch(...) {
        std::cerr << "Unexpected exception.\n";
        result = 1;
    }
#endif
   
    delete texture_manager;
    if(TTF_WasInit())
        TTF_Quit();
    if(SDL_WasInit(0))
        SDL_Quit();
    return result;
}
