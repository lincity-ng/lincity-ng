#ifndef __TEXTUREMANAGERSDL_HPP__
#define __TEXTUREMANAGERSDL_HPP__

#include "gui/TextureManager.hpp"

/**
 * This handles the creation and sharing of textures.
 * 
 * A texture is a wrapper around a pixmaps. Using a wrapper allows to make use
 * of hardware acceleration in some scenarios (for example in opengl where the
 * pixmaps can get uploaded into the gfx cards memory).
 */
class TextureManagerSDL : public TextureManager
{
public:
    TextureManagerSDL();
    virtual ~TextureManagerSDL();

    Texture* load(const std::string& filename, Filter filter);
    Texture* create(SDL_Surface* surface);
};

#endif
