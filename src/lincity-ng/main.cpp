/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>
Copyright (C) 2024 David Bears <dbear4q@gmail.com>

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

#include "main.hpp"

#include <SDL.h>                                 // for SDL_GL_SetAttribute
#include <SDL_mixer.h>                           // for Mix_HookMusicFinished
#include <SDL_opengl.h>                          // for glDisable, glLoadIde...
#include <SDL_ttf.h>                             // for TTF_Init, TTF_Quit
#include <assert.h>                              // for assert
#include <config.h>                              // for PACKAGE_NAME, PACKAG...
#include <libxml/parser.h>                       // for xmlCleanupParser
#include <stdio.h>                               // for NULL, printf
#include <stdlib.h>                              // for setenv
#include <unistd.h>                              // for execlp
#include <filesystem>                            // for operator/, path
#include <iostream>                              // for basic_ostream, opera...
#include <memory>                                // for unique_ptr, allocator
#include <sstream>                               // for basic_stringstream
#include <stdexcept>                             // for runtime_error
#include <string>                                // for char_traits, basic_s...

#include "Config.hpp"                            // for getConfig, Config
#include "Game.hpp"                              // for Game
#include "MainLincity.hpp"                       // for initLincity
#include "MainMenu.hpp"                          // for MainMenu
#include "PBar.hpp"                              // for LCPBarPage1, LCPBarP...
#include "Sound.hpp"                             // for Sound, getSound, Mus...
#include "gui/FontManager.hpp"                   // for FontManager, fontMan...
#include "gui/Painter.hpp"                       // for Painter
#include "gui/PainterGL/PainterGL.hpp"           // for PainterGL
#include "gui/PainterGL/TextureManagerGL.hpp"    // for TextureManagerGL
#include "gui/PainterSDL/PainterSDL.hpp"         // for PainterSDL
#include "gui/PainterSDL/TextureManagerSDL.hpp"  // for TextureManagerSDL
#include "gui/TextureManager.hpp"                // for texture_manager, Tex...
#include "lincity/init_game.h"                   // for destroy_game
#include "tinygettext/tinygettext.hpp"           // for DictionaryManager

#ifndef DEBUG
#include <exception>                             // for exception
#endif

extern Config *configPtr;

SDL_Window* window = NULL;
SDL_GLContext window_context = NULL;
SDL_Renderer* window_renderer = NULL;
Painter* painter = 0;
tinygettext::DictionaryManager* dictionaryManager = 0;
bool restart = false;
const char *appdatadir;

void musicHalted() {
    getSound()->changeTrack(NEXT_OR_FIRST_TRACK);
    //FIXME: options menu song entry doesn't update while song changes.
}

void videoSizeChanged(int width, int height) {
    if (getConfig()->useOpenGL) {
        /* Reset OpenGL state */
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glClearColor(0, 0, 0, 0);
        glViewport(0, 0, width, height);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, height, 0, -1, 1);

        glClear(GL_COLOR_BUFFER_BIT);
    }
}
void resizeVideo(int width, int height, bool fullscreen)
{
    // Set fullscreen (video mode change)
    if (fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window, 0);
        SDL_SetWindowSize(window, width, height);
    }
}

void initVideo(int width, int height)
{
    Uint32 flags = 0;

    flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;
    if( getConfig()->useOpenGL ){
        flags |= SDL_WINDOW_OPENGL;
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 1);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 1);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 1);
        //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
        //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    }
    if(getConfig()->useFullScreen)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    window = SDL_CreateWindow(PACKAGE_NAME " " PACKAGE_VERSION,
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, width, height,
                              flags);
    if( getConfig()->useOpenGL ){
        window_context = SDL_GL_CreateContext(window);
        SDL_GL_SetSwapInterval(1);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glClearColor(0, 0, 0, 0);
        glViewport(0, 0, width, height);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, height, 0, -1, 1);

        glClear(GL_COLOR_BUFFER_BIT);

        painter = new PainterGL(window);
        std::cout << "\nOpenGL Mode " << width;
        std::cout << "x" << height << "\n";

        texture_manager = new TextureManagerGL();
    } else {
        window_renderer = SDL_CreateRenderer(window, -1, 0);

        painter = new PainterSDL(window_renderer);
        std::cout << "\nSDL Mode " << width;
        std::cout << "x"<< height <<"\n";

        texture_manager = new TextureManagerSDL();
    }

    fontManager = new FontManager();
}

