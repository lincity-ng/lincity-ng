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
#include <config.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <assert.h>

#include "TextureManagerSDL.hpp"
#include "TextureSDL.hpp"

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


/** @file gui/PainterSDL/TextureManagerSDL.cpp */

