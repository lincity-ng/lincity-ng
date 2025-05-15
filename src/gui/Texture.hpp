/* ---------------------------------------------------------------------- *
 * src/gui/Texture.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Matthias Braun <matze@braunis.de>
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

/**
 * @author Matthias Braun
 * @file Texture.hpp
 */

#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

/**
 * @class Texture
 * @brief Wrapper around a pixmap.
 *
 * Texture have to be created by the TextureManager class.
 */
class Texture {
public:
  virtual ~Texture()
  { }

  virtual int getWidth() const = 0;
  virtual int getHeight() const = 0;

  enum class ScaleMode {
    NEAREST, LINEAR, ANISOTROPIC
  };
  virtual void setScaleMode(ScaleMode mode) = 0;
};

#endif

/** @file gui/Texture.hpp */
