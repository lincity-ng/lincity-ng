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
 * @file Gradient.hpp
 */

#ifndef __GRADIENT_HPP__
#define __GRADIENT_HPP__

#include "Component.hpp"
#include <memory>
#include <stdint.h>
#include "Color.hpp"

class XmlReader;
class Texture;

/**
 * @class Gradient
 */
class Gradient : public Component
{
public:
    Gradient();
    virtual ~Gradient();

    void parse(XmlReader& reader);
    void resize(float width, float height);
    void draw(Painter& painter);

private:
    void draw_horizontal_line(SDL_Surface* surface, int x1, int y1, int x2,
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void draw_vertical_line(SDL_Surface* surface, int x1, int y1, int y2,
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    
    std::auto_ptr<Texture> texture;
    Color from, to;
    enum Direction {
        LEFT_RIGHT,
        TOP_BOTTOM
    };
    Direction direction;
};

#endif


/** @file gui/Gradient.hpp */

