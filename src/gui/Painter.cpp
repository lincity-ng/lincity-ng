#include <config.h>

#include <iostream>
#include <assert.h>

#include <SDL_opengl.h>
#include <SDL_gfxPrimitives.h>
#include "Painter.hpp"

Painter::Painter(SDL_Surface* _target)
    : target(_target)
{
}

Painter::~Painter()
{
}

void
Painter::drawTexture(const Texture* tex, Rectangle rect)
{
    if(tex == 0) {
        std::cerr << "Trying to render 0 texture.";
#ifdef DEBUG
        assert(false);
#endif
        return;
    }

#if 0
    if(rect.getWidth() != tex->surface->w
            || rect.getHeight() != tex->surface->h) {
        std::cerr << "Scaling not implemented for software renderer.\n";
        return;
    }
#endif
    
    Vector2 screenpos = transform.apply(rect.p1);
    
    SDL_Rect drect;
    drect.x = (int) screenpos.x;
    drect.y = (int) screenpos.y;
    SDL_BlitSurface(tex->surface, 0, target, &drect);
}

void
Painter::fillRectangle(Rectangle rect)
{
    Vector2 screenpos = transform.apply(rect.p1);
    
    SDL_Rect drect;
    drect.x = (int) screenpos.x;
    drect.y = (int) screenpos.y;
    drect.w = (int) rect.getWidth();
    drect.h = (int) rect.getHeight();

    SDL_FillRect(target, &drect, fillColor);
    
}

void
Painter::drawRectangle(Rectangle rect)
{
    Vector2 screenpos = transform.apply(rect.p1);
    Vector2 screenpos2 = transform.apply(rect.p2);
    rectangleRGBA(target, (int) screenpos.x, (int) screenpos.y,
            (int) screenpos2.x, (int) screenpos2.y,
            lineColor.r, lineColor.g, lineColor.b, lineColor.a);
}

void
Painter::setFillColor(Color color)
{
    fillColor = SDL_MapRGBA(target->format, color.r, color.g, color.b, color.a);
}

void
Painter::setLineColor(Color color)
{
    lineColor = color;
}

void
Painter::translate(const Vector2& vec)
{
    transform.translation -= vec;
}

void
Painter::pushTransform()
{
    transformStack.push_back(transform);
}

void
Painter::popTransform()
{
    transform = transformStack.back();
    transformStack.pop_back();
}

void
Painter::setClipRectangle(Rectangle rect)
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
Painter::clearClipRectangle()
{
    SDL_SetClipRect(target, 0);
}
