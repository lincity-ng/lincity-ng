#include <config.h>

#include <SDL_image.h>
#include <SDL_opengl.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <assert.h>

#include "TextureManagerSDL.hpp"
#include "TextureSDL.hpp"
#include "gui/Filter.hpp"
#include "PhysfsStream/PhysfsSDL.hpp"

TextureManagerSDL::TextureManagerSDL()
{}

TextureManagerSDL::~TextureManagerSDL()
{}

Texture*
TextureManagerSDL::create(SDL_Surface* image)
{
    return new TextureSDL(image);
}

Texture*
TextureManagerSDL::load(const std::string& filename, Filter filter)
{
    SDL_Surface* image = IMG_Load_RW(getPhysfsSDLRWops(filename), 1);
    if(!image) {
        std::stringstream msg;
        msg << "Couldn't load image '" << filename
            << "' :" << SDL_GetError();
        throw std::runtime_error(msg.str());
    }

    switch(filter) {
        case NO_FILTER:
            break;
        case FILTER_GREY:
            color2Grey(image);
            break;
        default:
            std::cerr << "Unknown filter specified for image.\n";
#ifdef DEBUG
            assert(false);
#endif
            break;
    }

    SDL_Surface* surface = SDL_DisplayFormatAlpha(image);
    SDL_FreeSurface(image);

    Texture* result = create(surface);
    return result;
}

