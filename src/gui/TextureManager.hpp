#ifndef __TEXTUREMANAGER_HPP__
#define __TEXTUREMANAGER_HPP__

#include <SDL.h>
#include <string>

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
    virtual ~TextureManager()
    {}
    
    enum Filter {
        NO_FILTER,
        /// Turn image into a greyscale image
        FILTER_GREY
    };
    
    /** load an image file from disk and create a texture */
    virtual Texture* load(const std::string& filename,
            Filter filter = NO_FILTER) = 0;
    /** Create a texture from an SDL_Surface. This function takes ownership of
     * the SDL_Surface and will free it.
     */
    virtual Texture* create(SDL_Surface* surface) = 0;
};

extern TextureManager* texture_manager;

#endif
