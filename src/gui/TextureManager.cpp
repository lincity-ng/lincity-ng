#include <config.h>

#include <SDL_image.h>
#include <SDL_opengl.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "TextureManager.hpp"

TextureManager* texture_manager = 0;

TextureManager::TextureManager()
{}

TextureManager::~TextureManager()
{}

Texture*
TextureManager::create(SDL_Surface* image)
{
    return new Texture(image);
}

Texture*
TextureManager::load(const std::string& filename)
{
    // hack until we have proper resource management
    std::string real_filename = "data/" + filename;
    
    SDL_Surface* image = IMG_Load(real_filename.c_str());
    if(!image) {
        std::stringstream msg;
        msg << "Couldn't load image '" << filename
            << "' :" << SDL_GetError();
        throw std::runtime_error(msg.str());
    }

    Texture* result = create(image);
    return result;
}

