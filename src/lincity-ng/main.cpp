/* ---------------------------------------------------------------------- *
 * src/lincity-ng/main.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Matthias Braun <matze@braunis.de>
 * Copyright (C) 2024-2025 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "main.hpp"

#include <SDL.h>                                 // for SDL_GetError, SDL_Se...
#include <SDL_mixer.h>                           // for Mix_HookMusicFinished
#include <SDL_ttf.h>                             // for TTF_Init, TTF_Quit
#include <fmt/base.h>                            // for println
#include <fmt/format.h>
#include <gettext.h>                             // for bindtextdomain, text...
#include <libxml/xmlversion.h>                   // for LIBXML_VERSION
#include <cassert>                               // for assert
#include <clocale>                               // for NULL, setlocale, LC_ALL
#include <cstdio>                                // for stderr
#include <cstdlib>                               // for getenv, setenv, unse...
#include <cstring>                               // for strcmp
#include <filesystem>                            // for path, operator/
#include <iostream>                              // for basic_ostream, opera...
#include <memory>                                // for unique_ptr
#include <optional>                              // for optional, nullopt
#include <stdexcept>                             // for runtime_error
#include <string>                                // for basic_string, char_t...

#include "Config.hpp"                            // for getConfig, Config
#include "MainLincity.hpp"                       // for initLincity
#include "MainMenu.hpp"                          // for MainMenu
#include "Sound.hpp"                             // for Sound, getSound, Mus...
#include "config.h"                              // for PACKAGE_NAME, HAVE_N...
#include "gui/FontManager.hpp"                   // for FontManager, fontMan...
#include "gui/Painter.hpp"                       // for Painter
#include "gui/PainterSDL/PainterSDL.hpp"         // for PainterSDL
#include "gui/PainterSDL/TextureManagerSDL.hpp"  // for TextureManagerSDL
#include "gui/TextureManager.hpp"                // for texture_manager, Tex...
#include "util/gettextutil.hpp"                  // for _

#if LIBXML_VERSION < 21400
#include <libxml/parser.h>                       // for xmlInitParser, xmlCl...
#endif

#ifndef DISABLE_GL_MODE
#include <SDL_opengl.h>                          // for glDisable, glLoadIde...

#include "gui/PainterGL/PainterGL.hpp"           // for PainterGL
#include "gui/PainterGL/TextureManagerGL.hpp"    // for TextureManagerGL
#endif

#ifndef DEBUG
#include <exception>                             // for exception
#endif

#ifdef WIN32
#undef bindtextdomain
#define bindtextdomain wbindtextdomain
#endif

extern Config *configPtr;

SDL_Window* window = NULL;
SDL_GLContext window_context = NULL;
SDL_Renderer* window_renderer = NULL;
Painter* painter = 0;
// bool restart = false;
const char *appdatadir;
std::optional<std::string> oldLanguage = std::nullopt;

void musicHalted() {
    getSound()->changeTrack(NEXT_OR_FIRST_TRACK);
    //FIXME: options menu song entry doesn't update while song changes.
}

void videoSizeChanged(int width, int height) {
#ifndef DISABLE_GL_MODE
    if(getConfig()->useOpenGL.get()) {
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
#endif
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
#ifndef DISABLE_GL_MODE
    if(getConfig()->useOpenGL.get()) {
        flags |= SDL_WINDOW_OPENGL;
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 1);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 1);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 1);
        //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
        //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    }
#endif
    if(getConfig()->useFullScreen.get())
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    window = SDL_CreateWindow(PACKAGE_NAME " " PACKAGE_VERSION,
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, width, height,
                              flags);

    if(getConfig()->useFullScreen.get()) {
      // actual window size may be different than requested
      SDL_GetWindowSize(window, &width, &height);
      getConfig()->videoX.session = width;
      getConfig()->videoY.session = height;
    }

#ifndef DISABLE_GL_MODE
    if(getConfig()->useOpenGL.get()) {
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
    }
    else
#endif
    {
        window_renderer = SDL_CreateRenderer(window, -1, 0);

        painter = new PainterSDL(window_renderer);
        std::cout << "\nSDL Mode " << width;
        std::cout << "x"<< height <<"\n";

        texture_manager = new TextureManagerSDL(window_renderer);
    }

    fontManager = new FontManager();
}

#ifdef HAVE_NL_MSG_CAT_CNTR
// glibc gettext magic
extern "C" int _nl_msg_cat_cntr;
#endif
void
setLang(const std::string& lang) {
  if(lang != "autodetect") {
#ifdef WIN32
    _putenv_s("LANGUAGE", lang.c_str());
#else
    setenv("LANGUAGE", lang.c_str(), 1);
#endif
  }
  else if(oldLanguage) {
#ifdef WIN32
    _putenv_s("LANGUAGE", oldLanguage->c_str());
#else
    setenv("LANGUAGE", oldLanguage->c_str(), 1);
#endif
  }
  else {
#ifdef WIN32
    _putenv_s("LANGUAGE", "");
#else
    unsetenv("LANGUAGE");
#endif
  }
#define GETTEXT_HAS_

#ifdef HAVE_NL_MSG_CAT_CNTR
  // glibc gettext magic
  ++_nl_msg_cat_cntr;
#endif
}

// This tries to get the same language that as gettext.
std::string
getLang() {
#if ENABLE_NLS
  const char *locale = setlocale(LC_MESSAGES, NULL);
#else
  const char *locale = "C.UTF-8";
#endif
  assert(locale);
  if(locale && !strcmp(locale, "C")) return "C";
  const char *language = getenv("LANGUAGE");
  if(language && *language) return language;
  if(locale) return locale;
  return "";
}

void mainLoop() {
  MainMenu(window).run();
}

int
main(int argc, char** argv) {
  // initialize XML parser early because it is needed for parsing the config
  LIBXML_TEST_VERSION;
#if LIBXML_VERSION < 21400
  xmlInitParser();
#endif

  // parse config and command line
  configPtr = new Config();
  getConfig()->init(argc, argv);

  // set the preferred language
#if ENABLE_NLS
  if(const char *old = getenv("LANGUAGE")) oldLanguage = old;
  setlocale(LC_ALL, "");
  if(getConfig()->language.get() != "autodetect")
    setLang(getConfig()->language.get());
  bindtextdomain(PACKAGE_NAME,
    (getConfig()->appDataDir.get() / "locale").c_str());
  textdomain(PACKAGE_NAME);
#endif

  // show versions or help message if requested
  if(getConfig()->showVersion.get()) {
    fmt::println(PRETTY_NAME_VERSION);
    return 0;
  }
  if(getConfig()->showHelp.get()) {
    Config::printHelp(argv[0]);
    return 0;
  }

  // print welcome message
  fmt::println(stderr, _("starting {} ..."), PRETTY_NAME_VERSION);

  // initialize resources
  constexpr Uint32 sdlSubsystems =
    SDL_INIT_TIMER |
    SDL_INIT_AUDIO |
    SDL_INIT_VIDEO |
    SDL_INIT_EVENTS;
  if(SDL_Init(sdlSubsystems) < 0)
    throw std::runtime_error(fmt::format(
      "failed to initialize SDL: {}", SDL_GetError()));
  if(TTF_Init() < 0)
    throw std::runtime_error(fmt::format(
      "failed to initialize SDL_ttf: {}", TTF_GetError()));
  SDL_SetHint(SDL_HINT_APP_NAME, PRETTY_NAME);
  SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
  initVideo(getConfig()->videoX.get(), getConfig()->videoY.get());
  initLincity();
  std::unique_ptr<Sound> sound(new Sound());
  Mix_HookMusicFinished(musicHalted);

  // enter main loop
  mainLoop();

  // save the current game
  getConfig()->save();

  // clean up
  delete painter;
  delete fontManager;
  delete texture_manager;
  TTF_Quit();
  SDL_Quit();
#if LIBXML_VERSION < 20911
  xmlCleanupParser();
#endif

  return 0;
}


/** @file lincity-ng/main.cpp */
