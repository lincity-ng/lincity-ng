#ifndef __TEXTUREMANAGERGL_HPP__
#define __TEXTUREMANAGERGL_HPP__

#include "gui/TextureManager.hpp"

class TextureManagerGL : public TextureManager
{
public:
    TextureManagerGL();
    virtual ~TextureManagerGL();

    Texture* load(const std::string& filename);
    Texture* create(SDL_Surface* surface);
};

#endif

