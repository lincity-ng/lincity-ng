#include <config.h>

#include "Filter.hpp"

#include <stdint.h>
#include <stdexcept>

void color2Grey(SDL_Surface* surface)
{
    if(surface->format->BitsPerPixel != 32)
        throw std::runtime_error("Can only convert 32 bit images to greyscale");
    
    SDL_LockSurface(surface);
    
    uint8_t* p = (uint8_t*) surface->pixels;
    for(int y = 0; y < surface->h; ++y) {
        uint32_t* pixel = (uint32_t*) p;
        for(int x = 0; x < surface->w; ++x) {
            float red 
                = (*pixel & surface->format->Rmask) >> surface->format->Rshift;
            float green
                = (*pixel & surface->format->Gmask) >> surface->format->Gshift;
            float blue
                = (*pixel & surface->format->Bmask) >> surface->format->Bshift;
            
            float greyvalf = 0.3 * red + 0.59 * green + 0.11 * blue;
            uint32_t greyval = (uint32_t) greyvalf;
            *pixel = *pixel & surface->format->Amask 
                | (greyval << surface->format->Rshift)
                | (greyval << surface->format->Gshift)
                | (greyval << surface->format->Bshift);
            pixel++;
        }
        p += surface->pitch;
    }
    SDL_UnlockSurface(surface);
}

