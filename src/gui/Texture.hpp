#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include <SDL.h>

/**
 * Wrapper around a pixmap. Texture have to be created by the TextureManager
 * class
 */
class Texture
{
public:
    ~Texture();

    float getWidth() const 
    {
        return surface->w;
    }
    float getHeight() const
    {
        return surface->h;
    }
    
private:
    friend class Painter;
    friend class TextureManager;
    Texture(SDL_Surface* _surface)
        : surface(_surface)
    { }

    SDL_Surface* surface;
};

#endif
