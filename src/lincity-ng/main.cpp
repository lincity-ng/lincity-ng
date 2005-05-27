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

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <physfs.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

#include "gui/FontManager.hpp"
#include "gui/TextureManager.hpp"
#include "gui/PainterSDL/TextureManagerSDL.hpp"
#include "gui/PainterSDL/PainterSDL.hpp"
#include "gui/PainterGL/TextureManagerGL.hpp"
#include "gui/PainterGL/PainterGL.hpp"

#include "main.hpp"
#include "MainLincity.hpp"
#include "MainMenu.hpp"
#include "Game.hpp"
#include "Sound.hpp"
#include "Config.hpp"

Painter* painter = 0;
TinyGetText::DictionaryManager* dictionaryManager = 0;

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

void initPhysfs(const char* argv0)
{
    if(!PHYSFS_init(argv0)) {
        std::stringstream msg;
        msg << "Couldn't initialize physfs: " << PHYSFS_getLastError();
        throw std::runtime_error(msg.str());
    }

    // Initialize physfs (this is a slightly modified version of
    // PHYSFS_setSaneConfig
    const char* application = /* PACKAGE_NAME */ "lincity";
    const char* userdir = PHYSFS_getUserDir();
    const char* dirsep = PHYSFS_getDirSeparator();
    char* writedir = new char[strlen(userdir) + strlen(application) + 2];

    // Set configuration directory
    sprintf(writedir, "%s.%s", userdir, application);
    if(!PHYSFS_setWriteDir(writedir)) {
        // try to create the directory
        char* mkdir = new char[strlen(application) + 2];
        sprintf(mkdir, ".%s", application);
        if(!PHYSFS_setWriteDir(userdir) || !PHYSFS_mkdir(mkdir)) {
            std::ostringstream msg;
            msg << "Failed creating configuration directory '" <<
                writedir << "': " << PHYSFS_getLastError();
            delete[] writedir;
            delete[] mkdir;
            throw std::runtime_error(msg.str());
        }
        delete[] mkdir;

        if(!PHYSFS_setWriteDir(writedir)) {
            std::ostringstream msg;
            msg << "Failed to use configuration directory '" <<            
                writedir << "': " << PHYSFS_getLastError();
            delete[] writedir;
            throw std::runtime_error(msg.str());
        }
    }
    PHYSFS_addToSearchPath(writedir, 0);
    delete[] writedir;
   
    // Search for archives and add them to the search path
    const char* archiveExt = "zip";
    char** rc = PHYSFS_enumerateFiles("/");
    size_t extlen = strlen(archiveExt);

    for(char** i = rc; *i != 0; ++i) {
        size_t l = strlen(*i);
        if((l > extlen) && ((*i)[l - extlen - 1] == '.')) {
            const char* ext = (*i) + (l - extlen);
            if(strcasecmp(ext, archiveExt) == 0) {
                const char* d = PHYSFS_getRealDir(*i);
                char* str = new char[strlen(d) + strlen(dirsep) + l + 1];
                sprintf(str, "%s%s%s", d, dirsep, *i);
                PHYSFS_addToSearchPath(str, 1);
                delete[] str;
            }
        }
    }

    PHYSFS_freeList(rc);
            
    // when started from source dir...
    std::string dir = PHYSFS_getBaseDir();
    dir += "/data";
    std::string testfname = dir;
    testfname += "/images/tiles/images.xml";
    FILE* f = fopen(testfname.c_str(), "r");
    if(f) {
        fclose(f);
        if(!PHYSFS_addToSearchPath(dir.c_str(), 1)) {
#ifdef DEBUG
            std::cout << "Warning: Couldn't add '" << dir << 
                "' to physfs searchpath: " << PHYSFS_getLastError() << "\n";
#endif
        }
    }

#ifdef APPDATADIR
    if(!PHYSFS_addToSearchPath(APPDATADIR, 1)) {
#ifdef DEBUG
        std::cout << "Couldn't add '" << APPDATADIR 
            << "' to physfs searchpath: " << PHYSFS_getLastError() << "\n";
#endif
    }
#endif

    // allow symbolic links
    PHYSFS_permitSymbolicLinks(1);

    //show search Path 
    char **i;
    for (i = PHYSFS_getSearchPath(); *i != NULL; i++)
        printf("[%s] is in the search path.\n", *i);
}

void initVideo(int width, int height)
{
    int bpp = 0;
    int flags = 0;
    if( getConfig()->useOpenGL ){
        flags = SDL_OPENGL;
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 1);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 1);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 1);
        //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
        //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    } else {
        flags = SDL_HWSURFACE;
    }
    if(getConfig()->useFullScreen)
        flags |= SDL_FULLSCREEN;
    else
        flags |= SDL_RESIZABLE;

    SDL_Surface* screen
        = SDL_SetVideoMode(width, height, bpp, flags);
    SDL_WM_SetCaption(PACKAGE_NAME " " PACKAGE_VERSION, 0);
    if(!screen) {
        std::stringstream msg;
        msg << "Couldn't set video mode ("
            << width << "x" << height
            << "-" << bpp << "bpp) : " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }

    delete painter;
    if( getConfig()->useOpenGL ){
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glClearColor(0, 0, 0, 0);
        glViewport(0, 0, screen->w, screen->h);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, screen->w, screen->h, 0, -1, 1);

        glClear(GL_COLOR_BUFFER_BIT);
    
        painter = new PainterGL();
        std::cout << "OpenGL Mode\n";
    } else {
        painter = new PainterSDL(screen);
        std::cout << "SDL Mode\n";
    }

    if(texture_manager == 0) {
        if( getConfig()->useOpenGL ) {
            texture_manager = new TextureManagerGL();
        } else {
            texture_manager = new TextureManagerSDL();
        }
    }

    if(fontManager == 0) {
        fontManager = new FontManager();
    }
}

