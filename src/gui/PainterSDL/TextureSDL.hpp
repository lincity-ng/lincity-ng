/* ---------------------------------------------------------------------- *
 * src/gui/PainterSDL/TextureSDL.hpp
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

#ifndef __TEXTURESDL_HPP__
#define __TEXTURESDL_HPP__

#include <SDL.h>           // for SDL_Texture

#include "../Texture.hpp"  // for Texture

class TextureSDL : public Texture {
public:
  virtual ~TextureSDL();

  int getWidth() const override {
    return width;
  }
  int getHeight() const override {
    return height;
  }

  virtual void setScaleMode(ScaleMode mode) override;

private:
  friend class PainterSDL;
  friend class TextureManagerSDL;
  TextureSDL(SDL_Texture *tx);

  SDL_Texture *tx = nullptr;
  mutable int width = 0, height = 0;
};

#endif

/** @file gui/PainterSDL/TextureSDL.hpp */
