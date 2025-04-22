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
#include <SDL2_gfxPrimitives.h>  // for aalineRGBA, aapolygonRGBA, boxRGBA
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


void
PainterSDL::fillPolygon(int numberPoints, const Vector2* points)
{
    Vector2 screenpos;
    Sint16* vx = new Sint16[numberPoints];
    Sint16* vy = new Sint16[numberPoints];
    for(int i = 0; i < numberPoints; i++ ) {
         screenpos = transform.apply( points[ i ] );
         vx[ i ] = (int) screenpos.x;
         vy[ i ] = (int) screenpos.y;
    }
    filledPolygonRGBA( renderer, vx, vy, numberPoints,
            fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    delete[] vx;
    delete[] vy;
}

void
PainterSDL::drawPolygon(int numberPoints, const Vector2* points)
{
    Vector2 screenpos;
    Sint16* vx = new Sint16[numberPoints];
    Sint16* vy = new Sint16[numberPoints];
    for(int i = 0; i < numberPoints; i++ ) {
         screenpos = transform.apply( points[ i ] );
         vx[ i ] = (int) screenpos.x;
         vy[ i ] = (int) screenpos.y;
    }
    aapolygonRGBA( renderer, vx, vy, numberPoints,
            lineColor.r, lineColor.g, lineColor.b, lineColor.a);
    delete[] vx;
    delete[] vy;
}

void
PainterSDL::drawLine( const Vector2 pointA, const Vector2 pointB )
{
    Vector2 screenpos = transform.apply( pointA );
    Vector2 screenpos2 = transform.apply( pointB );
    aalineRGBA( renderer, (int) screenpos.x, (int) screenpos.y,
            (int) screenpos2.x, (int) screenpos2.y,
      lineColor.r, lineColor.g, lineColor.b, lineColor.a);


}

void
PainterSDL::fillRectangle(const Rect2D& rect)
{
    Vector2 screenpos = transform.apply(rect.p1);
    Vector2 screenpos2 = transform.apply(rect.p2);
    boxRGBA(renderer, (int) screenpos.x, (int) screenpos.y,
            (int) screenpos2.x, (int) screenpos2.y,
            fillColor.r, fillColor.g, fillColor.b, fillColor.a);
}

void
PainterSDL::drawRectangle(const Rect2D& rect)
{
    Vector2 screenpos = transform.apply(rect.p1);
    Vector2 screenpos2 = transform.apply(rect.p2);
    rectangleRGBA(renderer, (int) screenpos.x, (int) screenpos.y,
            (int) screenpos2.x, (int) screenpos2.y,
            lineColor.r, lineColor.g, lineColor.b, lineColor.a);
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
