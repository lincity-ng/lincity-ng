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
#include <physfs.h>                              // for PHYSFS_mount, PHYSFS...
#include <stdio.h>                               // for NULL, printf, sprintf
#include <stdlib.h>                              // for exit, malloc, free
#include <string.h>                              // for strlen, strncmp, strdup
#include <unistd.h>                              // for execlp
#include <iostream>                              // for operator<<, basic_os...
#include <memory>                                // for allocator, unique_ptr
#include <sstream>                               // for basic_ostringstream
#include <stdexcept>                             // for runtime_error
#include <string>                                // for char_traits, basic_s...

#include "Config.hpp"                            // for getConfig, Config
#include "Game.hpp"                              // for Game
#include "MainLincity.hpp"                       // for initLincity
#include "MainMenu.hpp"                          // for MainMenu
#include "PBar.hpp"                              // for LCPBarPage1, LCPBarP...
#include "PhysfsStream/PhysfsError.hpp"          // for getPhysfsLastError
#include "Sound.hpp"                             // for Sound, getSound, Mus...
#include "gui/FontManager.hpp"                   // for FontManager, fontMan...
#include "gui/Painter.hpp"                       // for Painter
#include "gui/PainterGL/PainterGL.hpp"           // for PainterGL
#include "gui/PainterGL/TextureManagerGL.hpp"    // for TextureManagerGL
#include "gui/PainterSDL/PainterSDL.hpp"         // for PainterSDL
#include "gui/PainterSDL/TextureManagerSDL.hpp"  // for TextureManagerSDL
#include "gui/TextureManager.hpp"                // for texture_manager, Tex...
#include "lc_error.h"                            // for HANDLE_ERRNO
#include "lincity/init_game.h"                   // for destroy_game
#include "tinygettext/tinygettext.hpp"           // for DictionaryManager

#ifndef DEBUG
#include <exception>                             // for exception
#endif

SDL_Window* window = NULL;
SDL_GLContext window_context = NULL;
SDL_Renderer* window_renderer = NULL;
Painter* painter = 0;
tinygettext::DictionaryManager* dictionaryManager = 0;
bool restart = false;
const char *appdatadir;

#ifdef __APPLE__
     extern char *getBundleSharePath(char *packageName);
#endif

#ifdef WIN32
static char *strndup(const char *s, size_t n) {
  n = strnlen(s, n);
  char *d;
  if(d = (char *)malloc((n + 1) * sizeof(char)))
    strncpy(d, s, n)[n] = '\0';
  return d;
}
#endif

/**
 * Computes the path of the root of `subtrahend` relative to the root of
 * `minuend` assuming `subtrahend` and `minuend` refer to the same location.
 *
 * That is, `<root><minuend>` and `<root><result>/<subtrahend>` will refer
 * to the same location. The result will be absolute iff minuend is absolute.
 *
 * The result string is owned by the caller and should be freed with `free()`.
 *
 * Examples:
 *  path_subtract("/a/b/c/d", "c/d") --> "/a/b"
 *  path_subtract("/a/b/c/d", "/a/b/c/d") --> "/"
 *  path_subtract("c/d", "/a/b/c/d") --> "../.."
 *  path_subtract("../c/d", "/a/b/c/d") --> "../../.."
 *  path_subtract("a//x/../b/////c/.//y/.././d", "z/../c//.//d") --> "a//x/../b"
 *  path_subtract("a/b/c/x/y/z/../../../d", "c/d") --> "a/b"
 *  path_subtract("/a/b/c/d", "x/d") --> NULL (cannot refer to the same file)
 *  path_subtract("a/b/c/d", "/c/d") --> NULL (minuend goes below root)
 *  path_subtract("/c/d", "a/b/c/d") --> NULL (subtrahend goes below root)
 *  path_subtract("/a/b/c/d", "../c/d") --> NULL (cannot determine '..')
 *  path_subtract("/../a/b/c/d", "c/d") --> NULL (minuend dips below root)
 *  path_subtract("c/d", "/../a/b/c/d") --> NULL (subtrahend dips below root)
 */
