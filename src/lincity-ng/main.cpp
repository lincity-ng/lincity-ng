#include <config.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <SDL.h>
#include <SDL_ttf.h>

#include "gui/TextureManager.hpp"

#include "main.hpp"
#include "MainLincity.hpp"
#include "MainMenu.hpp"
#include "Game.hpp"

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

void initVideo(int width, int height)
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
    MainState state = MAINMENU;
    MainState nextstate;

    while(state != QUIT) {
        switch(state) {
            case MAINMENU:
                MainMenu* menu = new MainMenu();
                nextstate = menu->run();
                delete menu;
                break;
            case INGAME:
                Game* game = new Game();
                nextstate = game->run();
                delete game;
                break;
            default:
                assert(false);
        }

        state = nextstate;
    }
}

int main(int , char** )
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
