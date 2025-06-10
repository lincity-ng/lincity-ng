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
 * @file TextureManager.cpp
 */

#include "TextureManager.hpp"

#include <SDL.h>                  // for SDL_GetError, SDL_Surface
#include <SDL_image.h>            // for IMG_Load
#include <cassert>                // for assert
#include <filesystem>             // for path, operator/
#include <iostream>               // for char_traits, basic_ostream, operator<<
#include <sstream>                // for basic_stringstream
#include <stdexcept>              // for runtime_error
#include <string>                 // for basic_string
#include <utility>                // for pair, make_pair

#include "Filter.hpp"             // for color2Grey
#include "lincity-ng/Config.hpp"  // for getConfig, Config

TextureManager* texture_manager = 0;

TextureManager::~TextureManager()
{
    for(Textures::iterator i = textures.begin(); i != textures.end(); ++i)
        delete i->second;
}

Texture*
TextureManager::load(const std::filesystem::path& filename, Filter filter)
{
    TextureInfo info;
    info.filename = getConfig()->appDataDir.get() / filename;
    info.filter = filter;

    Textures::iterator i = textures.find(info);
    if(i != textures.end()) {
        return i->second;
    }

    SDL_Surface* image = IMG_Load(info.filename.string().c_str());
    if(!image) {
        std::stringstream msg;
        msg << "couldn't load image: " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }

    switch(filter) {
        case FILTER_GREY:
            color2Grey(image);
            break;
        case NO_FILTER:
            break;
        default:
            std::cerr << "Unknown filter specified for image.\n";
            assert(false);
            break;
    }

    Texture* result = create(image);
    SDL_FreeSurface(image);
    textures.insert(std::make_pair(info, result));

    return result;
}


/** @file gui/TextureManager.cpp */
