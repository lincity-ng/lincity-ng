#include <config.h>

#include "PainterSDL.hpp"

#include <iostream>
#include <assert.h>
#include <typeinfo>
#include <SDL_opengl.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>

#include "TextureSDL.hpp"

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

void
PainterSDL::drawTexture(const Texture* texture, const Vector2& pos)
{
    assert(typeid(*texture) == typeid(TextureSDL));
    const TextureSDL* textureSDL = static_cast<const TextureSDL*> (texture);
    
    if(texture == 0) {
        std::cerr << "Trying to render 0 texture.";
#ifdef DEBUG
        assert(false);
#endif
        return;
    }

    Vector2 screenpos = transform.apply(pos);
    
    SDL_Rect drect;
    drect.x = (int) screenpos.x;
    drect.y = (int) screenpos.y;
    SDL_BlitSurface(textureSDL->surface, 0, target, &drect);
}

void
PainterSDL::drawStretchTexture(const Texture* texture, const Rect2D& rect)
{
    assert(typeid(*texture) == typeid(TextureSDL));
    const TextureSDL* textureSDL = static_cast<const TextureSDL*> (texture);
    
    if(texture == 0 || texture->getWidth() == 0 || texture->getHeight() == 0) {
        std::cerr << "Trying to render 0 texture.";
#ifdef DEBUG
        assert(false);
#endif
        return;
    }

    Vector2 screenpos = transform.apply(rect.p1);
    
    SDL_Rect drect;
    drect.x = (int) screenpos.x;
    drect.y = (int) screenpos.y;
    drect.w = (int) rect.getWidth();
    drect.h = (int) rect.getHeight();
    
    double zoomx = drect.w / textureSDL->getWidth();
    double zoomy = drect.h / textureSDL->getHeight(); 
    SDL_Surface *tmp 
        = zoomSurface(textureSDL->surface, zoomx, zoomy, SMOOTHING_OFF);
    SDL_BlitSurface(tmp, 0, target, &drect);
    SDL_FreeSurface(tmp);
}

#if 0
void
PainterSDL::fillDiamond(Rect2D rect)
{
    Vector2 p1( rect.p1.x + ( rect.getWidth() / 2 ), rect.p1.y );
    Vector2 p2( rect.p1.x, rect.p1.y + ( rect.getHeight() / 2 ) );
    Vector2 p3( rect.p1.x + ( rect.getWidth() / 2 ), rect.p2.y );
    Vector2 p4( rect.p2.x, rect.p1.y + ( rect.getHeight() / 2 ) );
    Vector2 screenpos = transform.apply(p1);
    Vector2 screenpos2 = transform.apply(p2);
    Vector2 screenpos3 = transform.apply(p3);
    Vector2 screenpos4 = transform.apply(p4);
    Sint16 vx[ 4 ], vy[ 4 ];
    vx[ 0 ] = (int) screenpos.x;
    vx[ 1 ] = (int) screenpos2.x;
    vx[ 2 ] = (int) screenpos3.x;
    vx[ 3 ] = (int) screenpos4.x;
    vy[ 0 ] = (int) screenpos.y;
    vy[ 1 ] = (int) screenpos2.y;
    vy[ 2 ] = (int) screenpos3.y;
    vy[ 3 ] = (int) screenpos4.y;
    
    filledPolygonRGBA( target, vx, vy, 4 ,
            fillColor.r, fillColor.g, fillColor.b, fillColor.a);
}
#endif

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