void flipScreenBuffer()
{
    if( getConfig()->useOpenGL ){
        SDL_GL_SwapBuffers();
        //glClear(GL_COLOR_BUFFER_BIT);
    } else {
        SDL_Flip(SDL_GetVideoSurface());
    }
}

void mainLoop()
{
    std::auto_ptr<MainMenu> menu;
    std::auto_ptr<Game> game;
    MainState state = MAINMENU;
    MainState nextstate;

    while(state != QUIT) {
        switch(state) {
            case MAINMENU:
                {
                    if(menu.get() == 0)
                        menu.reset(new MainMenu());
                    nextstate = menu->run();
                }
                break;
            case INGAME:
                {
                    if(game.get() == 0)
                        game.reset(new Game());
                    nextstate = game->run();
                    if(menu.get() == 0)
                        menu.reset(new MainMenu());
                    menu->gotoMainMenu();
                }
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

#ifndef DEBUG //in debug mode we wanna have a backtrace
    try {
        std::cout << "Starting Lincity-NG...\n";
#else
        std::cout << "Starting Lincity-NG in Debug Mode...\n";
#endif                                                     
        initPhysfs(argv[0]);
        dictionaryManager = new TinyGetText::DictionaryManager();
        dictionaryManager->set_charset("UTF-8");
        dictionaryManager->add_directory("locale");
#ifndef DEBUG
    } catch(std::exception& e) {
        std::cerr << "Unexpected exception: " << e.what() << "\n";        
        return 1;
    } catch(...) {
        std::cerr << "Unexpected exception.\n";
        return 1;
    }                                                                     
#endif
    
    // ugly: set LINCITY_HOME environment variable
    const char* dir = PHYSFS_getRealDir("colour.pal");
    if(dir == 0) {
        std::cerr << "Couldn't locate lincity data (colour.pal).\n";
        return 1;
    }
    std::cout << "LINCITY_HOME: " << dir << "\n";
    char tmp[256];
    snprintf(tmp, sizeof(tmp), "LINCITY_HOME=%s/", dir);
    putenv(tmp);
   
    //parse commandline args
    int currentArgument = 0; 
    bool knownArgument = true; //argv[0] is the programname.
    std::string argStr;
    while( currentArgument < argc ) {
        argStr = argv[ currentArgument ];
        
        if(( argStr == "-v" ) || ( argStr == "--version" )){ //show Version & exit
            std::cout << PACKAGE_NAME << " version " << PACKAGE_VERSION << "\n";
            knownArgument = true;
            exit( 0 );
        }
        if(( argStr == "-h" ) || ( argStr == "--help" )){ //show Options & exit
            std::cout << PACKAGE_NAME << " version " << PACKAGE_VERSION << "\n";
            std::cout << "Command line overrides configfiles.\n";
            std::cout << "Known arguments are:\n";
            std::cout << "-v    --version    show version and exit\n";
            std::cout << "-h    --help       show his text and exit\n";
            std::cout << "-gl   --gl         use OpenGL\n";
            std::cout << "-sdl  --sdl        use SDL\n";
            std::cout << "-w    --window     run in window\n";
            std::cout << "-f    --fullscreen run  fullscreen\n";
            knownArgument = true;
            exit( 0 );
        }
        if(( argStr == "-gl" ) || ( argStr == "--gl" )
	|| ( argStr == "-opengl" ) || ( argStr == "--opengl" )){ //use OpenGL
            getConfig()->useOpenGL = true; 
            knownArgument = true;
        }
        if(( argStr == "-sdl" ) || ( argStr == "--sdl" )){ //use SGL
            getConfig()->useOpenGL = false; 
            knownArgument = true;
        }
        if(( argStr == "-f" ) || ( argStr == "--fullscreen" )){ //fullscreen
            getConfig()->useFullScreen = true; 
            knownArgument = true;
        }
        if(( argStr == "-w" ) || ( argStr == "--window" )){ //windowed
            getConfig()->useFullScreen = false; 
            knownArgument = true;
        }
        
        //This has to be the last Test:
        if( !knownArgument ){
            std::cerr << "Unknown command line argument: " << argStr << "\n";
        }
        currentArgument++;
        knownArgument = false;
    }
#ifndef DEBUG //in debug mode we wanna have a backtrace
    try {
#endif
        std::auto_ptr<Sound> sound; 
        sound.reset(new Sound()); 
        initSDL();
        initTTF();
        initVideo(getConfig()->videoX, getConfig()->videoY);
        initLincity();

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
    getConfig()->save();
    delete painter;
    delete fontManager;
    delete texture_manager;
    if(TTF_WasInit())
        TTF_Quit();
    if(SDL_WasInit(0))
        SDL_Quit();
    delete dictionaryManager;
    dictionaryManager = 0;
    PHYSFS_deinit();
    return result;
}

