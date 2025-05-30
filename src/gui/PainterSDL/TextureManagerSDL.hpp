/* ---------------------------------------------------------------------- *
 * src/gui/PainterSDL/TextureManagerSDL.hpp
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

#ifndef __TEXTUREMANAGERSDL_HPP__
#define __TEXTUREMANAGERSDL_HPP__

#include <SDL.h>                  // for SDL_Surface

#include "../TextureManager.hpp"  // for TextureManager

/**
 * This handles the creation and sharing of textures.
 *
 * A texture is a wrapper around a pixmaps. Using a wrapper allows to make use
 * of hardware acceleration in some scenarios (for example in opengl where the
 * pixmaps can get uploaded into the gfx cards memory).
 */
class TextureManagerSDL : public TextureManager
{
public:
    TextureManagerSDL(SDL_Renderer *renderer);
    virtual ~TextureManagerSDL();

    Texture* create(SDL_Surface* surface);

private:
  SDL_Renderer *renderer;
};

#endif

/** @file gui/PainterSDL/TextureManagerSDL.hpp */
