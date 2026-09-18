/* ---------------------------------------------------------------------- *
 * src/gui/Image.hpp
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

#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <filesystem>

#include "Component.hpp"  // for Component

class Texture;
namespace xmlpp {
class TextReader;
}  // namespace xmlpp

/**
 * @class Image
 */
class Image : public Component
{
public:
    Image();
    virtual ~Image();

    void parse(xmlpp::TextReader& reader);

    void resize(float width, float height);
    void draw(Painter& painter);

    std::filesystem::path getFilename() const;
    void setFile(const std::filesystem::path &filename);

private:
    Texture *texture;
    std::filesystem::path filename;
    bool lockRatio;
};

#endif


/** @file gui/Image.hpp */
