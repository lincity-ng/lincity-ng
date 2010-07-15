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

#include <config.h>

#include "TextureManager.hpp"

#include <SDL_image.h>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <assert.h>
#include "gui/Filter.hpp"
#include "PhysfsStream/PhysfsSDL.hpp"

TextureManager* texture_manager = 0;

TextureManager::~TextureManager()
{
    for(Textures::iterator i = textures.begin(); i != textures.end(); ++i)
        delete i->second;
}

Texture*
TextureManager::load(const std::string& filename, Filter filter)
{
    TextureInfo info;
    info.filename = filename;
    info.filter = filter;

    Textures::iterator i = textures.find(info);
    if(i != textures.end()) {
        return i->second;
    }
    
    SDL_Surface* image = IMG_Load_RW(getPhysfsSDLRWops(filename), 1);
    if(!image) {
        std::stringstream msg;
        msg << "Couldn't load image '" << filename
            << "' :" << SDL_GetError();
        throw std::runtime_error(msg.str());
    }

    switch(filter) {
        case FILTER_GREY:
            color2Grey(image);
            break;
        case NO_FILTER:
            break;
        default:
#ifdef DEBUG
            assert(false);
#endif
            std::cerr << "Unknown filter specified for image.\n";
            break;
    }

    Texture* result = create(image);
    textures.insert(std::make_pair(info, result));
    
    return result;
}


/** @file gui/TextureManager.cpp */

