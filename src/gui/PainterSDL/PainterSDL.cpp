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
#include "PainterSDL.hpp"

#include <SDL.h>                 // for Sint16, SDL_Rect, SDL_CreateTextureF...
#include <cassert>               // for assert
#include <stdlib.h>              // for NULL

#include "Vector2.hpp"        // for Vector2
#include "Color.hpp"             // for Color
#include "Rect2D.hpp"            // for Rect2D
#include "Texture.hpp"           // for Texture
#include "TextureSDL.hpp"        // for TextureSDL

#ifndef NDEBUG
#include <typeinfo>              // for type_info
#endif

#ifdef _MSC_VER
#define lrint(x) (long int)x
#define lroundf(x) (long int)(x + .5)
#endif

PainterSDL::PainterSDL(SDL_Renderer* _renderer)
  : renderer(_renderer)
{ }

PainterSDL::~PainterSDL() { }

void
PainterSDL::drawTexture(const Texture *texture, const Vector2& pos) {
  drawStretchTexture(texture, Rect2D(pos, pos + texture->getSize()));
}

void
PainterSDL::drawStretchTexture(const Texture *texture, const Rect2D& rect)
{
    assert(typeid(*texture) == typeid(TextureSDL));
    const TextureSDL *textureSDL = static_cast<const TextureSDL *>(texture);

#ifdef DEBUG_ALL
    if(texture == 0 || texture->getWidth() == 0 || texture->getHeight() == 0) {
        std::cerr << "Trying to render 0 texture.";
        assert(false);
        return;
    }
#endif

    Vector2 screenpos = transform.apply(rect.p1);
    SDL_FRect drect;
    drect.x = screenpos.x;
    drect.y = screenpos.y;
    drect.w = rect.getWidth();
    drect.h = rect.getHeight();

    SDL_RenderCopyF(renderer, textureSDL->tx, NULL, &drect);
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

  SDL_RenderGeometryRaw(renderer, NULL,
    xy, sizeof(float) * 2, &color, 0, NULL, 0, numberPoints,
    indices, (numberPoints - 2) * 3, 4);

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
  SDL_SetRenderDrawColor(renderer,
    lineColor.r, lineColor.g, lineColor.b, lineColor.a);
  SDL_RenderDrawLinesF(renderer, screenPoints, numberPoints + 1);
  delete[] screenPoints;
}

void
PainterSDL::drawLine(const Vector2 pointA, const Vector2 pointB) {
  Vector2 screenposA = transform.apply(pointA);
  Vector2 screenposB = transform.apply(pointB);
  SDL_SetRenderDrawColor(renderer,
    lineColor.r, lineColor.g, lineColor.b, lineColor.a);
  SDL_RenderDrawLineF(renderer,
    screenposA.x, screenposA.y,
    screenposB.x, screenposB.y);
}

void
PainterSDL::makeSDLRect(const Rect2D& src, SDL_FRect& dst) const {
  Vector2 p = transform.apply(src.p1);
  Vector2 s = src.p2 - src.p1;
  dst = (SDL_FRect){.x = p.x, .y = p.y, .w = s.x, .h = s.y};
}

void
PainterSDL::fillRectangle(const Rect2D& rect) {
  SDL_FRect screenRect; makeSDLRect(rect, screenRect);
  SDL_SetRenderDrawColor(renderer,
    fillColor.r, fillColor.g, fillColor.b, fillColor.a);
  SDL_RenderFillRectF(renderer, &screenRect);
}

void
PainterSDL::drawRectangle(const Rect2D& rect) {
  SDL_FRect screenRect; makeSDLRect(rect, screenRect);
  SDL_SetRenderDrawColor(renderer,
    lineColor.r, lineColor.g, lineColor.b, lineColor.a);
  SDL_RenderDrawRectF(renderer, &screenRect);
}

void
PainterSDL::setFillColor(Color color)
{
    fillColor = color;
}

void
PainterSDL::setLineColor(Color color)
{
    lineColor = color;
}

void
PainterSDL::translate(const Vector2& vec)
{
    transform.translation -= vec;
}

void
PainterSDL::pushTransform()
{
    transformStack.push_back(transform);
}

void
PainterSDL::popTransform()
{
    transform = transformStack.back();
    transformStack.pop_back();
}

void
PainterSDL::setClipRectangle(const Rect2D& rect)
{
    Vector2 screenpos = transform.apply(rect.p1);
    SDL_Rect cliprect;
    cliprect.x = (int) screenpos.x;
    cliprect.y = (int) screenpos.y;
    cliprect.w = (int) rect.getWidth();
    cliprect.h = (int) rect.getHeight();
    SDL_RenderSetClipRect(renderer, &cliprect);
}

void
PainterSDL::clearClipRectangle()
{
    SDL_RenderSetClipRect(renderer, NULL);
}

Painter*
PainterSDL::createTexturePainter(Texture* texture)
{
    assert(typeid(*texture) == typeid(TextureSDL));
    TextureSDL* textureSDL = static_cast<TextureSDL*> (texture);

    // return new PainterSDL(textureSDL);
    return NULL;
}

void
PainterSDL::updateScreen()
{
    SDL_RenderPresent(renderer);
}


/** @file gui/PainterSDL/PainterSDL.cpp */
