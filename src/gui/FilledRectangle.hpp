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
 * @file FilledRectangle.hpp
 */

#ifndef __FILLEDRECTANGLE_HPP__
#define __FILLEDRECTANGLE_HPP__

#include "Component.hpp"
#include "Color.hpp"

class XmlReader;

/**
 * @class FilledRectangle
 */
class FilledRectangle : public Component
{
public:
    FilledRectangle();
    virtual ~FilledRectangle();

    void parse(XmlReader& reader);
    void resize(float width, float height);
    void draw(Painter& painter);
    
private:
    Color color;
};

#endif

/** @file gui/FilledRectangle.hpp */

