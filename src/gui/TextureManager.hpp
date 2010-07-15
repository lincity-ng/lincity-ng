/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 * @author Matthias Braun
 * @file TextureManager.hpp
 */

#ifndef __TEXTUREMANAGER_HPP__
#define __TEXTUREMANAGER_HPP__

#include <SDL.h>
#include <map>
#include <string>

#include "Texture.hpp"

/**
 * @class TextureManager
 * @brief This handles the creation and sharing of textures.
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
     * Load an image file from disk and create a texture. The texture will be
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

/** @file gui/TextureManager.hpp */