static char *path_subtract(const char *minuend, const char *subtrahend) {
  const char * const dirsep = PHYSFS_getDirSeparator();
  const size_t dirseplen = strlen(dirsep);

  const char *path[2] = {minuend, subtrahend};
  const char *elb[2];
  const char *ele[2];
  size_t eln[2];
  bool finished[2] = {false, false};
  for(int i = 0; i < 2; i++)
    elb[i] = path[i] + strlen(path[i]) + dirseplen;
  int skip[2] = {0, 0};

  while(true) {
    for(int i = 0; i < 2; i++) {
      while(true) {
        ele[i] = elb[i] - dirseplen;
        if(ele[i] <= path[i]) {
          finished[i] = true;
          break;
        }
        elb[i] = ele[i] - dirseplen;
        while(true) {
          if(elb[i] < path[i]) {
            elb[i] = path[i];
            break;
          }
          if(!strncmp(elb[i], dirsep, dirseplen)) {
            elb[i] += dirseplen;
            break;
          }
          elb[i]--;
        }

        eln[i] = ele[i] - elb[i];
        if(!strncmp(elb[i], "", eln[i]) || !strncmp(elb[i], ".", eln[i]))
          ;
        else if(!strncmp(elb[i], "..", eln[i]))
          skip[i]++;
        else if(skip[i])
          skip[i]--;
        else
          break;
      }
    }

    if(finished[1]) {
      if(skip[1])
        return NULL;
      if(skip[0] && !strncmp(minuend, dirsep, dirseplen))
        return NULL;
      if(!finished[0] && !strncmp(subtrahend, dirsep, dirseplen))
        return NULL;
      if(skip[0]) {
        char *ret = (char *)malloc(skip[0] * (2 + dirseplen));
        if(!ret) return NULL;
        char *retptr = ret;
        for(int i = 0; i < skip[0]; i++) {
          retptr += sprintf(retptr, "%s..", i ? dirsep : "");
        }
        return ret;
      }
      if(finished[0])
        return strdup(strncmp(minuend, dirsep, dirseplen) ? "." : dirsep);
      return strndup(minuend, ele[0] - minuend);
    }
    else if(finished[0]) {
      skip[0]++;
    }
    else if(eln[0] != eln[1] || strncmp(elb[0], elb[1], eln[0])) {
      return NULL;
    }
  }
}

