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
#include <libxml/parser.h>
#include <unistd.h>

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
#include "PBar.hpp"
#include "lincity/loadsave.h"

#ifdef ENABLE_BINRELOC
#include "binreloc.h"
#endif

Painter* painter = 0;
TinyGetText::DictionaryManager* dictionaryManager = 0;
bool restart = false;

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
    const char* application = LC_SAVE_DIR;
    const char* userdir = PHYSFS_getUserDir();
    const char* dirsep = PHYSFS_getDirSeparator();
    char* writedir = new char[strlen(userdir) + strlen(application) + 2];

    // Set configuration directory
    //sprintf(writedir, "%s.%s", userdir, application);
    sprintf(writedir, "%s%s", userdir, application);
    if(!PHYSFS_setWriteDir(writedir)) {
        // try to create the directory
        char* mkdir = new char[strlen(application) + 2];
        sprintf(mkdir, "%s", application);
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

#if defined(APPDATADIR) || defined(ENABLE_BINRELOC)
    std::string datadir;
#ifdef ENABLE_BINRELOC
    BrInitError error;
    if (br_init (&error) == 0 && error != BR_INIT_ERROR_DISABLED) {
        printf ("Warning: BinReloc failed to initialize (error code %d)\n",
                error);
        printf ("Will fallback to hardcoded default path.\n");
    }
    
    char* brdatadir = br_find_data_dir("/usr/local/share");
    datadir = brdatadir;
    datadir += "/" PACKAGE_NAME;
    free(brdatadir);
#else
    datadir = APPDATADIR;
#endif
    
    if(!PHYSFS_addToSearchPath(datadir.c_str(), 1)) {
        std::cout << "Couldn't add '" << datadir
            << "' to physfs searchpath: " << PHYSFS_getLastError() << "\n";
    }
#endif

    // allow symbolic links
    PHYSFS_permitSymbolicLinks(1);

    //show search Path 
    for(char** i = PHYSFS_getSearchPath(); *i != NULL; i++)
        printf("[%s] is in the search path.\n", *i);

    // ugly: set LINCITY_HOME environment variable
    const char* lincityhome = PHYSFS_getRealDir("colour.pal");
    if(lincityhome == 0) {
        throw std::runtime_error("Couldn't locate lincity data (colour.pal).");
    }
    std::cout << "LINCITY_HOME: " << lincityhome << "\n";
    char tmp[256];
    snprintf(tmp, sizeof(tmp), "LINCITY_HOME=%s", lincityhome);
    putenv(tmp);
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

    SDL_Surface* screen
        = SDL_SetVideoMode(width, height, bpp, flags);
    SDL_WM_SetCaption(PACKAGE_NAME " " PACKAGE_VERSION, 0);
    if(!screen) {
        std::stringstream msg;
        msg << "Couldn't set video mode ("
            << width << "x" << height
            << "-" << bpp << "bpp) : " << SDL_GetError();
        if(getConfig()->useOpenGL) {
            std::cerr << "* Fallback to software mode.\n";
            getConfig()->useOpenGL = false;
            initVideo(width, height);
            return;
        }
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
        std::cout << "OpenGL Mode " << getConfig()->videoX; 
        std::cout << "x" << getConfig()->videoY << "\n";
    } else {
        painter = new PainterSDL(screen);
        std::cout << "SDL Mode " << getConfig()->videoX;
        std::cout << "x"<< getConfig()->videoY <<"\n";
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

void checkGlErrors()
{
    GLenum glerror = glGetError();
    if( glerror == GL_NO_ERROR ){
        return;
    }
    std::cerr << "glGetError reports";
    while( glerror != GL_NO_ERROR ){
        std::cerr << " ";
        switch( glerror ){
            case GL_INVALID_ENUM:
                std::cerr << "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                std::cerr << "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                std::cerr << "GL_INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                std::cerr << "GL_STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW: 
                std::cerr << "GL_STACK_UNDERFLOW";
                break;
            case GL_TABLE_TOO_LARGE:           
                std::cerr << "GL_TABLE_TOO_LARGE";
                break;
            case GL_OUT_OF_MEMORY:           
                std::cerr << "GL_OUT_OF_MEMORY";
                break;
            default:
                std::cerr << glerror;
        }
        glerror = glGetError();
    }
    std::cerr << "\n";
}

void flipScreenBuffer()
{
    if( getConfig()->useOpenGL ){
        checkGlErrors();
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
    
    //we need the game-gui to set all states while loading a savegame
    if(game.get() == 0)
        game.reset(new Game());
    while(!LCPBarInstance){//wait until PBars exist so they can be initalized
        printf(".");
        SDL_Delay(100);
    }
    initLincity();

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

void parseCommandLine(int argc, char** argv)
{
    for(int currentArgument = 1; currentArgument < argc; ++currentArgument) {
        std::string argStr = argv[currentArgument];
        
        if(argStr == "-v" || argStr == "--version") {
            std::cout << PACKAGE_NAME << " version " << PACKAGE_VERSION << "\n";
            exit(0);
        } else  if(argStr == "-h" || argStr == "--help") {
            std::cout << PACKAGE_NAME << " version " << PACKAGE_VERSION << "\n";
            std::cout << "Command line overrides configfiles.\n";
            std::cout << "Known arguments are:\n";
            std::cout << "-v        --version         show version and exit\n";
            std::cout << "-h        --help            show his text and exit\n";
            std::cout << "-g        --gl              use OpenGL\n";
            std::cout << "-s        --sdl             use SDL\n";
            std::cout << "-S [size] --size [size]     specify screensize (eg. 1024x768)\n";
            std::cout << "-w        --window          run in window\n";
            std::cout << "-f        --fullscreen      run fullscreen\n";
            std::cout << "-m        --mute            mute audio\n";
            exit(0);
        } else if(argStr == "-g" || argStr == "--gl") {
            getConfig()->useOpenGL = true;
        } else if(argStr == "-s" || argStr == "--sdl") {
            getConfig()->useOpenGL = false; 
        } else if(argStr == "-S" || argStr == "--size") {
            currentArgument++;
            if(currentArgument >= argc) {
                std::cerr << "Error: --size needs a parameter.\n";
                exit(1);
            }
            argStr = argv[currentArgument];
            int newX, newY, count;
            count = sscanf( argStr.c_str(), "%ix%i", &newX, &newY );
            if( count != 2  ) {
                std::cerr << "Error: Can not parse --size parameter.\n";
                exit( 1 );
            }
            if(newX <= 0 || newY <= 0) {
                std::cerr << "Error: Size parameter out of range.\n";
                exit(1);
            }
            getConfig()->videoX = newX;
            getConfig()->videoY = newY;
            std::cout << newX << " " << newY << "\n";
        } else if(argStr == "-f" || argStr == "--fullscreen") {
            getConfig()->useFullScreen = true; 
        } else if(argStr == "-w" || argStr == "--window") {
            getConfig()->useFullScreen = false; 
        } else if(argStr == "-m" || argStr == "--mute") {
            getConfig()->soundEnabled = false;
            getConfig()->musicEnabled = false;
        } else {
            std::cerr << "Unknown command line argument: " << argStr << "\n";
            exit(1);
        }
    }
}

int main(int argc, char** argv)
{
    int result = 0;

#ifndef DEBUG //in debug mode we wanna have a backtrace
    try {
        std::cout << "Starting " << PACKAGE_NAME << " (version " << PACKAGE_VERSION << ")...\n";
#else
        std::cout << "Starting " << PACKAGE_NAME << " (version " << PACKAGE_VERSION << ") in Debug Mode...\n";
#endif                                                     
        initPhysfs(argv[0]);
        dictionaryManager = new TinyGetText::DictionaryManager();
        dictionaryManager->set_charset("UTF-8");
        dictionaryManager->add_directory("locale");
#ifdef DEBUG
        std::cout << "Language is \"" << dictionaryManager->get_language() << "\".\n";
#endif
        
#ifndef DEBUG
    } catch(std::exception& e) {
        std::cerr << "Unexpected exception: " << e.what() << "\n";        
        return 1;
    } catch(...) {
        std::cerr << "Unexpected exception.\n";
        return 1;
    }                                                                     
#endif

    parseCommandLine(argc, argv);
   
// in debug mode we want a backtrace of the exceptions so we don't catch them
#ifndef DEBUG
    try {
#endif
        xmlInitParser();
        std::auto_ptr<Sound> sound; 
        sound.reset(new Sound()); 
        initSDL();
        initTTF();
        initVideo(getConfig()->videoX, getConfig()->videoY);

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
    xmlCleanupParser();
    delete dictionaryManager;
    dictionaryManager = 0;
    PHYSFS_deinit();
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