void mainLoop()
{
    std::unique_ptr<MainMenu> menu;
    std::unique_ptr<Game> game;
    MainState state = MAINMENU;
    MainState nextstate;

    while(state != QUIT)
    {
        switch(state)
        {
            case MAINMENU:
                {
                    if(menu.get() == 0)
                    {   menu.reset(new MainMenu(window));}
                    nextstate = menu->run();
                }
                break;
            case INGAME:
                {
                    if(game.get() == 0)
                    {
                        game.reset(new Game(window));

                        while(!LCPBarPage1 || !LCPBarPage2)
                        {//wait until PBars exist so they can be initalized
                            printf(".");
                            SDL_Delay(100);
                        }
                    }
                    nextstate = game->run();
                    if(menu.get() == 0)
                    {    menu.reset(new MainMenu(window));}
                    menu->gotoMainMenu();

                }
                break;
            case RESTART:
                restart = true;
                nextstate = QUIT;
                break;
            default:
                assert(false);
        }
        state = nextstate;
    }
}

int main(int argc, char** argv)
{
    int result = 0;

    configPtr = new Config();
    getConfig()->parseCommandLine(argc, argv);

#ifndef DEBUG //in debug mode we wanna have a backtrace
    try {
        std::cout << "Starting " << PACKAGE_NAME << " (version " << PACKAGE_VERSION << ")...\n";
#else
        std::cout << "Starting " << PACKAGE_NAME << " (version " << PACKAGE_VERSION << ") in Debug Mode...\n";
#endif

        if( getConfig()->language != "autodetect" ){
#if defined (WIN32)
            _putenv_s("LINCITY_LANG", getConfig()->language.c_str());
#else
            setenv("LINCITY_LANG", getConfig()->language.c_str(), false);
#endif
        }
        dictionaryManager = new tinygettext::DictionaryManager();
        dictionaryManager->set_charset("UTF-8");
        dictionaryManager->add_directory(getConfig()->appDataDir / "locale");
        std::cout << "Language is \"" << dictionaryManager->get_language() << "\".\n";

        // char *lc_textdomain_directory[1024];
        // if(snprintf(lc_textdomain_directory, 1024, "%s%c%s",
        //   appdatadir, "/", "locale") < 1024) {
        //   char *dm = bindtextdomain(PACKAGE, lc_textdomain_directory);
        //   fprintf(stderr, "Bound textdomain directory is %s\n", dm);
        //   char *td = textdomain(PACKAGE);
        //   fprintf(stderr, "Textdomain is %s\n", td);
        // } else {
        //   fprintf(stderr, "warning: %s, %s",
        //     "data directory path name too long",
        //     "cannot set text domain");
        // }

#ifndef DEBUG
    } catch(std::exception& e) {
        std::cerr << "Unexpected exception: " << e.what() << "\n";
        return 1;
    } catch(...) {
        std::cerr << "Unexpected exception.\n";
        return 1;
    }
#endif

// in debug mode we want a backtrace of the exceptions so we don't catch them
    try {
        xmlInitParser ();
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            std::stringstream msg;
            msg << "Couldn't initialize SDL: " << SDL_GetError();
            throw std::runtime_error(msg.str());
        }
        if(TTF_Init() < 0) {
            std::stringstream msg;
            msg << "Couldn't initialize SDL_ttf: " << SDL_GetError();
            throw std::runtime_error(msg.str());
        }
        initVideo(getConfig()->videoX, getConfig()->videoY);
        initLincity();
        std::unique_ptr<Sound> sound;
        sound.reset(new Sound());
        //set a function to call when music stops
        Mix_HookMusicFinished(musicHalted);
        mainLoop();
        getConfig()->save();
        destroy_game();
    } catch(std::exception& e) {
        std::cerr << "Unexpected exception: " << e.what() << "\n";
        result = 1;
    } catch(...) {
        std::cerr << "Unexpected exception.\n";
        result = 1;
    }
    delete painter;
    delete fontManager;
    delete texture_manager;
    if(TTF_WasInit())
        TTF_Quit();
    if(SDL_WasInit(0))
        SDL_Quit();
    xmlCleanupParser();
    delete dictionaryManager;
    dictionaryManager = 0;
    if( restart ){
#ifdef WIN32
        //Windows has a Problem with Whitespaces.
        std::string fixWhiteSpaceInPathnameProblem;
        fixWhiteSpaceInPathnameProblem="\"";
        fixWhiteSpaceInPathnameProblem+=argv[0];
        fixWhiteSpaceInPathnameProblem+="\"";
        execlp( argv[0], fixWhiteSpaceInPathnameProblem.c_str(), (char *) NULL );
#else
        execlp( argv[0], argv[0], (char *) NULL );
#endif
    }
    return result;
}


/** @file lincity-ng/main.cpp */
