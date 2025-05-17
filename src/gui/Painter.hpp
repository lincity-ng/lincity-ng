/* ---------------------------------------------------------------------- *
 * src/gui/Painter.hpp
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

#ifndef __PAINTER_HPP__
#define __PAINTER_HPP__

#include <SDL.h>
#include <vector>
#include <memory>

#include "Rect2D.hpp"
#include "TextureManager.hpp"
#include "Color.hpp"

/**
 * @class Painter
 * @brief This class is needed to perform drawing operations.
 *
 * It contains a stack of
 * trasnformations (currently only translation) which is applied to the drawing
 * operations. This is usefull for child widgets in the gui-component tree.
 */
class Painter {
public:
  virtual ~Painter() { }

  // drawing
  virtual void drawTexture(const Texture* texture, Vector2 pos) = 0;
  virtual void drawStretchTexture(const Texture *texture, const Rect2D& rect) = 0;
  virtual void fillRectangle(const Rect2D& rect) = 0;
  virtual void drawRectangle(const Rect2D& rect) = 0;
  virtual void fillPolygon(int numberPoints, const Vector2 *points) = 0;
  virtual void drawPolygon(int numberPoints, const Vector2 *points) = 0;
  virtual void drawLine(Vector2 pointA, Vector2 pointB) = 0;
  virtual void clear() = 0;

  virtual void setFillColor(Color color) = 0;
  virtual void setLineColor(Color color) = 0;

  // render target
  virtual std::unique_ptr<Texture> createTargetTexture(int w, int h) = 0;
  virtual void pushRenderTarget(Texture *target) = 0;
  virtual void popRenderTarget() = 0;

  virtual void updateScreen() = 0;

  // transform
  virtual void translate(Vector2 tl) = 0;
  virtual void pushTransform() = 0;
  virtual void popTransform() = 0;

  // clipRect
  virtual void pushClipRect(const Rect2D& clipRect) = 0;
  virtual void popClipRect() = 0;
};

#endif

/** @file gui/Painter.hpp */
