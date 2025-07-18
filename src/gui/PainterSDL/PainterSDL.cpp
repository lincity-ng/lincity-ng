/* ---------------------------------------------------------------------- *
 * src/gui/PainterSDL/PainterSDL.cpp
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

#include "PainterSDL.hpp"

#include <SDL.h>           // for SDL_GetError, SDL_SetRenderDrawColor, SDL_...
#include <algorithm>       // for max, min
#include <cassert>         // for assert
#include <cmath>           // for lround
#include <cstdlib>         // for NULL
#include <memory>          // for unique_ptr
#include <stdexcept>       // for runtime_error
#include <string>          // for basic_string, operator+, string

#include "Color.hpp"       // for Color
#include "Rect2D.hpp"      // for Rect2D
#include "Texture.hpp"     // for Texture
#include "TextureSDL.hpp"  // for TextureSDL
#include "Vector2.hpp"     // for Vector2

#ifndef NDEBUG
#include <typeinfo>        // for type_info
#endif

#define HANDLE_ERR(SDL_CALL) do { \
  if(SDL_CALL) \
    throw std::runtime_error(std::string(#SDL_CALL": ") + SDL_GetError()); \
} while(0)

PainterSDL::PainterSDL(SDL_Renderer* _renderer) :
  renderer(_renderer)
{
  cliprectStack.push_back(CR_NONE);
  HANDLE_ERR(SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND));
}

PainterSDL::~PainterSDL() { }

void
PainterSDL::drawTexture(const Texture *texture, Vector2 pos) {
  assert(texture);
  assert(typeid(*texture) == typeid(TextureSDL));
  const TextureSDL *textureSDL = static_cast<const TextureSDL *>(texture);

  // We need to round the screen position to avoid sampling on pixel boundaries.
  // Really, we only need to do this when NEAREST sampling is used.
  Vector2 screenpos = transform.apply(pos);
  SDL_FRect drect = {
    .x = std::round(screenpos.x),
    .y = std::round(screenpos.y),
    .w = (float)texture->getWidth(),
    .h = (float)texture->getHeight(),
  };

  HANDLE_ERR(SDL_RenderCopyF(renderer, textureSDL->tx, NULL, &drect));
}

void
PainterSDL::drawStretchTexture(const Texture *texture, const Rect2D& rect) {
  assert(texture);
  assert(typeid(*texture) == typeid(TextureSDL));
  const TextureSDL *textureSDL = static_cast<const TextureSDL *>(texture);

  Vector2 screenpos = transform.apply(rect.p1);
  SDL_FRect drect = {
    .x = screenpos.x,
    .y = screenpos.y,
    .w = rect.getWidth(),
    .h = rect.getHeight(),
  };

  HANDLE_ERR(SDL_RenderCopyF(renderer, textureSDL->tx, NULL, &drect));
}

/**
 * Note: This function assumes the polygon is convex.
**/
void
PainterSDL::fillPolygon(int numberPoints, const Vector2* points) {
  assert(numberPoints >= 3);

  SDL_Color color = (SDL_Color)
    {.r = fillColor.r, .g = fillColor.g, .b = fillColor.b, .a = fillColor.a};

  float *xy = new float[numberPoints * 2];
  for(int i = 0; i < numberPoints; i++) {
    Vector2 p = transform.apply(points[i]);
    xy[i * 2 + 0] = p.x;
    xy[i * 2 + 1] = p.y;
  }

  int *indices = new int[(numberPoints - 2) * 3];
  indices[0] = 0;
  indices[1] = 1;
  indices[2] = numberPoints - 1;
  for(int i = 3; i < (numberPoints - 2) * 3; i += 3) {
    indices[i + 0] = indices[i - 2];
    indices[i + 1] = indices[i - 1];
    indices[i + 2] = indices[i - 2] + (i << 1 & 2) - 1;
  }

  HANDLE_ERR(SDL_RenderGeometryRaw(renderer, NULL,
    xy, sizeof(float) * 2, &color, 0, NULL, 0, numberPoints,
    indices, (numberPoints - 2) * 3, 4));

  delete[] xy;
  delete[] indices;
}

void
PainterSDL::drawPolygon(int numberPoints, const Vector2* points) {
  SDL_FPoint *screenPoints = new SDL_FPoint[numberPoints + 1];
  for(int i = 0; i < numberPoints; i++) {
    Vector2 p = transform.apply(points[i]);
    screenPoints[i] = (SDL_FPoint){.x = p.x, .y = p.y};
  }
  screenPoints[numberPoints] = screenPoints[0]; // close the polygon
  HANDLE_ERR(SDL_SetRenderDrawColor(renderer,
    lineColor.r, lineColor.g, lineColor.b, lineColor.a));
  HANDLE_ERR(SDL_RenderDrawLinesF(renderer, screenPoints, numberPoints + 1));
  delete[] screenPoints;
}

void
PainterSDL::drawLine(Vector2 pointA, Vector2 pointB) {
  Vector2 screenposA = transform.apply(pointA);
  Vector2 screenposB = transform.apply(pointB);
  HANDLE_ERR(SDL_SetRenderDrawColor(renderer,
    lineColor.r, lineColor.g, lineColor.b, lineColor.a));
  HANDLE_ERR(SDL_RenderDrawLineF(renderer,
    screenposA.x, screenposA.y, screenposB.x, screenposB.y));
}

