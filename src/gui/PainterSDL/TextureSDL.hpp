#ifndef __TEXTURESDL_HPP__
#define __TEXTURESDL_HPP__

#include "gui/Texture.hpp"
#include <SDL.h>

/**
 * Wrapper around a pixmap. Texture have to be created by the TextureManager
 * class
 */
class TextureSDL : public Texture
{
public:
    virtual ~TextureSDL();

    float getWidth() const 
    {
        return surface->w;
    }
    float getHeight() const
    {
        return surface->h;
    }
    
private:
    friend class PainterSDL;
    friend class TextureManagerSDL;
    TextureSDL(SDL_Surface* _surface)
        : surface(_surface)
    { }

    SDL_Surface* surface;
};

#endif
