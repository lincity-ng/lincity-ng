#ifndef __TEXTUREMANAGERGL_HPP__
#define __TEXTUREMANAGERGL_HPP__

#include "gui/TextureManager.hpp"

class TextureManagerGL : public TextureManager
{
public:
    TextureManagerGL();
    virtual ~TextureManagerGL();

    Texture* create(SDL_Surface* surface);
};

#endif

