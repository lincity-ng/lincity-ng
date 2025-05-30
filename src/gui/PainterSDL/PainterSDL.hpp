/* ---------------------------------------------------------------------- *
 * src/gui/PainterSDL/PainterSDL.hpp
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

#ifndef __PAINTERSDL_HPP__
#define __PAINTERSDL_HPP__

#include <SDL.h>           // for SDL_Renderer
#include <cmath>           // for INFINITY
#include <deque>           // for deque
#include <vector>          // for vector

#include "../Color.hpp"       // for Color
#include "../Painter.hpp"     // for Painter
#include "../Vector2.hpp"     // for Vector2
#include "../Rect2D.hpp"      // for Rect2D

class TextureSDL;

/**
 * This class is needed to perform drawing operations. It contains a stack of
 * trasnformations (currently only translation) which is applied to the drawing
 * operations. This is usefull for child widgets in the gui-component tree.
 */
class PainterSDL : public Painter
{
public:
  PainterSDL(SDL_Renderer* target);
  virtual ~PainterSDL();

  void drawTexture(const Texture* texture, Vector2 pos) override;
  void drawStretchTexture(const Texture *texture, const Rect2D& rect) override;
  void fillRectangle(const Rect2D& rect) override;
  void drawRectangle(const Rect2D& rect) override;
  void fillPolygon(int numberPoints, const Vector2* points) override;
  void drawPolygon(int numberPoints, const Vector2* points) override;
  void drawLine(Vector2 pointA, Vector2 pointB) override;
  void clear() override;

  void setFillColor(Color color) override;
  void setLineColor(Color color) override;

  std::unique_ptr<Texture> createTargetTexture(int w, int h) override;
  void pushRenderTarget(Texture *target) override;
  void popRenderTarget() override;

  void updateScreen() override;

  void translate(Vector2 tl) override;
  void pushTransform() override;
  void popTransform() override;

  void pushClipRect(const Rect2D& region) override;
  void popClipRect() override;


private:
  SDL_Renderer* renderer;

  struct Transform {
    Vector2 translation;

    Vector2 apply(const Vector2& v) const {
      return v - translation;
    }
  };
  // the stack used by push-/popTransform
  std::vector<Transform> transformStack;
  // the currently active transform
  Transform transform;

  std::deque<TextureSDL *> targetStack;

  std::deque<Rect2D> cliprectStack;
  void updateClipRect();
  static constexpr Rect2D CR_NONE = Rect2D(0,0,INFINITY,INFINITY);

  Color fillColor,lineColor;
};

#endif

/** @file gui/PainterSDL/PainterSDL.hpp */
