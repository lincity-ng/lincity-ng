#include <config.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <physfs.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <stdlib.h>

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
    if(!PHYSFS_setSaneConfig("devs", PACKAGE, ".zip", 0, 1)) {
        std::stringstream msg;
        msg << "Couldn't set physfs config: " << PHYSFS_getLastError();
        throw std::runtime_error(msg.str());
    }
#ifdef APPDATADIR
    if(!PHYSFS_addToSearchPath(APPDATADIR, 1)) {
        std::stringstream msg;
        msg << "Couldn't add '" << APPDATADIR << "' to physfs searchpath: "
            << PHYSFS_getLastError();
        throw std::runtime_error(msg.str());
    }
#endif
    
    // when started from source dir...
    std::string dir = PHYSFS_getBaseDir();
    dir += "/data";
    if(!PHYSFS_addToSearchPath(dir.c_str(), 1)) {
        std::stringstream msg;
        msg << "Couldn't add '" << dir << "' to physfs searchpath: "
            << PHYSFS_getLastError();
        throw std::runtime_error(msg.str());
    }

    // allow symbolic links
    PHYSFS_permitSymbolicLinks(1);
}

void initVideo(int width, int height)
{
    int bpp = 0;
    int flags;
    if( getConfig()->useOpenGL ){
        flags = SDL_OPENGL | SDL_RESIZABLE;
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 1);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 1);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 1);
        //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
        //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    } else {
        flags = SDL_HWSURFACE  | SDL_RESIZABLE;
    }
    SDL_Surface* screen
        = SDL_SetVideoMode(width, height, bpp, flags);
    SDL_WM_SetCaption(PACKAGE_NAME " " PACKAGE_VERSION, 0);
    if(!screen) {
        std::stringstream msg;
        msg << "Couldn't set video mode ("
            << width << "x" << height << "-" << bpp << "bpp) : "
            << SDL_GetError();
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
    } else {
        painter = new PainterSDL(screen);
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
    MainState state = MAINMENU;
    MainState nextstate;

    while(state != QUIT) {
        switch(state) {
            case MAINMENU:
                {
                    MainMenu* menu = new MainMenu();
                    nextstate = menu->run();
                    delete menu;
                }
                break;
            case INGAME:
                {
                    Game* game = new Game();
                    nextstate = game->run();
                    delete game;
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

    initPhysfs(argv[0]);

    //set LINCITY_HOME environment variable
    setenv( "LINCITY_HOME", getConfig()->lincityHome.c_str(), 1 );
   
    //parse commandline args
    int currentArgument = 0; 
    bool knownArgument = true; //argv[0] is the programname.
    std::string argStr;
    while( currentArgument < argc ) {
        argStr = argv[ currentArgument ];
        
        if( argStr == "-v" ){ //show Version & exit
            std::cout << PACKAGE_NAME << " version " << PACKAGE_VERSION << "\n";
            knownArgument = true;
            exit( 0 );
        }
        if( argStr == "-h" ){ //show Options & exit
            std::cout << PACKAGE_NAME << " version " << PACKAGE_VERSION << "\n";
            std::cout << "Known arguments are:\n";
            std::cout << "-v show version and exit\n";
            std::cout << "-h show his text and exit\n";
            std::cout << "-gl use OpenGL\n";
            std::cout << "-sdl use SDL (this is default)\n";
            knownArgument = true;
            exit( 0 );
        }
        if( argStr == "-gl" ){ //use OpenGL
            getConfig()->useOpenGL = true; 
            knownArgument = true;
        }
        if( argStr == "-sdl" ){ //use SGL
            getConfig()->useOpenGL = false; 
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
        initVideo(getConfig()->videoX, getConfig()->videoY );
	    initLincity();

        if( getConfig()->useOpenGL ) {
            texture_manager = new TextureManagerGL();
        } else {
            texture_manager = new TextureManagerSDL();
        }
        
        std::cout << "MainLoop\n";
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

    delete painter;
    delete texture_manager;
    if(TTF_WasInit())
        TTF_Quit();
    if(SDL_WasInit(0))
        SDL_Quit();
    PHYSFS_deinit();
    return result;
}

