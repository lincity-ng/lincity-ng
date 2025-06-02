/* ---------------------------------------------------------------------- *
 * src/gui/PainterSDL/TextureManagerSDL.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Matthias Braun <matze@braunis.de>
 * Copyright (C) 2025      David Bears <dbear4q@gmail.com>
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

#include "TextureManagerSDL.hpp"

#include <SDL.h>           // for SDL_ConvertSurfaceFormat, SDL_FreeSurface
#include <stdexcept>

#include "TextureSDL.hpp"  // for TextureSDL

static const Uint8 ALPHA_BARRIER = 100;

TextureManagerSDL::TextureManagerSDL(SDL_Renderer *renderer)
  : renderer(renderer)
{}

TextureManagerSDL::~TextureManagerSDL()
{}

Texture *
TextureManagerSDL::create(SDL_Surface *image) {
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
  if(!texture) {
    throw std::runtime_error(SDL_GetError());
  }
  return new TextureSDL(texture);
}


/** @file gui/PainterSDL/TextureManagerSDL.cpp */
