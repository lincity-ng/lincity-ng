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
#ifndef __TEXTURESDL_HPP__
#define __TEXTURESDL_HPP__

#include <SDL.h>    // for SDL_Surface, SDL_FreeSurface
#include <stddef.h>         // for NULL
#include <stdexcept>

#include "Texture.hpp"  // for Texture
#include "Vector2.hpp"

/**
 * Wrapper around a pixmap. Texture have to be created by the TextureManager
 * class
 */
class TextureSDL : public Texture
{
public:
    virtual ~TextureSDL();

    float getWidth() const {
      return getSize().x;
    }
    float getHeight() const {
      return getSize().y;
    }

    Vector2 getSize() const {
      if(!size.x) {
        int w = 0, h = 0;
        if(SDL_QueryTexture(tx, NULL, NULL, &w, &h)) {
          throw std::runtime_error(SDL_GetError());
        }
        size = Vector2((float)w, (float)h);
      }
      return size;
    }

private:
    friend class PainterSDL;
    friend class TextureManagerSDL;
    TextureSDL(SDL_Texture *tx) : tx(tx) { }

    SDL_Texture *tx;
    mutable Vector2 size;
};

#endif

/** @file gui/PainterSDL/TextureSDL.hpp */
