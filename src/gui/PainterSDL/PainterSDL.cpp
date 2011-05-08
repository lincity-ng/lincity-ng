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
#include <config.h>

#include "PainterSDL.hpp"

#include <iostream>
#include <assert.h>
#include <math.h>
#include <typeinfo>
#include <SDL_opengl.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
#include <SDL_video.h>
#include <stdlib.h>

#include "TextureSDL.hpp"

#ifdef _MSC_VER
#define lrint(x) (long int)x
#define lroundf(x) (long int)(x + .5)
#endif

PainterSDL::PainterSDL(SDL_Surface* _target)
    : target(_target)
{
}

PainterSDL::PainterSDL(TextureSDL* texture)
    : target(texture->surface)
{
}

PainterSDL::~PainterSDL()
{
}

//ERM  this function seems to account for SOME of the slowdown
//This function draw a tile with zoom = 1
//i.e. with no transformation, so it is as fast as possible
void
PainterSDL::drawTexture(const Texture* texture, const Vector2& pos)
{
    assert(typeid(*texture) == typeid(TextureSDL));
    const TextureSDL* textureSDL = static_cast<const TextureSDL*> (texture);

#ifdef DEBUG_ALL
    if(texture == 0) {
        std::cerr << "Trying to render 0 texture.";
        assert(false);
        return;
    }
#endif

    Vector2 screenpos = transform.apply(pos);

    SDL_Rect drect;
    drect.x = lrint(screenpos.x);
    drect.y = lrint(screenpos.y);
    SDL_BlitSurface(textureSDL->surface, 0, target, &drect);
}

//RectIntersection checks to see if two SDL_Rects intersect each other
//  This is actually stolen from SDL code (SDL_HasIntersection function in SDL_rect.c),
//  but I think that's a very recent addition which is not accessible here.
bool RectIntersection(const SDL_Rect * A, const SDL_Rect * B)
{
    int Amin, Amax, Bmin, Bmax;

    // Horizontal intersection
    Amin = A->x;
    Amax = Amin + A->w;
    Bmin = B->x;
    Bmax = Bmin + B->w;
    if (Bmin > Amin)
        Amin = Bmin;
    if (Bmax < Amax)
        Amax = Bmax;
    if (Amax <= Amin)
        return false;

    // Vertical intersection
    Amin = A->y;
    Amax = Amin + A->h;
    Bmin = B->y;
    Bmax = Bmin + B->h;
    if (Bmin > Amin)
        Amin = Bmin;
    if (Bmax < Amax)
        Amax = Bmax;
    if (Amax <= Amin)
        return false;

    return true;
}

//ERM  this function seems to account for MOST of the slowdown
//AL1  this function is twice as slow as drawTexture
void
PainterSDL::drawStretchTexture(Texture* texture, const Rect2D& rect)
{
    assert(typeid(*texture) == typeid(TextureSDL));
    TextureSDL* textureSDL = static_cast< TextureSDL*> (texture);

#ifdef DEBUG_ALL
    if(texture == 0 || texture->getWidth() == 0 || texture->getHeight() == 0) {
        std::cerr << "Trying to render 0 texture.";
        assert(false);
        return;
    }
#endif

    Vector2 screenpos = transform.apply(rect.p1);

    SDL_Rect drect, cliprect;
    drect.x = lroundf(screenpos.x);
    drect.y = lroundf(screenpos.y);
    // kinda hacky... but zoomSurface sometimes produces 1 pixel too small
    // images
    drect.w = lroundf(rect.getWidth()) /*+ 1*/;
    drect.h = lroundf(rect.getHeight()) /*+ 1*/;

    SDL_GetClipRect(target, &cliprect);  //get the current cliprect for the target

    //This intersection test would not normally be necessary since SDL_BlitSurface
    //  will only blit to the cliprect and skip others.
    //  The problem here is that we are zooming all surfaces before blitting, so
    //  even the clipped rects get zoomed.
    //  So, the solution is to do the clipping ourselves so that we don't zoom
    //  surfaces ultimately destined to be clipped.
    if(!RectIntersection(&drect, &cliprect))
        return;

    double zoomx = drect.w / textureSDL->getWidth();
    double zoomy = drect.h / textureSDL->getHeight();

    //This code caches zoomed surfaces so that they do not need to be zoomed each blit
    if(textureSDL->zoomSurface == NULL || zoomx != textureSDL->zoomx || zoomy != textureSDL->zoomy)
    {
        textureSDL->setZoomSurface(zoomSurface(textureSDL->surface, zoomx, zoomy, SMOOTHING_OFF), zoomx, zoomy);
    }
    SDL_BlitSurface(textureSDL->zoomSurface, 0, target, &drect);

/*
    //This was the original code that would zoom a surface, blit it, and then free it.
    SDL_Surface *tmp;
    tmp = zoomSurface(textureSDL->surface, zoomx, zoomy, SMOOTHING_OFF);
    SDL_BlitSurface(tmp, 0, target, &drect);
    SDL_FreeSurface(tmp);
*/
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
    filledPolygonRGBA( target, vx, vy, numberPoints,
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
    aapolygonRGBA( target, vx, vy, numberPoints,
            lineColor.r, lineColor.g, lineColor.b, lineColor.a);
    delete[] vx;
    delete[] vy;
}

void
PainterSDL::drawLine( const Vector2 pointA, const Vector2 pointB )
{
    Vector2 screenpos = transform.apply( pointA );
    Vector2 screenpos2 = transform.apply( pointB );
    aalineRGBA( target, (int) screenpos.x, (int) screenpos.y,
            (int) screenpos2.x, (int) screenpos2.y,
      lineColor.r, lineColor.g, lineColor.b, lineColor.a);


}

void
PainterSDL::fillRectangle(const Rect2D& rect)
{
    Vector2 screenpos = transform.apply(rect.p1);
    Vector2 screenpos2 = transform.apply(rect.p2);
    boxRGBA(target, (int) screenpos.x, (int) screenpos.y,
            (int) screenpos2.x, (int) screenpos2.y,
            fillColor.r, fillColor.g, fillColor.b, fillColor.a);
}

void
PainterSDL::drawRectangle(const Rect2D& rect)
{
    Vector2 screenpos = transform.apply(rect.p1);
    Vector2 screenpos2 = transform.apply(rect.p2);
    rectangleRGBA(target, (int) screenpos.x, (int) screenpos.y,
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
    SDL_SetClipRect(target, &cliprect);
}

void
PainterSDL::clearClipRectangle()
{
    SDL_SetClipRect(target, 0);
}

Painter*
PainterSDL::createTexturePainter(Texture* texture)
{
    assert(typeid(*texture) == typeid(TextureSDL));
    TextureSDL* textureSDL = static_cast<TextureSDL*> (texture);

    return new PainterSDL(textureSDL);
}


/** @file gui/PainterSDL/PainterSDL.cpp */

