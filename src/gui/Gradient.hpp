/* ---------------------------------------------------------------------- *
 * src/gui/Gradient.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Matthias Braun <matze@braunis.de>
 * Copyright (C) 2026      David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#ifndef __GRADIENT_HPP__
#define __GRADIENT_HPP__

#include <memory>         // for unique_ptr

#include "Color.hpp"      // for Color
#include "Component.hpp"  // for Component
#include "Event.hpp"

class Texture;
namespace xmlpp {
class TextReader;
}  // namespace xmlpp

/**
 * @class Gradient
 */
class Gradient : public Component
{
public:
    Gradient();
    virtual ~Gradient();

    void parse(xmlpp::TextReader& reader);
    void resize(float width, float height) override;
    void draw(Painter& painter) override;
    void event(const Event& event) override;

private:

    std::unique_ptr<Texture> texture;
    Color from, to;
    float angle;
};

#endif


/** @file gui/Gradient.hpp */
