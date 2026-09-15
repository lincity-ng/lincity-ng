/* ---------------------------------------------------------------------- *
 * src/lincity-ng/main.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Matthias Braun <matze@braunis.de>
 * Copyright (C) 2024-2025 David Bears <dbear4q@gmail.com>
 * Copyright (C) 2026      Marc Young <myoung008@gmail.com>
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

#include <SDL3/SDL.h>            // for SDL_GetError, SDL_SetHint, SDL_HINT_...
#include <SDL3_ttf/SDL_ttf.h>    // for TTF_Init, TTF_Quit
#include <fmt/base.h>            // for println
#include <fmt/format.h>          // for format
#include <gettext.h>             // for bindtextdomain, textdomain
#include <libxml/xmlversion.h>   // for LIBXML_VERSION, LIBXML_TEST_VERSION
#include <cassert>               // for assert
#include <clocale>               // for setlocale, LC_ALL, LC_MESSAGES, NULL
#include <cstdio>                // for stderr
#include <cstdlib>               // for getenv, setenv, unsetenv
#include <cstring>               // for strcmp
#include <filesystem>            // for path, operator/
#include <memory>                // for unique_ptr
#include <optional>              // for optional, nullopt, nullopt_t
#include <stdexcept>             // for runtime_error
#include <string>                // for basic_string, string, operator!=

#include "Config.hpp"            // for Config, getConfig
#include "MainLincity.hpp"       // for initLincity
#include "MainMenu.hpp"          // for MainMenu
#include "Sound.hpp"             // for Sound
#include "Video.hpp"             // for deinitVideo, initVideo, window
#include "config.h"              // for HAVE_NL_MSG_CAT_CNTR, PACKAGE_NAME, ...
#include "util/gettextutil.hpp"  // for _

#if LIBXML_VERSION < 21400
#include <libxml/parser.h>                       // for xmlInitParser, xmlCl...
#endif

#ifndef DISABLE_GL_MODE
#include <SDL3/SDL_opengl.h>                     // for glDisable, glLoadIde...

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

// bool restart = false;
const char *appdatadir;
std::optional<std::string> oldLanguage = std::nullopt;

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
    SDL_INIT_AUDIO |
    SDL_INIT_VIDEO |
    SDL_INIT_EVENTS;
  if( !SDL_Init(sdlSubsystems))
    throw std::runtime_error(fmt::format(
      "failed to initialize SDL: {}", SDL_GetError()));
  if( !TTF_Init())
    throw std::runtime_error(fmt::format(
      "failed to initialize SDL_ttf: {}", SDL_GetError()));
  SDL_SetHint(SDL_HINT_APP_NAME, PRETTY_NAME);
  SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
  initVideo();
  initLincity();
  std::unique_ptr<Sound> sound(new Sound());

  // enter main loop
  MainMenu(window).run();

  // save the configuration
  getConfig()->save();

  // clean up
  deinitVideo();
  TTF_Quit();
  SDL_Quit();
#if LIBXML_VERSION < 20911
  xmlCleanupParser();
#endif

  return 0;
}


/** @file lincity-ng/main.cpp */