void initPhysfs(const char* argv0)
{
    if(!PHYSFS_init(argv0)) {
        std::stringstream msg;
        msg << "Couldn't initialize physfs: "
            << getPhysfsLastError();
        throw std::runtime_error(msg.str());
    }

    // Initialize physfs (this is a slightly modified version of
    // PHYSFS_setSaneConfig
    const char* writedir = PHYSFS_getPrefDir("lincity-ng", "lincity-ng");
    if(!writedir) {
      std::ostringstream msg;
      // Unfortunately, PHYSFS_getPrefDir does not expose the path name if
      // creating the directory failed.
      msg << "Failed to get configuration directory '";
      throw std::runtime_error(msg.str());
    }

    // enable writing to configuration directory
    if(!PHYSFS_setWriteDir(writedir)) {
        std::ostringstream msg;
        msg << "Failed to enable writing to configuration directory '"
            << writedir << "': " << getPhysfsLastError();
        throw std::runtime_error(msg.str());
    }

    // mount configuration directory
    if(!PHYSFS_mount(writedir, nullptr, 0)) {
        std::ostringstream msg;
        msg << "Failed to mount configuration directory '"
            << writedir << "': " << getPhysfsLastError();
        throw std::runtime_error(msg.str());
    }

    // compute install prefix from PHYSFS_getBaseDir()
    const char* dirsep = PHYSFS_getDirSeparator();
    char *installPrefix = NULL;
    if(strncmp(INSTALL_BINDIR, dirsep, strlen(dirsep))) {
      HANDLE_ERRNO(
        installPrefix = path_subtract(PHYSFS_getBaseDir(), INSTALL_BINDIR),
        !installPrefix, 0, "trouble finding install prefix"
      );
    }


    // mount read-only data directory
    bool foundRodd = false;
    char *appdatadir_calc = NULL;
    if(installPrefix && strncmp(INSTALL_APPDATADIR, dirsep, strlen(dirsep))) {
      HANDLE_ERRNO(
        appdatadir_calc = (char *)malloc(
          strlen(installPrefix)
          + strlen(dirsep)
          + strlen(INSTALL_APPDATADIR)
          + 1),
        !appdatadir_calc, 0, "malloc"
      );
      if(appdatadir_calc) {
        sprintf(appdatadir_calc, "%s%s%s",
          installPrefix, dirsep, INSTALL_APPDATADIR);
        appdatadir = appdatadir_calc;
      }
    }
    if(appdatadir) {
      foundRodd = PHYSFS_mount(appdatadir, nullptr, 1);
    }

    if(!foundRodd) {
      // use compiled-in install prefix as fallback
      foundRodd = PHYSFS_mount(INSTALL_FULL_APPDATADIR, nullptr, 1);
      if(foundRodd)
        appdatadir = INSTALL_FULL_APPDATADIR;
    }
    if(!foundRodd) {
      std::ostringstream msg;
      msg << "Failed to mount read-only data directory '"
          << appdatadir << "' or '"
          << INSTALL_FULL_APPDATADIR
          << "': " << getPhysfsLastError();
      throw std::runtime_error(msg.str());
    }

    // free(appdatadir);
    free(installPrefix);


    // Search for archives and add them to the search path
    //TODO: add zips later
    const char* archiveExt = ".zip";
    char** rc = PHYSFS_enumerateFiles("/");
    size_t extlen = strlen(archiveExt);
    //TODO sort .zip files! so we are sure which patch is first.
    //and change all file access to physfs. what does PHYSFS_getRealDir
    //do when file in in archive?
    for(char** i = rc; *i != 0; ++i) {
        size_t l = strlen(*i);
        const char* ext = (*i) + (l - extlen);
        if(l >= extlen && !SDL_strcasecmp(ext, archiveExt)) {
            const char* d = PHYSFS_getRealDir(*i);
            char* str = new char[strlen(d) + strlen(dirsep) + l + 1];
            sprintf(str, "%s%s%s", d, dirsep, *i);
            PHYSFS_mount(str, nullptr, 1);
            delete[] str;
        }
    }
    PHYSFS_freeList(rc);

    // allow symbolic links
    PHYSFS_permitSymbolicLinks(1);

    //show search Path
    for(char** i = PHYSFS_getSearchPath(); *i != NULL; i++)
    {   printf("[%s] is in the search path.\n", *i);}
    //show write directory
    printf("[%s] is the write directory.\n", PHYSFS_getWriteDir());
}

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
            std::cout << "-v           --version         show version and exit\n";
            std::cout << "-h           --help            show this text and exit\n";
            std::cout << "-g           --gl              use OpenGL\n";
            std::cout << "-s           --sdl             use SDL\n";
            std::cout << "-S [size]    --size [size]     specify screensize (eg. -S 1024x768)\n";
            std::cout << "-w           --window          run in window\n";
            std::cout << "-f           --fullscreen      run fullscreen\n";
            std::cout << "-m           --mute            mute audio\n";
            std::cout << "                               -q 9 skips animation steps for speed.\n";
            std::cout << "                               -q 8 is the slowest speed with full animation.\n";
            std::cout << "                               -q 1 is fastest. It may heat your hardware!\n";
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

        if( getConfig()->language != "autodetect" ){
#if defined (WIN32)
            _putenv_s("LINCITY_LANG", getConfig()->language.c_str());
#else
            setenv("LINCITY_LANG", getConfig()->language.c_str(), false);
#endif
        }
        dictionaryManager = new tinygettext::DictionaryManager();
        dictionaryManager->set_charset("UTF-8");
        dictionaryManager->add_directory("locale");
        std::cout << "Language is \"" << dictionaryManager->get_language() << "\".\n";

        // char *lc_textdomain_directory[1024];
        // if(snprintf(lc_textdomain_directory, 1024, "%s%c%s",
        //   appdatadir, PHYSFS_getDirSeparator(), "locale") < 1024) {
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
    parseCommandLine(argc, argv); // Do not use getConfig() before parseCommandLine for anything command line might change.

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


/** @file lincity-ng/main.cpp */
