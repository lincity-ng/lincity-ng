#ifndef __TEXTUREMANAGER_HPP__
#define __TEXTUREMANAGER_HPP__

#include <SDL.h>
#include <string>

#include "Rect2D.hpp"
#include "Texture.hpp"

/**
 * This handles the creation and sharing of textures.
 * 
 * A texture is a wrapper around a pixmaps. Using a wrapper allows to make use
 * of hardware acceleration in some scenarios (for example in opengl where the
 * pixmaps can get uploaded into the gfx cards memory).
 */
class TextureManager
{
public:
    TextureManager();
    ~TextureManager();

    Texture* load(const std::string& filename);
    Texture* create(SDL_Surface* surface);
};

extern TextureManager* texture_manager;

#endif
