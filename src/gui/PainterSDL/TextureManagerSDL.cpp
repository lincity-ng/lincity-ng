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

static const Uint8 ALPHA_BARRIER = 100;

TextureManagerSDL::TextureManagerSDL()
{}

TextureManagerSDL::~TextureManagerSDL()
{}

Texture*
TextureManagerSDL::create(SDL_Surface* image)
{
    // The code below transforms images with alpha channel to color keyed
    // images. But I disabled it again, because it seems to make the rendering
    // actually slower on my machine which is very odd...

#if 0
    if(image->format->BitsPerPixel != 32)
        throw std::runtime_error("Only 32bit images supported");
    
    Uint32 colorkey = SDL_MapRGB(image->format, 255, 0, 254);
    
    // convert alpha channel to colorkey...
    SDL_LockSurface(image);
    
    Uint8* p = (uint8_t*) image->pixels;
    for(int y = 0; y < image->h; ++y) {
        Uint32* pixel = (uint32_t*) p;
        for(int x = 0; x < image->w; ++x) {
            Uint8 alpha 
                = (*pixel & image->format->Amask) >> image->format->Ashift;
            if(alpha <= ALPHA_BARRIER) {
                *pixel = colorkey;
            }
            pixel++;
        }
        p += image->pitch;
    }
    SDL_UnlockSurface(image);
    
    //SDL_SetAlpha(image, SDL_SRCCOLORKEY, 0);
    //SDL_SetColorKey(image, SDL_SRCCOLORKEY, colorkey);
    SDL_Surface* surface = SDL_DisplayFormatAlpha(image);
    SDL_FreeSurface(image);

    SDL_SetAlpha(surface, 0, 0);
    SDL_SetColorKey(surface, SDL_SRCCOLORKEY,
            SDL_MapRGB(surface->format, 255, 0, 254));
#else
    SDL_Surface* surface = SDL_DisplayFormatAlpha(image);
    SDL_FreeSurface(image);
#endif

    return new TextureSDL(surface);
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

    Texture* result = create(image);
    return result;
}

