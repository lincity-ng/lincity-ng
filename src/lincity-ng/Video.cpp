/* ---------------------------------------------------------------------- *
 * src/lincity-ng/Video.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2026      David Bears <dbear4q@gmail.com>
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

#include "Video.hpp"

#include <SDL3/SDL.h>                            // for SDL_Window, SDL_Crea...
#include <fmt/base.h>                            // for println
#include <cstdio>                                // for stderr
#include <optional>                              // for optional

#include "Config.hpp"                            // for Config, getConfig
#include "config.h"                              // for PACKAGE_NAME, PACKAG...
#include "gui/FontManager.hpp"                   // for FontManager, fontManager
#include "gui/Painter.hpp"                       // for Painter
#include "gui/PainterSDL/PainterSDL.hpp"         // for PainterSDL
#include "gui/PainterSDL/TextureManagerSDL.hpp"  // for TextureManagerSDL
#include "gui/TextureManager.hpp"                // for texture_manager, Tex...
#include "gui/Vector2.hpp"                       // for Vector2

#ifndef DISABLE_GL_MODE
#include <SDL3/SDL_opengl.h>                     // for glDisable, glLoadIde...

#include "gui/PainterGL/PainterGL.hpp"           // for PainterGL
#include "gui/PainterGL/TextureManagerGL.hpp"    // for TextureManagerGL
#endif

SDL_Window* window = NULL;
SDL_Renderer* window_renderer = NULL;
Painter* painter = nullptr;

#ifndef DISABLE_GL_MODE
SDL_GLContext window_context = NULL;
#endif


Vector2 getVirtualWindowSize(SDL_Window *window) {
  float scale = SDL_GetWindowDisplayScale(window);
  int width, height;
  SDL_GetWindowSizeInPixels(window, &width, &height);
  return Vector2(width, height) / scale;
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

void initVideo() {
  Uint32 flags =
    SDL_WINDOW_RESIZABLE |
    SDL_WINDOW_HIGH_PIXEL_DENSITY;

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
      flags |= SDL_WINDOW_FULLSCREEN;

  window = SDL_CreateWindow(PACKAGE_NAME " " PACKAGE_VERSION,
    getConfig()->videoX.get(), getConfig()->videoY.get(), flags);

  if(getConfig()->videoX.isDefault() && getConfig()->videoY.isDefault()) {
    // default window size assumed no scaling before the window is created
    float scale = SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(window));
    if(!scale) {
      fmt::println(stderr, "error: SDL_GetDisplayContentScale: {}",
        SDL_GetError());
      scale = 1.f;
    }
    getConfig()->videoX.session = getConfig()->videoX.get() * scale;
    getConfig()->videoY.session = getConfig()->videoY.get() * scale;
    SDL_SetWindowSize(window,
      getConfig()->videoX.get(),
      getConfig()->videoY.get()
    );
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
    window_renderer = SDL_CreateRenderer(window, nullptr);
    painter = new PainterSDL(window_renderer);
    texture_manager = new TextureManagerSDL(window_renderer);
  }

  fontManager = new FontManager();
}

void deinitVideo() {
  delete painter;
  delete fontManager;
  delete texture_manager;
}
