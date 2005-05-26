#ifndef __TEXTUREMANAGER_HPP__
#define __TEXTUREMANAGER_HPP__

#include <SDL.h>
#include <map>
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
    virtual ~TextureManager();
    
    enum Filter {
        NO_FILTER = 0,
        /// Turn image into a greyscale image
        FILTER_GREY
    };
    
    /**
     * load an image file from disk and create a texture. The texture will be
     * cached so don't delete it.
     */
    Texture* load(const std::string& filename, Filter filter = NO_FILTER);

    /**
     * Create a texture from an SDL_Surface. This function takes ownership of
     * the SDL_Surface and will free it.
     */
    virtual Texture* create(SDL_Surface* surface) = 0;

private:
    struct TextureInfo {
        std::string filename;
        Filter filter;

        TextureInfo()
            : filter(NO_FILTER)
        { }
        TextureInfo(const TextureInfo& other)
            : filename(other.filename), filter(other.filter)
        { }
        
        bool operator < (const TextureInfo& other) const
        {
            if(filename < other.filename)
                return true;
            if(filename > other.filename)
                return false;
            if((int) filter < (int) other.filter)
                return true;
            return false;
        }

        bool operator== (const TextureInfo& other) const
        {
            return filename == other.filename && filter == other.filter;
        }
    };
    
    typedef std::map<TextureInfo, Texture*> Textures;
    Textures textures;
};

extern TextureManager* texture_manager;

#endif
