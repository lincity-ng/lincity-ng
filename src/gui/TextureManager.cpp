#include <config.h>

#include "TextureManager.hpp"

#include <SDL_image.h>
#include <sstream>
#include <stdexcept>
#include <iostream>
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

