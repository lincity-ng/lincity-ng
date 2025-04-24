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

#include "TextureSDL.hpp"

#include <SDL.h>
#include <cassert>
#include <stdexcept>
#include <cstddef>

TextureSDL::TextureSDL(SDL_Texture *tx) : tx(tx) {
  assert(tx);
  if(SDL_QueryTexture(tx, NULL, NULL, &width, &height))
    throw std::runtime_error(SDL_GetError());
  assert(width && height);
}

TextureSDL::~TextureSDL() {
  SDL_DestroyTexture(tx);
}

/** @file gui/PainterSDL/TextureSDL.cpp */
