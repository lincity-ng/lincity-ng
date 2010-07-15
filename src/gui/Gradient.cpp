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
 * @file Gradient.cpp
 */

#include <config.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <assert.h>

#include "Gradient.hpp"
#include "XmlReader.hpp"
#include "TextureManager.hpp"
#include "Painter.hpp"
#include "ComponentFactory.hpp"

#ifdef _MSC_VER
#define lrintf(x) (long int)x
#endif

Gradient::Gradient()
    : direction(LEFT_RIGHT)
{}

Gradient::~Gradient()
{}

void
Gradient::parse(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else if(strcmp(attribute, "from") == 0) {
            from.parse(value);
        } else if(strcmp(attribute, "to") == 0) {
            to.parse(value);
        } else if (strcmp(attribute, "direction") == 0) {
            if(strcmp(value, "left-right") == 0) {
                direction = LEFT_RIGHT;
            } else if (strcmp(value, "top-bottom") == 0) {
                direction = TOP_BOTTOM;
            } else {
                std::stringstream msg;
                msg << "Invalid gradient direction '" << value << "'.";
                throw std::runtime_error(msg.str());
            }   
        } else {
            std::cerr << "Skipping unknown attribute '"
                      << attribute << "'.\n";
        }
    }

    flags |= FLAG_RESIZABLE;
}

void
Gradient::resize(float width, float height)
{
    float w;
    if(direction == LEFT_RIGHT)
        w = width;
    else if(direction == TOP_BOTTOM)
        w = height;
    else
        assert(false);
    
    float dr = ((float) to.r - (float) from.r) / w;
    float dg = ((float) to.g - (float) from.g) / w;
    float db = ((float) to.b - (float) from.b) / w;
    float da = ((float) to.a - (float) from.a) / w;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                                (int) width, (int) height,
                                                32, 0xff000000,
                                                0x00ff0000,
                                                0x0000ff00,
                                                0x000000ff);
#else
    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                                (int) width, (int) height,
                                                32, 0x000000ff,
                                                0x0000ff00,
                                                0x00ff0000,
                                                0xff000000);
#endif
    if(surface == 0)
        throw std::runtime_error("Couldn't create SDL_Surface for gradient. "
                                 "(Out of memory?");

    float r = from.r;
    float g = from.g;
    float b = from.b;
    float a = from.a;
    if(direction == LEFT_RIGHT) {
        for(int x = 0; x < (int) width; ++x) {
            draw_vertical_line(surface, x, 0, (int) height,
                               lrintf(r), lrintf(g),
                               lrintf(b), lrintf(a));
            r += dr;
            g += dg;
            b += db;
            a += da;
        }
    } else {
        for(int y = 0; y < (int) height; ++y) {
            draw_horizontal_line(surface, 0, y, (int) width,
                                 lrintf(r), lrintf(g),
                                 lrintf(b), lrintf(a));
            r += dr;
            g += dg;
            b += db;
            a += da;
        }
    }

    texture.reset(texture_manager->create(surface));
    this->width = width;
    this->height = height;
}

void
Gradient::draw(Painter& painter)
{
    painter.drawTexture(texture.get(), Vector2(0, 0));
}

inline void
Gradient::draw_horizontal_line(SDL_Surface* surface, int x1, int y1, int x2,
                               uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    uint32_t col = (uint32_t) r << surface->format->Rshift
        | (uint32_t) g << surface->format->Gshift
        | (uint32_t) b << surface->format->Bshift
        | (uint32_t) a << surface->format->Ashift;
    
    uint8_t* pix = (uint8_t*) surface->pixels + (y1*surface->pitch) + x1*4;
    for(int x = x1; x < x2; ++x) {
        uint32_t* p = (uint32_t*) pix;
        *p = col;
        pix += 4;
    }
}

inline void
Gradient::draw_vertical_line(SDL_Surface* surface, int x1, int y1, int y2,
                             uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    uint32_t col = (uint32_t) r << surface->format->Rshift
        | (uint32_t) g << surface->format->Gshift
        | (uint32_t) b << surface->format->Bshift
        | (uint32_t) a << surface->format->Ashift;
    int pitch = surface->pitch;
    
    uint8_t* pix = (uint8_t*) surface->pixels + (y1*pitch) + x1*4;
    for(int y = y1; y < y2; ++y) {
        uint32_t* p = (uint32_t*) pix;
        *p = col;
        pix += pitch;
    }
}

IMPLEMENT_COMPONENT_FACTORY(Gradient)

/** @file gui/Gradient.cpp */