void
PainterSDL::fillRectangle(const Rect2D& rect) {
  Vector2 screenpos = transform.apply(rect.p1);
  SDL_FRect screenRect = {
    .x = screenpos.x,
    .y = screenpos.y,
    .w = rect.getWidth(),
    .h = rect.getHeight(),
  };
  HANDLE_ERR(SDL_SetRenderDrawColor(renderer,
    fillColor.r, fillColor.g, fillColor.b, fillColor.a));
  HANDLE_ERR(SDL_RenderFillRectF(renderer, &screenRect));
}

void
PainterSDL::drawRectangle(const Rect2D& rect) {
  Vector2 screenpos = transform.apply(rect.p1);
  SDL_FRect screenRect = {
    .x = screenpos.x,
    .y = screenpos.y,
    .w = rect.getWidth(),
    .h = rect.getHeight(),
  };
  HANDLE_ERR(SDL_SetRenderDrawColor(renderer,
    lineColor.r, lineColor.g, lineColor.b, lineColor.a));
  HANDLE_ERR(SDL_RenderDrawRectF(renderer, &screenRect));
}

void
PainterSDL::clear() {
  HANDLE_ERR(SDL_SetRenderDrawColor(renderer,
    fillColor.r, fillColor.g, fillColor.b, fillColor.a));
  HANDLE_ERR(SDL_RenderClear(renderer));
}

void
PainterSDL::setFillColor(Color color) {
  fillColor = color;
}

void
PainterSDL::setLineColor(Color color) {
  lineColor = color;
}

void
PainterSDL::updateScreen() {
  SDL_RenderPresent(renderer);
}

void
PainterSDL::translate(Vector2 tl) {
  transform.translation -= tl;
}

void
PainterSDL::pushTransform() {
  transformStack.push_back(transform);
}

void
PainterSDL::popTransform() {
  transform = transformStack.back();
  transformStack.pop_back();
}

void
PainterSDL::pushClipRect(const Rect2D& clipRect) {
  Rect2D newcr(
    transform.apply(clipRect.p1),
    transform.apply(clipRect.p2)
  );
  const Rect2D& oldcr = cliprectStack.back();
  if(oldcr != CR_NONE) {
    newcr = Rect2D(
      std::max(newcr.p1.x, oldcr.p1.x),
      std::max(newcr.p1.y, oldcr.p1.y),
      std::min(newcr.p2.x, oldcr.p2.x),
      std::min(newcr.p2.y, oldcr.p2.y)
    );
  }
  cliprectStack.push_back(newcr);
  updateClipRect();
}

void
PainterSDL::popClipRect() {
  cliprectStack.pop_back();
  updateClipRect();
}

void
PainterSDL::updateClipRect() {
  if(cliprectStack.back() == CR_NONE) {
    HANDLE_ERR(SDL_RenderSetClipRect(renderer, NULL));
  }
  else {
    SDL_Rect clip = {
      .x = (int)lround(cliprectStack.back().p1.x),
      .y = (int)lround(cliprectStack.back().p1.y),
      .w = (int)lround(cliprectStack.back().p2.x),
      .h = (int)lround(cliprectStack.back().p2.y),
    };
    clip.w -= clip.x;
    clip.h -= clip.y;
    HANDLE_ERR(SDL_RenderSetClipRect(renderer, &clip));
  }
}

std::unique_ptr<Texture>
PainterSDL::createTargetTexture(int width, int height) {
  SDL_Texture *texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, width, height);
  HANDLE_ERR(!texture);
  HANDLE_ERR(SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND));
  return std::unique_ptr<Texture>(new TextureSDL(texture));
}

void
PainterSDL::pushRenderTarget(Texture *target) {
  TextureSDL *t = dynamic_cast<TextureSDL *>(target);
  assert(t);
  #ifndef NDEBUG
  {
    int tflags;
    HANDLE_ERR(SDL_QueryTexture(t->tx, NULL, &tflags, NULL, NULL));
    assert(tflags == SDL_TEXTUREACCESS_TARGET);
  }
  {
    SDL_RendererInfo rinfo;
    HANDLE_ERR(SDL_GetRendererInfo(renderer, &rinfo));
    assert(rinfo.flags & SDL_RENDERER_TARGETTEXTURE);
  }
  #endif
  targetStack.push_back(t);
  HANDLE_ERR(SDL_SetRenderTarget(renderer, t->tx));

  cliprectStack.push_back(CR_NONE);
  HANDLE_ERR(SDL_RenderSetClipRect(renderer, NULL));

  transformStack.push_back(transform);
  transform = Transform();
}

void
PainterSDL::popRenderTarget() {
  targetStack.pop_back();
  HANDLE_ERR(SDL_SetRenderTarget(renderer,
    targetStack.empty() ? NULL : targetStack.back()->tx));

  cliprectStack.pop_back();
  updateClipRect();

  transform = transformStack.back();
  transformStack.pop_back();
}


/** @file gui/PainterSDL/PainterSDL.cpp */
